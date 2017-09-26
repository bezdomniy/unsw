import numpy as np
import glob #this will be useful when reading reviews from file
import os
import tarfile
import string
import tensorflow as tf

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

    return input_data, labels, optimizer, accuracy, loss
