from flask import Flask, request, jsonify, Response

from flask_restplus import Resource, Api, reqparse, abort, fields
from flask_sqlalchemy import SQLAlchemy

import sqlite3

import requests
import json
import io
import datetime
import uuid

def create_db(db_file):
    '''
    use this function to create a db, don't change the name of this function.
    db_file: Your database's name.
    '''
    try:
        conn = sqlite3.connect(db_file)
    except sqlite3.Error as e:
        print(e)
    finally:
        conn.close()
        
def getJsonContent(indicator_id):
    url = 'http://api.worldbank.org/v2/countries/all/indicators/' + indicator_id + '?per_page=2000&date=2013:2018&format=json'

    response = requests.get(url)
    jsonContent = json.load(io.BytesIO(response.content))

    try:
        if indicator_id != 'favicon.ico' and jsonContent[0]['pages'] > 1:
            pages = jsonContent[0]['pages']

            for page in range(2,pages+1):
                url = 'http://api.worldbank.org/v2/countries/all/indicators/' + indicator_id + '?per_page=2000&date=2013:2018&format=json&page=' + str(page) 
                response = requests.get(url)
                jsonContent = json.load(io.BytesIO(response.content))
        return jsonContent
    except KeyError:
        abort(404, message="Indicator_id {} not found!".format(indicator_id))

def cleanJSON(jsonContent):
    ret = dict()
    ret['indicator'] = jsonContent[1][0]['indicator']['id']
    ret['indicator_value'] = jsonContent[1][0]['indicator']['value']

    ret['entries'] = list()
    for item in jsonContent[1]:
        ret['entries'].append({'country':item['country']['value'], 'date':item['date'], 'value':item['value']})

    return ret
    
def loadJSON(jsonContent, creation_time, collection_id, location):
    indicator = jsonContent['indicator']
    indicator_value = jsonContent['indicator_value']
    
    indicatorExistsCheck = db.session.query(Collection).filter(Collection.indicator == indicator, Collection.location == location)
    if indicatorExistsCheck.first():
        return 200, indicatorExistsCheck.first().collection_id, indicatorExistsCheck.first().creation_time
        
    
    nextItem = Collection(creation_time = creation_time, 
                             collection_id = collection_id, 
                             indicator = indicator,
                             indicator_value = indicator_value,
                             location = location)
    db.session.add(nextItem)

    for item in jsonContent['entries']:
        nextItem = Entry(    collection_id = collection_id, 
                             country = item['country'],
                             date = item['date'],
                             value = item['value'])
        db.session.add(nextItem)
        
    db.session.commit()
    return 201, None

app = Flask(__name__)
create_db('data.db')

app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///data.db'
db = SQLAlchemy(app)

class Collection(db.Model):
    location = db.Column(db.String(20), nullable=False)
    collection_id = db.Column(db.String(36), primary_key=True)
    creation_time = db.Column(db.DateTime, nullable=False)
    indicator = db.Column(db.String(20), nullable=False)
    indicator_value = db.Column(db.String(20), nullable=False)
    
    entries = db.relationship('Entry', backref='collection')
        
    @property
    def serialize(self):
        return {'collection_id': self.collection_id,
                'indicator': self.indicator,
                'indicator_value': self.indicator_value,
                'creation_time': self.creation_time}
    
    def get_repr(self, collection):
        return {"location" : "/" + self.location + "/" + self.collection_id, 
                "collection_id" : self.collection_id,  
                "creation_time": self.creation_time.strftime('%Y-%m-%dT%H:%M:%SZ'),
                "indicator" : self.indicator}

    def __repr__(self):
        return str(self.get_repr)
    
    
class Entry(db.Model):
    collection_id = db.Column(db.String(36), db.ForeignKey('collection.collection_id', ondelete='CASCADE'), primary_key=True)
    country = db.Column(db.String(20), primary_key=True)
    date = db.Column(db.String(4), primary_key=True)
    value = db.Column(db.Float)
    
    @property
    def serialize(self):
        return {'country': self.country,
                'date': self.date,
                'value': self.value}

    def __repr__(self):
        return '<Country: {}, Date: {}, Value: {}>'.format(self.country, self.date, self.value)
    
db.create_all()
api = Api(app, version='1.0', title='Data Service for World Bank Economic Indicators',
    description='Student ID: z3218424\nAssignment 2')

expected_fields = api.model('Resource', {
    'indicator_id': fields.String,
})

@api.route('/<collections>')
@api.doc(params={'collections': 'Your resource name'})
class Collection_Endpoint(Resource):
    @api.doc(description='Question 1- Import a collection from the data service')
    @api.expect(expected_fields)
    def post(self, collections):
        indicator_id = api.payload['indicator_id']
        jsonContent = getJsonContent(indicator_id)
        cleanedContent = cleanJSON(jsonContent)
        
        creation_time = datetime.datetime.utcnow()
        collection_id = str(uuid.uuid4())

        retStatus = loadJSON(cleanedContent, creation_time, collection_id, collections)
        if retStatus[1]:
            collection_id = retStatus[1]
            creation_time = retStatus[2]

        return Response(json.dumps({'location' : '/' + collections + '/' + collection_id, 
                'collection_id' : collection_id,  
                'creation_time': creation_time.strftime('%Y-%m-%dT%H:%M:%SZ'),
                'indicator' : indicator_id}), status=retStatus[0], mimetype='application/json')
    
    @api.doc(description='Question 3 - Retrieve the list of available collections')
    def get(self, collections):
        return jsonify(list(x.get_repr(collections) for x in Collection.query
                            .filter(Collection.location == collections).all()))


