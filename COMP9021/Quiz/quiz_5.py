# Prompts the user for a seed, a dimension dim, and an upper bound N.
# Randomly fills a grid of size dim x dim with numbers between 0 and N and computes:
# - the largest value n such that there is a path of the form (0, 1, 2,... n), and
# - the number of such paths.
# A path is obtained by repeatedly moving in the grid one step north, south, west, or east.

# Written by *** and Eric Martin for COMP9021


import sys
from random import seed, randint


def display_grid():
    width = len(str(upper_bound))
    for i in range(dim):
        print('    ', end = '')
        for j in range(dim):
            print('{0:{2}s}{1:{2}d}'.format(' ', grid[i][j], width), end = '')
        print()
    print()


def update(max_value, nb_of_paths_of_max_value, value, nb_of_paths):
    if value > max_value:
        max_value = value
    if nb_of_paths > nb_of_paths_of_max_value:
        nb_of_paths_of_max_value = nb_of_paths
    return max_value, nb_of_paths_of_max_value
    
  
def value_and_number_of_longest_paths(i = None, j = None):
    if i == None:
        i,j = 0,0
        value=0
        nb_of_paths=0
    else:
        max_value,nb_of_paths_of_max_value = update(max_value, nb_of_paths_of_max_value, value, nb_of_paths)

    if grid[i+1][j] = grid[i][j]+1:
        value_and_number_of_longest_paths(i+1,j)
    if grid[i][j+1] = grid[i][j]+1:
        value_and_number_of_longest_paths(i,j+1)
    if i != 0:
        if grid[i-1][j] = grid[i][j]+1:
            value_and_number_of_longest_paths(i-1,j)
    if j != 0:
        if grid[i][j-1] = grid[i][j]+1:
            value_and_number_of_longest_paths(i,j-1)
    if 
        


    if i < dim:
        value_and_number_of_longest_paths(i+1,j)
    elif j < dim:
        value_and_number_of_longest_paths(0,j+1)
    else:
        return max_value,nb_of_paths_of_max_value

    # REPLACE "return None, None" above AND THIS COMMENT WITH YOUR CODE (aim for around 20 lines of code)
        

provided_input = input('Enter three nonnegative integers: ')
provided_input = provided_input.split()
if len(provided_input) != 3:
    print('Incorrect input, giving up.')
    sys.exit()
try:
    provided_seed, dim, upper_bound = tuple(int(provided_input[i]) for i in range(3))
except ValueError:
    print('Incorrect input, giving up.')
    sys.exit()

seed(provided_seed)
grid = [[randint(0, upper_bound) for _ in range(dim)] for _ in range(dim)]
print('Here is the grid that has been generated:')
display_grid()

max_value, nb_of_paths_of_max_value = value_and_number_of_longest_paths()
if nb_of_paths_of_max_value == 0:
    print('There is no 0 in the grid.')
else:
    print('The longest paths made up of consecutive numbers starting from 0 go up to {}.'.format(max_value))
    if nb_of_paths_of_max_value == 1:
        print('There is one such path.')
    else:
        print('There are', nb_of_paths_of_max_value, 'such paths.')
