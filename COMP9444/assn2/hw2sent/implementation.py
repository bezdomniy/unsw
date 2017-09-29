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
            no_punct = ''.join(c.lower() for c in s if c not in (string.punctuation or '\x97'))
            no_punct = no_punct.split()

            if i < 5:
            #    print(s[:40])
                print(no_punct[:5])

            for j in range(40):
                try:
                    data[i][j] = glove_dict[no_punct[j]]
                except (KeyError, IndexError):
                    data[i][j] = 0
                
            #if i < 5:
                #print(no_punct[i][:5])
                #print(data[i][:5])

    
    for i in range(5):
        print(data[i][:5])
    
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

    embedding_shape = 50
    num_steps = 40
    vocab_size = len(glove_embeddings_arr)

    input_data = tf.placeholder(tf.int32,[batch_size,40], name="input_data")
    labels = tf.placeholder(tf.int32,[batch_size,2], name="labels")

    embeddings = tf.get_variable("embeddings", shape=[400001,50], initializer=tf.constant_initializer(np.array(glove_embeddings_arr)), trainable=False)

    inputs = tf.nn.embedding_lookup(embeddings, input_data)

    #word_list = tf.unstack(inputs, axis=1)

    cell = tf.contrib.rnn.BasicLSTMCell(embedding_shape, state_is_tuple=True)
    initial_state = cell.zero_state(batch_size, tf.float32)


    #outputs, state = tf.contrib.rnn.static_rnn(cell, word_list,initial_state=initial_state,dtype=tf.float32)
    outputs, state = tf.nn.dynamic_rnn(cell, inputs,initial_state=initial_state,dtype=tf.float32)
    #outputs = tf.reduce_mean(outputs, reduction_indices=[1])

    outputs = tf.transpose(outputs, [1, 0, 2])
    last = tf.gather(outputs, int(outputs.get_shape()[0]) - 1)

    logits = tf.contrib.layers.fully_connected(last, 2, activation_fn=None)

    loss = tf.losses.softmax_cross_entropy(labels,logits)

    optimizer = tf.train.RMSPropOptimizer(0.01).minimize(loss)

    preds = tf.nn.softmax(logits)
    correct_preds = tf.equal(tf.argmax(preds, 1, output_type=tf.int32), tf.argmax(labels, 1, output_type=tf.int32))
    accuracy = tf.reduce_mean(tf.cast(correct_preds, tf.float32))

    return input_data, labels, optimizer, accuracy, loss
