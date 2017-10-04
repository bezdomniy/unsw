import numpy as np
import glob #this will be useful when reading reviews from file
import os
import tarfile
import string
import tensorflow as tf

os.environ['TF_CPP_MIN_LOG_LEVEL']='2'
batch_size = 50

def load_data(glove_dict):
    """
    Take reviews from text files, vectorize them, and load them into a
    numpy array. Any preprocessing of the reviews should occur here. The first
    12500 reviews in the array should be the positive reviews, the 2nd 12500
    reviews should be the negative reviews.
    RETURN: numpy array of data with each row being a review in vectorized
    form"""
    """Extract data from tarball and store as list of strings"""
    if not os.path.exists(os.path.join(os.path.dirname(__file__), 'data2/')):
        with tarfile.open('reviews.tar.gz', "r") as tarball:
            dir = os.path.dirname(__file__)
            tarball.extractall(os.path.join(dir, 'data2/'))
    
    print("READING DATA")
    data = np.zeros((25000,40), dtype=np.int)

    dir = os.path.dirname(__file__)
    file_list = glob.glob(os.path.join(dir,
                                        'data2/pos/*'))
    file_list.extend(glob.glob(os.path.join(dir,
                                        'data2/neg/*')))
    print("Parsing %s files" % len(file_list))
    for i in range(len(file_list)):
        with open(file_list[i], "r",  encoding="utf-8") as openf:
            s = openf.read()

            # Replace punctuation with space
            punc = string.punctuation.replace("'",'')
            translator = s.maketrans(punc, ' '*len(punc))
            no_punct = s.lower().translate(translator)

            no_punct = ''.join(c.lower() for c in no_punct if c != "'")
            no_punct = no_punct.split()

            # stopwords from: http://www.lextek.com/manuals/onix/stopwords1.html
            stop_words = set(['about',
                                                        'above',
                                                        'across',
                                                        'after',
                                                        'again',
                                                        'against',
                                                        'all',
                                                        'almost',
                                                        'alone',
                                                        'along',
                                                        'already',
                                                        'also',
                                                        'although',
                                                        'always',
                                                        'among',
                                                        'an',
                                                        'and',
                                                        'another',
                                                        'any',
                                                        'anybody',
                                                        'anyone',
                                                        'anything',
                                                        'anywhere',
                                                        'are',
                                                        'area',
                                                        'areas',
                                                        'around',
                                                        'as',
                                                        'ask',
                                                        'asked',
                                                        'asking',
                                                        'asks',
                                                        'at',
                                                        'away',
                                                        'b',
                                                        'back',
                                                        'backed',
                                                        'backing',
                                                        'backs',
                                                        'be',
                                                        'became',
                                                        'because',
                                                        'become',
                                                        'becomes',
                                                        'been',
                                                        'before',
                                                        'began',
                                                        'behind',
                                                        'being',
                                                        'beings',
                                                        'best',
                                                        'better',
                                                        'between',
                                                        'big',
                                                        'both',
                                                        'but',
                                                        'by',
                                                        'c',
                                                        'came',
                                                        'can',
                                                        'cannot',
                                                        'case',
                                                        'cases',
                                                        'certain',
                                                        'certainly',
                                                        'clear',
                                                        'clearly',
                                                        'come',
                                                        'could',
                                                        'd',
                                                        'did',
                                                        'differ',
                                                        'different',
                                                        'differently',
                                                        'do',
                                                        'does',
                                                        'done',
                                                        'down',
                                                        'down',
                                                        'downed',
                                                        'downing',
                                                        'downs',
                                                        'during',
                                                        'e',
                                                        'each',
                                                        'early',
                                                        'either',
                                                        'end',
                                                        'ended',
                                                        'ending',
                                                        'ends',
                                                        'enough',
                                                        'even',
                                                        'evenly',
                                                        'ever',
                                                        'every',
                                                        'everybody',
                                                        'everyone',
                                                        'everything',
                                                        'everywhere',
                                                        'f',
                                                        'face',
                                                        'faces',
                                                        'fact',
                                                        'facts',
                                                        'far',
                                                        'felt',
                                                        'few',
                                                        'find',
                                                        'finds',
                                                        'first',
                                                        'for',
                                                        'four',
                                                        'from',
                                                        'full',
                                                        'fully',
                                                        'further',
                                                        'furthered',
                                                        'furthering',
                                                        'furthers',
                                                        'g',
                                                        'gave',
                                                        'general',
                                                        'generally',
                                                        'get',
                                                        'gets',
                                                        'give',
                                                        'given',
                                                        'gives',
                                                        'go',
                                                        'going',
                                                        'good',
                                                        'goods',
                                                        'got',
                                                        'great',
                                                        'greater',
                                                        'greatest',
                                                        'group',
                                                        'grouped',
                                                        'grouping',
                                                        'groups',
                                                        'h',
                                                        'had',
                                                        'has',
                                                        'have',
                                                        'having',
                                                        'he',
                                                        'her',
                                                        'here',
                                                        'herself',
                                                        'high',
                                                        'high',
                                                        'high',
                                                        'higher',
                                                        'highest',
                                                        'him',
                                                        'himself',
                                                        'his',
                                                        'how',
                                                        'however',
                                                        'i',
                                                        'if',
                                                        'important',
                                                        'in',
                                                        'interest',
                                                        'interested',
                                                        'interesting',
                                                        'interests',
                                                        'into',
                                                        'is',
                                                        'it',
                                                        'its',
                                                        'itself',
                                                        'j',
                                                        'just',
                                                        'k',
                                                        'keep',
                                                        'keeps',
                                                        'kind',
                                                        'knew',
                                                        'know',
                                                        'known',
                                                        'knows',
                                                        'l',
                                                        'large',
                                                        'largely',
                                                        'last',
                                                        'later',
                                                        'latest',
                                                        'least',
                                                        'less',
                                                        'let',
                                                        'lets',
                                                        'like',
                                                        'likely',
                                                        'long',
                                                        'longer',
                                                        'longest',
                                                        'm',
                                                        'made',
                                                        'make',
                                                        'making',
                                                        'man',
                                                        'many',
                                                        'may',
                                                        'me',
                                                        'member',
                                                        'members',
                                                        'men',
                                                        'might',
                                                        'more',
                                                        'most',
                                                        'mostly',
                                                        'mr',
                                                        'mrs',
                                                        'much',
                                                        'must',
                                                        'my',
                                                        'myself',
                                                        'n',
                                                        'necessary',
                                                        'need',
                                                        'needed',
                                                        'needing',
                                                        'needs',
                                                        'never',
                                                        'new',
                                                        'new',
                                                        'newer',
                                                        'newest',
                                                        'next',
                                                        'no',
                                                        'nobody',
                                                        'non',
                                                        'noone',
                                                        'not',
                                                        'nothing',
                                                        'now',
                                                        'nowhere',
                                                        'number',
                                                        'numbers',
                                                        'o',
                                                        'of',
                                                        'off',
                                                        'often',
                                                        'old',
                                                        'older',
                                                        'oldest',
                                                        'on',
                                                        'once',
                                                        'one',
                                                        'only',
                                                        'open',
                                                        'opened',
                                                        'opening',
                                                        'opens',
                                                        'or',
                                                        'order',
                                                        'ordered',
                                                        'ordering',
                                                        'orders',
                                                        'other',
                                                        'others',
                                                        'our',
                                                        'out',
                                                        'over',
                                                        'p',
                                                        'part',
                                                        'parted',
                                                        'parting',
                                                        'parts',
                                                        'per',
                                                        'perhaps',
                                                        'place',
                                                        'places',
                                                        'point',
                                                        'pointed',
                                                        'pointing',
                                                        'points',
                                                        'possible',
                                                        'present',
                                                        'presented',
                                                        'presenting',
                                                        'presents',
                                                        'problem',
                                                        'problems',
                                                        'put',
                                                        'puts',
                                                        'q',
                                                        'quite',
                                                        'r',
                                                        'rather',
                                                        'really',
                                                        'right',
                                                        'right',
                                                        'room',
                                                        'rooms',
                                                        's',
                                                        'said',
                                                        'same',
                                                        'saw',
                                                        'say',
                                                        'says',
                                                        'second',
                                                        'seconds',
                                                        'see',
                                                        'seem',
                                                        'seemed',
                                                        'seeming',
                                                        'seems',
                                                        'sees',
                                                        'several',
                                                        'shall',
                                                        'she',
                                                        'should',
                                                        'show',
                                                        'showed',
                                                        'showing',
                                                        'shows',
                                                        'side',
                                                        'sides',
                                                        'since',
                                                        'small',
                                                        'smaller',
                                                        'smallest',
                                                        'so',
                                                        'some',
                                                        'somebody',
                                                        'someone',
                                                        'something',
                                                        'somewhere',
                                                        'state',
                                                        'states',
                                                        'still',
                                                        'still',
                                                        'such',
                                                        'sure',
                                                        't',
                                                        'take',
                                                        'taken',
                                                        'than',
                                                        'that',
                                                        'the',
                                                        'their',
                                                        'them',
                                                        'then',
                                                        'there',
                                                        'therefore',
                                                        'these',
                                                        'they',
                                                        'thing',
                                                        'things',
                                                        'think',
                                                        'thinks',
                                                        'this',
                                                        'those',
                                                        'though',
                                                        'thought',
                                                        'thoughts',
                                                        'three',
                                                        'through',
                                                        'thus',
                                                        'to',
                                                        'today',
                                                        'together',
                                                        'too',
                                                        'took',
                                                        'toward',
                                                        'turn',
                                                        'turned',
                                                        'turning',
                                                        'turns',
                                                        'two',
                                                        'u',
                                                        'under',
                                                        'until',
                                                        'up',
                                                        'upon',
                                                        'us',
                                                        'use',
                                                        'used',
                                                        'uses',
                                                        'v',
                                                        'very',
                                                        'w',
                                                        'want',
                                                        'wanted',
                                                        'wanting',
                                                        'wants',
                                                        'was',
                                                        'way',
                                                        'ways',
                                                        'we',
                                                        'well',
                                                        'wells',
                                                        'went',
                                                        'were',
                                                        'what',
                                                        'when',
                                                        'where',
                                                        'whether',
                                                        'which',
                                                        'while',
                                                        'who',
                                                        'whole',
                                                        'whose',
                                                        'why',
                                                        'will',
                                                        'with',
                                                        'within',
                                                        'without',
                                                        'work',
                                                        'worked',
                                                        'working',
                                                        'works',
                                                        'would',
                                                        'x',
                                                        'y',
                                                        'year',
                                                        'years',
                                                        'yet',
                                                        'you',
                                                        'young',
                                                        'younger',
                                                        'youngest',
                                                        'your',
                                                        'yours',
                                                        'z',
                                                        'br',
                                                      #  ]]

            # stop words from https://www.link-assistant.com/seo-stop-words.html
             #no_stop = [w for w in no_punct if w not in [
                                                        '\x97','a',	'hence',	'see','able',	'her',	'seeing',
                                                        'about',	'here',	'seem','above',	'hereafter',	'seemed',
                                                        'abroad',	'hereby',	'seeming','according',	'herein',	'seems',
                                                        'accordingly',	'heres',	'seen','across',	'hereupon',	'self',
                                                        'actually',	'hers',	'selves','adj',	'herself',	'sensible',
                                                        'after',	'hes',	'sent','afterwards',	'hi',	'serious',
                                                        'again',	'him',	'seriously','against',	'himself',	'seven',
                                                        'ago',	'his',	'several','ahead',	'hither',	'shall',
                                                        'aint',	'hopefully',	'shant','all',	'how',	'she',
                                                        'allow',	'howbeit',	'shed','allows',	'however',	'shell',
                                                        'almost',	'hundred',	'shes','alone',	'i',	'should',
                                                        'along',	'id',	'shouldnt','alongside',	'ie',	'since',
                                                        'already',	'if',	'six','also',	'ignored',	'so',
                                                        'although',	'ill',	'some','always',	'im',	'somebody',
                                                        'am',	'immediate',	'someday','amid',	'in',	'somehow',
                                                        'amidst',	'inasmuch',	'someone','among',	'inc',	'something',
                                                        'amongst',	'inc.',	'sometime','an',	'indeed',	'sometimes',
                                                        'and',	'indicate',	'somewhat','another',	'indicated',	'somewhere',
                                                        'any',	'indicates',	'soon','anybody',	'inner',	'sorry',
                                                        'anyhow',	'inside',	'specified','anyone',	'insofar',	'specify',
                                                        'anything',	'instead',	'specifying','anyway',	'into',	'still',
                                                        'anyways',	'inward',	'sub','anywhere',	'is',	'such',
                                                        'apart',	'isnt',	'sup','appear',	'it',	'sure',
                                                        'appreciate',	'itd',	't','appropriate',	'itll',	'take',
                                                        'are',	'its',	'taken','arent',	'its',	'taking',
                                                        'around',	'itself',	'tell','as',	'ive',	'tends',
                                                        'as',	'j',	'th','aside',	'just',	'than',
                                                        'ask',	'k',	'thank','asking',	'keep',	'thanks',
                                                        'associated',	'keeps','thanx','at',	'kept',	'that',
                                                        'available',	'know',	'thatll','away',	'known',	'thats',
                                                        'awfully',	'knows',	'thats','b',	'l',	'thatve',
                                                        'back',	'last',	'the','backward',	'lately',	'their',
                                                        'backwards',	'later',	'theirs','be',	'latter',	'them',
                                                        'became',	'latterly',	'themselves','because',	'least',	'then',
                                                        'become',	'less',	'thence','becomes',	'lest',	'there',
                                                        'becoming',	'let',	'thereafter','been',	'lets',	'thereby',
                                                        'before',	'like',	'thered','beforehand',	'liked',	'therefore',
                                                        'begin',	'likely',	'therein','behind',	'likewise',	'therell',
                                                        'being',	'little',	'therere','believe',	'look',	'theres',
                                                        'below',	'looking',	'theres','beside',	'looks',	'thereupon',
                                                        'besides',	'low',	'thereve','best',	'lower',	'these',
                                                        'better',	'ltd',	'they','between',	'm',	'theyd',
                                                        'beyond',	'made',	'theyll','both',	'mainly',	'theyre',
                                                        'brief',	'make',	'theyve','but',	'makes',	'thing',
                                                        'by',	'many',	'things','c',	'may',	'think',
                                                        'came',	'maybe',	'third','can',	'maynt',	'thirty',
                                                        'cannot',	'me',	'this','cant',	'mean',	'thorough',
                                                        'cant',	'meantime',	'thoroughly','caption',	'meanwhile',	'those',
                                                        'cause',	'merely',	'though','causes',	'might',	'three',
                                                        'certain',	'mightnt',	'through','certainly',	'mine',	'throughout',
                                                        'changes',	'minus',	'thru','clearly',	'miss',	'thus',
                                                        'cmon',	'more',	'till','co',	'moreover',	'to',
                                                        'co.',	'most',	'together','com',	'mostly',	'too',
                                                        'come',	'mr',	'took','comes',	'mrs',	'toward',
                                                        'concerning',	'much',	'towards','consequently',	'must',	'tried',
                                                        'consider',	'mustnt',	'tries','considering',	'my',	'truly',
                                                        'contain',	'myself',	'try','containing',	'n',	'trying',
                                                        'contains',	'name',	'ts','corresponding',	'namely',	'twice',
                                                        'could',	'nd',	'two','couldnt',	'near',	'u',
                                                        'course',	'nearly',	'un','cs',	'necessary',	'under',
                                                        'currently',	'need',	'underneath','d',	'neednt',	'undoing',
                                                        'dare',	'needs',	'unfortunately', 'darent',	'neither',	'unless',
                                                        'definitely',	'never',	'unlike', 'described',	'neverf',	'unlikely',
                                                        'despite',	'neverless',	'until', 'did',	'nevertheless',	'unto',
                                                        'didnt',	'new',	'up',     'different',	'next',	'upon',
                                                        'directly',	'nine',	'upwards',      'do',	'ninety',	'us',
                                                        'does',	'no',	'use', 'doesnt',	'nobody',	'used', 
                                                        'doing',	'non',	'useful',   'done',	'none',	'uses',
                                                        'dont',	'nonetheless',	'using',  'down',	'noone',	'usually',
                                                        'downwards',	'no-one',	'v',    'during',	'nor',	'value',
                                                        'e',	'normally',	'various',        'each',	'not',	'versus',
                                                        'edu',	'nothing',	'very',        'eg',	'notwithstanding',	'via',
                                                        'eight',	'novel',	'viz',      'eighty',	'now',	'vs',
                                                        'either',	'nowhere',	'w',     'else',	'o',	'want',
                                                        'elsewhere',	'obviously',	'wants',     'end',	'of',	'was',
                                                        'ending',	'off',	'wasnt',     'enough',	'often',	'way',
                                                        'entirely',	'oh',	'we',    'especially',	'ok',	'wed',
                                                        'et',	'okay',	'welcome',   'etc',	'old',	'well',
                                                        'even',	'on',	'well',     'ever',	'once',	'went',
                                                        'evermore',	'one',	'were',            'every',	'ones',	'were',
                                                        'everybody',	'ones',	'werent',     'everyone',	'only',	'weve',
                                                        'everything',	'onto',	'what',    'everywhere',	'opposite',	'whatever',
                                                        'ex',	'or',	'whatll',      'exactly',	'other',	'whats',
                                                        'example',	'others',	'whatve',        'except',	'otherwise',	'when',
                                                        'f',	'ought',	'whence',           'fairly',	'oughtnt',	'whenever',
                                                        'far',	'our',	'where',      'farther',	'ours',	'whereafter',
                                                        'few',	'ourselves',	'whereas',          'fewer',	'out',	'whereby',
                                                        'fifth',	'outside',	'wherein',                   'first',	'over',	'wheres',
                                                        'five',	'overall',	'whereupon',         'followed',	'own',	'wherever',
                                                        'following',	'p',	'whether',   'follows',	'particular',	'which',
                                                        'for',	'particularly',	'whichever',     'forever',	'past',	'while',
                                                        'former',	'per',	'whilst',     'formerly',	'perhaps',	'whither',
                                                        'forth',	'placed',	'who',     'forward',	'please',	'whod',
                                                        'found',	'plus',	'whoever',      'four',	'possible',	'whole',
                                                        'from',	'presumably',	'wholl',     'further',	'probably',	'whom',
                                                        'furthermore',	'provided',	'whomever',      'g',	'provides',	'whos',
                                                        'get',	'q',	'whose',     'gets',	'que',	'why',
                                                        'getting',	'quite',	'will',     'given',	'qv',	'willing',
                                                        'gives',	'r',	'wish',        'go',	'rather',	'with',
                                                        'goes',	'rd',	'within',    'going',	're',	'without',
                                                        'gone',	'really',	'wonder',  'got',	'reasonably',	'wont',
                                                        'gotten',	'recent',	'would',      'greetings',	'recently',	'wouldnt',
                                                        'h',	'regarding',	'x',       'had',	'regardless',	'y',
                                                        'hadnt',	'regards',	'yes',     'half',	'relatively',	'yet',
                                                        'happens',	'respectively',	'you',     'hardly',	'right',	'youd',
                                                        'has',	'round',	'youll',     'hasnt',	's',	'your',
                                                        'have',	'said',	'youre',      'havent',	'same',	'yours',
                                                        'having',	'saw',	'yourself',        'he',	'say',	'yourselves',
                                                        'hed',	'saying',	'youve',       'hell',	'says',	'z',
                                                        'hello',	'second',	'zero',       'help',	'secondly',	'br'])



            no_stop = [w for w in no_punct if w not in stop_words]

            for j in range(40):
                try:
                    data[i][j] = glove_dict[no_stop[j]]
                except (KeyError, IndexError):
                    #print(no_stop[j])
                    data[i][j] = 0

    return data
    
   


