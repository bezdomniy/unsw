'''
COMP9321 Assignment One Code Template 2019T1
Name: Ilia Chibaev
Student ID: z3218424
'''

import pandas as pd
import csv
import re

def dropMissing(data):
    data = data.replace({"Unknown":None,"-":None})
    return data.dropna()

def cleanData(data):
    def titleise(text, exceptions1, ):
        return ' '.join([word if (word[:2] in exceptions[0] or (word[:2] in exceptions[1] and len(word) == 2)) else 
                         word.title() for word in text.split()])

    exceptions = [{'d\'' , 'l\''}, {'la', 'de'}]
    return data.applymap(lambda s:titleise(s, exceptions) if type(s) == str else s)

def q1():
    '''
    Put Your Question 1's code in this function
    '''
    data = pd.read_csv('accidents_2017.csv', float_precision='high')
    data = cleanData(data)
    print(data.head(10).to_csv(sep=' ',index=False))

def q2():
    '''
    Put Your Question 2's code in this function 
    '''
    data = pd.read_csv('accidents_2017.csv', float_precision='high')
    data = dropMissing(data)
    data = cleanData(data)
    data.to_csv("result_q2.csv",quoting=csv.QUOTE_NONNUMERIC,index=False)

def q3():
    '''
    Put Your Question 3's code in this function 
    '''
    data = pd.read_csv('accidents_2017.csv',index_col='Id', float_precision='high')
    data = dropMissing(data)
    data = cleanData(data)

    data2 = data.groupby(['District Name']).size().sort_values(ascending=False).to_frame().reset_index()
    data2.columns = ['District Name', 'Total numbers of accidents']
    print(data2.to_csv(sep=' ',index=False))

def q4():
    '''
    Put Your Question 4's code in this function 
    '''
    
    ## 1
    airStations = pd.read_csv('air_stations_Nov2017.csv')
    airStations = cleanData(airStations)
    print(airStations[['Station','District Name']].to_json(orient='records'))

    ## 2
    airQuality = pd.read_csv('air_quality_Nov2017.csv')
    airQuality = dropMissing(airQuality)
    print(airQuality.loc[airQuality['Air Quality'] != 'Good'].head(10).to_csv(sep=' ',index=False))

    ## 3
    airQuality['Hour'] = airQuality.apply(lambda row: pd.to_datetime(row['Generated'], dayfirst=True).hour, axis=1)
    airQuality['Day'] = airQuality.apply(lambda row: pd.to_datetime(row['Generated'], dayfirst=True).day, axis=1)
    airQuality['Month'] = airQuality.apply(lambda row: pd.to_datetime(row['Generated'], dayfirst=True).month, axis=1)

    airQuality = airQuality.merge(airStations[['Station','District Name']])

    accidents = pd.read_csv('accidents_2017.csv', float_precision='high')
    accidents = dropMissing(accidents)
    accidents = cleanData(accidents)

    monthsDict = {'January':1,'February':2,'March':3,'April':4,'May':5,'June':6,'July':7,
                  'August':8,'September':9,'October':10,'November':11,'December':12}
    accidents['MonthNumber'] = accidents.apply(lambda row: monthsDict[row['Month']], axis=1)

    accidents_airQuality = accidents.merge(airQuality[['Day', 'Month', 'Hour','District Name','Air Quality']],
                                           left_on=['Day', 'MonthNumber', 'Hour','District Name'],
                                           right_on=['Day', 'Month', 'Hour','District Name'])

    accidents_airQuality = accidents_airQuality.rename(columns={'Month_x': 'Month'})

    accidents_badAirQuality = accidents_airQuality.loc[accidents_airQuality['Air Quality'] != 'Good']

    accidents_badAirQuality.drop(['Air Quality','Month_y','MonthNumber'],
                                 axis=1).to_csv("result_q4.csv",quoting=csv.QUOTE_NONNUMERIC,index=False)


def q5():
    '''
    Bonus Question(Optional).
    Put Your Question 5's code in this function.
    '''
    # %matplotlib notebook
    import matplotlib.pyplot as plt
    geoData = pd.read_csv('accidents_2017.csv', float_precision='high')
    geoData = dropMissing(geoData)

    import matplotlib.image as mpimg
    mapImg=mpimg.imread('Map.png')

    ax = geoData.plot(kind="scatter", x="Longitude", y="Latitude", figsize=(10,10),
                       s=(geoData['Vehicles involved']**3)/20, label="Population",
                       c="Victims", cmap=plt.get_cmap("OrRd"),
                       colorbar=False, alpha=0.2
                      )

    # plt.imshow(mapImg, extent=[1.916805, 2.42321, 41.282911,41.493609], alpha=1)
    plt.imshow(mapImg, extent=[2.006874, 2.334000, 41.314292,41.462289], alpha=1)
    plt.ylabel("Latitude", fontsize=14)
    plt.xlabel("Longitude", fontsize=14)

    plt.legend(fontsize=16)
    plt.show()