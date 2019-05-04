# Randomly fills a grid of size 7 x 7 with NE, SE, SW, NW,
# meant to represent North-East, South-East, North-West, South-West,
# respectively, and starting from the cell in the middle of the grid,
# determines, for each of the 4 corners of the grid, the preferred path amongst
# the shortest paths that reach that corner, if any. At a given cell, it is possible to move
# according to any of the 3 directions indicated by the value of the cell;
# e.g., from a cell storing NE, it is possible to move North-East, East, or North.
# At any given point, one prefers to move diagonally, then horizontally,
# and vertically as a last resort.
#
# Written by *** and Eric Martin for COMP9021


import sys
from random import seed, choice
from array_queue import *

def display_grid():
    for i in range(dim):
        print('    ', end = '')
        for j in range(dim):
            print(' ', grid[i][j], end = '')
        print()
    print()

def preferred_paths_to_corners():
    best_path=ArrayQueue()
    best_path.enqueue([(3,3)])
    count_iterations=0
  
    direction_values={"NW":(-1,-1),"NE":(-1,1),"SW":(1,-1),"SE":(1,1),"E":(0,1),"W":(0,-1),"N":(-1,0),"S":(1,0)}
    c=set(corners)
    paths={}

    while c != [] and count_iterations<300000:
        count_iterations+=1
        position=best_path.dequeue()
        
        if (position[-1][-1],position[-1][0]) in c:
            reverse_coordinates=[(x[-1],x[0]) for x in position]
            c.remove(reverse_coordinates[-1])
            paths[reverse_coordinates[-1]]=reverse_coordinates
            
        available_paths={x:direction_values[x] for x in direction_values if x in (grid[position[-1][0]][position[-1][1]])}
        for i in available_paths:
            box1 = position[-1][0]+available_paths[i][0]
            box2 = position[-1][1]+available_paths[i][1]
            if box1 >=0 and box2 >=0 and box1 < 7 and box2 < 7:
                next_position=position +[(box1,box2)]
                best_path.enqueue(next_position)
        else:
            continue
        best_path.enqueue(position)

    return paths
        

try:
    seed_arg = int(input('Enter an integer: '))
except ValueError:
    print('Incorrect input, giving up.')
    sys.exit()
    
seed(seed_arg)
size = 3
dim = 2 * size + 1
grid = [[0] * dim for _ in range(dim)]
directions = 'NE', 'SE', 'SW', 'NW'

for i in range(dim):
    for j in range(dim):
        grid[i][j] = choice(directions)
print('Here is the grid that has been generated:')
display_grid()

corners = (0, 0), (dim - 1, 0), (dim - 1, dim - 1), (0, dim - 1)
paths = preferred_paths_to_corners()
if not paths:
    print('There is no path to any corner')
    sys.exit()
for corner in corners:
    if corner not in paths:
        print('There is no path to {}'.format(corner))
    else:
        print('The preferred path to {} is:'.format(corner))
        print('  ', paths[corner])