@api.route('/<collections>/<collection_id>')
@api.doc(params={'collections': 'Your resource name', 'collection_id': 'Your collection ID'})
class Collection_ID_Endpoint(Resource):
    @api.doc(description='Question 2- Deleting a collection with the data service')
    def delete(self, collections, collection_id):
        query_result = db.session.query(Collection).filter(Collection.collection_id == collection_id, Collection.location == collections)
    
        if query_result.first():
            query_result.delete()
            db.session.commit()
            return jsonify({'message' :'Collection = ' + collection_id + 'is removed from the database!'})
        else:
            abort(404, message="ID {} doesn't exist in resource {}".format(collection_id, collections))
        
    @api.doc(description='Question 4 - Retrieve a collection')
    def get(self, collections, collection_id):
        try:
            collections_q = Collection.query.filter(Collection.collection_id == collection_id, Collection.location == collections).all()[0].serialize
        except IndexError:
            abort(404, message="ID {} doesn't exist in resource {}".format(collection_id, collections))
            
        entries = list(x.serialize for x in Entry.query.join(Collection)
                       .filter(Entry.collection_id == collection_id, Collection.location == collections).all())
        
        collections_q['entries'] = entries
        
        return jsonify(collections_q)
    
@api.route('/<collections>/<collection_id>/<year>/<country>')
@api.doc(params={'collections': 'Your resource name', 'collection_id': 'Your collection ID','year': '4 character year - 2013-2018','country': 'Country name, space separated'})
class Country_Year_Endpoint(Resource):
    @api.doc(description='Question 5 - Retrieve economic indicator value for given country and a year')
    def get(self, collections, collection_id, year, country):
        try:
            collections_q = Collection.query.filter(Collection.collection_id == collection_id, Collection.location == collections).all()[0].serialize
        except IndexError:
            abort(404, message="ID {} doesn't exist in resource {}".format(collection_id, collections))
            
        entries = list(x.serialize for x in Entry.query.join(Collection)
                       .filter(Entry.collection_id == collection_id,
                                                       ((Entry.country == country.title()) | (Entry.country == country)),
                                                       Entry.date == year,
                                                       Collection.location == collections
                                                        ).all())

        del collections_q['indicator_value']
        del collections_q['creation_time']
        
        if not entries:
            abort(404, message="No such records found in: /{}/{}".format(collection, collection_id))
        
        collections_q['country'] = entries[0]['country']
        collections_q['year'] = entries[0]['date']
        collections_q['value'] = entries[0]['value']
        
        
        return jsonify(collections_q)
    
@api.route('/<collections>/<collection_id>/<year>')
@api.doc(params={'collections': 'Your resource name', 'collection_id': 'Your collection ID','year': '4 character year - 2013-2018'})
class Year_Endpoint(Resource):
    parser = reqparse.RequestParser()
    parser.add_argument('q', type=str, help='Optional: topN or bottomN entries by value (up to 999). E.g. top5, bottom1')
    
    @api.expect(parser)
    @api.doc(description='Question 6 - Retrieve top/bottom economic indicator values for a given year')
    def get(self, collections, collection_id, year):    

        q = self.parser.parse_args()['q']

        try:
            collections_q = Collection.query.filter(Collection.collection_id == collection_id, Collection.location == collections).all()[0].serialize
        except IndexError:
            abort(404, message="ID {} doesn't exist in resource {}".format(collection_id, collections))
        
        if (q):
            try:
                if q[:3] == 'top':
                    limit = int(q[-(len(q)-3):])
                    entries = list(x.serialize for x in Entry.query.join(Collection)
                                .filter(Entry.date == year, Collection.location == collections)
                                .order_by(Entry.value.is_(None), Entry.value.desc()).limit(limit).all()) 
                elif q[:6] == 'bottom':
                    limit = int(q[-(len(q)-6):])
                    entries = list(x.serialize for x in Entry.query.join(Collection)
                                .filter(Entry.date == year, Collection.location == collections)
                                .order_by(Entry.value.is_(None), Entry.value).limit(limit).all())  
                else:
                    abort(404, message="q: {} is invalid.".format(q))
            except ValueError:
                abort(404, message="q: {} is invalid.".format(q))
            # except TypeError:
            #     abort(404, message="Please pass a value for parameter q.")
        else:
            entries = list(x.serialize for x in Entry.query.join(Collection)
                                .filter(Entry.date == year, Collection.location == collections).all())  

        
        del collections_q['collection_id']
        del collections_q['creation_time']
        
        try:
            collections_q['entries'] = entries
        except UnboundLocalError:
            abort(404, message="q: {} is invalid.".format(q))
        
        return jsonify(collections_q)

if __name__ == '__main__':
    app.run()
