"""This module runs tests"""

from implementation import load_glove_embeddings, load_data, define_graph 
import os
import numpy as np
import tensorflow as tf
os.environ['TF_CPP_MIN_LOG_LEVEL']='2'

embeddings, word_index_dict = load_glove_embeddings()

data= load_data(word_index_dict,test=True)


def getValBatch():
    labels = []
    arr = np.zeros([25000, 40])
    for i in range(25000):
        if i <12500:
            arr[i] = data[i]
            labels.append([1, 0])
        else:
            arr[i] = data[i]
            labels.append([0, 1])
    return arr, labels

val_data, val_labels = getValBatch()

saver = tf.train.import_meta_graph('./checkpoints/trained_model.ckpt-50000.meta')

#input_data, labels, dropout_keep_prob, optimizer, accuracy, loss = define_graph(embeddings)

with tf.Session() as sess:
    sess.run(tf.global_variables_initializer())

    #saver.restore(sess, tf.train.latest_checkpoint('./checkpoints/'))
    saver.restore(sess, './checkpoints/trained_model.ckpt-50000')

    graph = tf.get_default_graph()

    input_data = graph.get_tensor_by_name("input_data:0")
    labels = graph.get_tensor_by_name("labels:0")

    accuracy = graph.get_tensor_by_name("accuracy:0")
    print("Model restored.")
    #

    accuracies = []

    for i in range(0,25000,50):  
        accuracy_value = sess.run([accuracy],{input_data: val_data[i:i+50], labels: val_labels[i:i+50]})
        print("Acc: ",accuracy_value)
        accuracies.append(accuracy_value)
    print("Test Accuracy = {:.3f}".format(np.mean(accuracies)))
    
