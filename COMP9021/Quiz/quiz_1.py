# Generates a list L_indexes of 12 random numbers between 0 and 3 included,
# generates a list L_values of 15 distinct random numbers between 0 and 99 included,
# and iteratively builds a list resulting_list as follows:
# - if i_1, ..., i_k is the longest initial segment of L_indexes consisting of nothing but
#   distinct numbers, then add to resulting_list the elements of L_values of index i_1, ..., i_k;
# - remove from L_indexes and L_values the numbers that have been used during the previous step.
#
# Written by *** and Eric Martin for COMP9021

import sys
from random import seed, randint, sample


nb_of_indexes = 12
max_index = 3
upper_bound = 100

try:
     seed(input('Enter an integer: '))
except TypeError:
    print('Incorrect input, giving up.')
    sys.exit()

L_indexes = [randint(0, max_index) for _ in range(nb_of_indexes)]
L_values = sample(range(upper_bound), nb_of_indexes + max_index)
print('The generated lists of indexes and values are, respectively:')
print('  ', L_indexes)
print('  ', L_values)

resulting_list = []

def longest_segment(input_list,start_index,segment_so_far):
     try:
          if input_list[start_index] not in segment_so_far:
               segment_so_far.append(input_list[start_index])
               longest_segment(input_list,start_index + 1,segment_so_far)
          return segment_so_far
     except:
          return input_list

while L_indexes:
     for i in longest_segment(L_indexes,0,[]):
          resulting_list.append(L_values[i])
          L_indexes.remove(i)
     L_values=[i for i in L_values if i not in resulting_list]

print('The resulting list of values is:')
print('  ', resulting_list)
