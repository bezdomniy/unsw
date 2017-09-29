"""This module runs tests"""

from implementation import load_glove_embeddings, load_data, define_graph 
import os
import numpy as np
import tensorflow as tf
os.environ['TF_CPP_MIN_LOG_LEVEL']='2'

embeddings, word_index_dict = load_glove_embeddings()

data= load_data(word_index_dict)

'''
def getValBatch():
    labels = []
    arr = np.zeros([5000, 40])
    for i in range(5000):
        if i <2500:
            arr[i] = data[10000+i]
            labels.append([1, 0])
        else:
            arr[i] = data[20000+i]
            labels.append([0, 1])
    return arr, labels

val_data, val_labels = getValBatch()

saver = tf.train.import_meta_graph('./checkpoints/trained_model.ckpt-90000.meta')

input_data, labels, optimizer, accuracy, loss = define_graph(embeddings)

with tf.Session() as sess:
    sess.run(tf.global_variables_initializer())
    saver.restore(sess, tf.train.latest_checkpoint('./checkpoints/'))
    print("Model restored.")
    #

    accuracies = []

    for i in range(0,5000,50):  
        accuracy_value = sess.run([accuracy],{input_data: val_data[i:i+50], labels: val_labels[i:i+50]})
        accuracies.append(accuracy_value)
    print("Test Accuracy = {:.3f}".format(np.mean(accuracies)))
    
'''