def load_glove_embeddings():
    """
    Load the glove embeddings into a array and a dictionary with words as
    keys and their associated index as the value. Assumes the glove
    embeddings are located in the same directory and named "glove.6B.50d.txt"
    RETURN: embeddings: the array containing word vectors
            word_index_dict: a dictionary matching a word in string form to
            its index in the embeddings array. e.g. {"apple": 119"}
    """
    
    data = open("glove.6B.50d.txt",'r',encoding="utf-8").read().split()
     
    embeddings= [np.zeros(50)]
    word_index_dict = {'UNK':0}
    
    word_counter = 1
    vector_place = 50
    
    for d in data:
        if vector_place == 50:
            embeddings.append(np.zeros(50))
            word_index_dict.update({d : word_counter})
            word_counter += 1
            vector_place = 0
        else:
            embeddings[-1][vector_place] = float(d)
            vector_place +=1

    #if you are running on the CSE machines, you can load the glove data from here
    #data = open("/home/cs9444/public_html/17s2/hw2/glove.6B.50d.txt",'r',encoding="utf-8")
    return embeddings, word_index_dict


def define_graph(glove_embeddings_arr):
    """
    Define the tensorflow graph that forms your model. You must use at least
    one recurrent unit. The input placeholder should be of size [batch_size,
    40] as we are restricting each review to it's first 40 words. The
    following naming convention must be used:
        Input placeholder: name="input_data"
        labels placeholder: name="labels"
        accuracy tensor: name="accuracy"
        loss tensor: name="loss"

    RETURN: input placeholder, labels placeholder, optimizer, accuracy and loss
    tensors"""

    dropout_keep_prob = tf.placeholder_with_default(1.0, shape=())

    def lstm_cell_with_dropout():
        cell = tf.contrib.rnn.BasicLSTMCell(hidden_units)
        return tf.contrib.rnn.DropoutWrapper(cell, output_keep_prob=dropout_keep_prob)

    embedding_shape = 50
    hidden_units = 20
    fully_connected_units = 20
    num_layers = 3
    vocab_size = len(glove_embeddings_arr)

    input_data = tf.placeholder(tf.int32,[batch_size,40], name="input_data")
    labels = tf.placeholder(tf.int32,[batch_size,2], name="labels")

    embeddings = tf.get_variable("embeddings", shape=[400001,embedding_shape], initializer=tf.constant_initializer(np.array(glove_embeddings_arr)), trainable=False)
    inputs = tf.nn.embedding_lookup(embeddings, input_data)

    #cell = tf.contrib.rnn.GRUCell(embedding_shape)
    #cell = tf.contrib.rnn.LayerNormBasicLSTMCell(embedding_shape,dropout_keep_prob=0.5)

    cell = tf.nn.rnn_cell.MultiRNNCell([lstm_cell_with_dropout() for _ in range(num_layers)]
                                        ,state_is_tuple=True)

    initial_state = cell.zero_state(batch_size, tf.float32)

    outputs, state = tf.nn.dynamic_rnn(cell, inputs,initial_state=initial_state,dtype=tf.float32)

    outputs = tf.transpose(outputs, [1, 0, 2])
    last = tf.gather(outputs, int(outputs.get_shape()[0]) - 1)

    #fully_connected = tf.contrib.layers.fully_connected(last, fully_connected_units, activation_fn=tf.sigmoid)
    fully_connected = tf.contrib.layers.fully_connected(last, fully_connected_units)
    fully_connected = tf.contrib.layers.dropout(fully_connected, dropout_keep_prob)

    logits = tf.contrib.layers.fully_connected(fully_connected, 2, activation_fn=None)

    loss = tf.losses.softmax_cross_entropy(labels,logits)

    optimizer = tf.train.RMSPropOptimizer(0.001).minimize(loss)

    preds = tf.nn.softmax(logits)
    correct_preds = tf.equal(tf.argmax(preds, 1, output_type=tf.int32), tf.argmax(labels, 1, output_type=tf.int32))
    accuracy = tf.reduce_mean(tf.cast(correct_preds, tf.float32))

    return input_data, labels, dropout_keep_prob, optimizer, accuracy, loss
