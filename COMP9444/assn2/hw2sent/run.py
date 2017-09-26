"""This module runs tests"""

from implementation import load_glove_embeddings, load_data

embeddings, word_index_dict = load_glove_embeddings()

data= load_data(word_index_dict)