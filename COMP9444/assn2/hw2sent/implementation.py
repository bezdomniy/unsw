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

    hidden_size = 40
    num_steps = 20
    vocab_size = len(glove_embeddings_arr)

    input_data = tf.placeholder(tf.int32,[batch_size,40])
    labels = tf.placeholder(tf.int32,[batch_size,2])

    embeddings = tf.Variable(tf.constant(0.0, shape=[vocab_size, 50]),
                    trainable=False, name="embeddings")

    embedding_placeholder = tf.placeholder(tf.float32, [vocab_size, 50])
    embedding_init = embeddings.assign(embedding_placeholder)

    sess = tf.Session()
    sess.run(embedding_init, feed_dict={embedding_placeholder: glove_embeddings_arr})

    inputs = tf.nn.embedding_lookup(embeddings, input_data)

    print(inputs)

    cell = tf.contrib.rnn.BasicLSTMCell(hidden_size, state_is_tuple=True)
    initial_state = cell.zero_state(batch_size, tf.float32)

    #unstacked_input = tf.unstack(inputs, num=num_steps, axis=1)

    #outputs, state = tf.contrib.rnn.static_rnn(cell, unstacked_input,initial_state=initial_state,dtype=tf.float32)

    outputs, state = tf.nn.dynamic_rnn(cell, inputs,dtype=tf.float32)
    outputs = tf.reduce_mean(outputs, reduction_indices=[1])
    print(outputs)
    #outputs, _ = tf.nn.dynamic_rnn(cell, inputs,dtype=tf.float32)

    #softmax_w = tf.get_variable("softmax_w", [hidden_size, 2], dtype=tf.float32)
    #softmax_b = tf.get_variable("softmax_b", [2], dtype=tf.float32)

    softmax_w = tf.Variable(tf.random_normal([hidden_size, 2], stddev=0.01, name='w'), dtype=tf.float32)
    softmax_b = tf.Variable(tf.random_normal(shape=[2], stddev=0.01, name='b'), dtype=tf.float32)
    
    # just using last output

    logits = tf.nn.xw_plus_b(outputs, softmax_w, softmax_b, name='logits')
    preds = tf.nn.softmax(logits)
    loss = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=logits,labels=labels))
    #loss= tf.reduce_mean(loss)

    #preds = tf.contrib.layers.fully_connected(outputs[:, -1], 1, activation_fn=tf.sigmoid)
    #print(labels)
    #print(preds)
    #loss = tf.losses.mean_squared_error(labels, preds)
    
    #logits_series = [tf.nn.xw_plus_b(output, softmax_w, softmax_b) for output in outputs]
    #preds_series = [tf.nn.softmax(logits) for logits in logits_series]
    #preds = tf.reduce_mean(preds_series)

    #labels_series = [labels * hidden_size]

    #losses = [tf.nn.softmax_cross_entropy_with_logits(logits=logits,labels=labels) for logits, labels in zip(logits_series,labels_series)]
    #loss = tf.reduce_mean(losses)
    
    #print(logits)
    

     # Reshape logits to be a 3-D tensor for sequence loss
    #logits = tf.reshape(logits, [batch_size, num_steps, vocab_size])



    optimizer = tf.train.AdamOptimizer(0.1).minimize(loss)

    correct_preds = tf.equal(tf.argmax(preds, 1, output_type=tf.int32), tf.argmax(labels, 1, output_type=tf.int32))
    accuracy = tf.reduce_mean(tf.cast(correct_preds, tf.float32))

    #accuracy = tf.constant(0)

    return input_data, labels, optimizer, accuracy, loss
