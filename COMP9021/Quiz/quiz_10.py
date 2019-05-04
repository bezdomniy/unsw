# Randomly generates N distinct integers with N provided by the user,
# inserts all these elements into a priority queue, and outputs a list
# L consisting of all those N integers, determined in such a way that:
# - inserting the members of L from those of smallest index of those of
#   largest index results in the same priority queue;
# - L is preferred in the sense that the last element inserted is as large as
#   possible, and then the penultimate element inserted is as large as possible, etc.
#
# Written by *** and Eric Martin for COMP9021

import sys
from random import seed, sample
from priority_queue import *
from copy import deepcopy

# Possibly define some functions

def shift(elements,p,i):
##    print('start1  ',elements)
    swap=elements.index(i)
    temp=elements[p]
    elements[p]=elements[swap]
    elements.remove(i)
    elements.insert(p//2,temp)
##    print('start   ',elements)

    test_pq=PriorityQueue()
    for j in [x for x in elements if x is not None]:
        if j is not None:
            test_pq.insert(j)
    if test_pq._data == pq._data:
##        print("here")
        return elements
        
    while p>=1:
        
        p=elements.index(temp)-1
        temp=elements[p]
        elements.pop(p)
        elements.insert((p+1)//2,temp)
##        print('checking',elements)
    
    return elements
    
    
def preferred_sequence():
    new_pq = PriorityQueue()

    elements=sorted([x for x in pq._data if x is not None],reverse=True)
    data=[x for x in pq._data if x is not None]
    output=[]

    for p in range(len(elements),1,-1):
        for i in elements:
            
            new_data=[None]+[x for x in data if x is not None]
            swap=new_data.index(i)

##            print('before',new_data)

            new_data=shift(new_data,p,i)
##            print('after ',new_data)

    
            for j in new_data:
                if j is not None:
                    new_pq.insert(j)

            if new_pq._data == pq._data:
                data=new_data[:]
##                print('yes')
                output.append(i)
                new_pq=PriorityQueue()
                break
            else:
                new_pq=PriorityQueue()
                
        try:
            elements.remove(i)

        except IndexError:
            break

    return new_data[1:]

provided_input = input('Enter 2 positive integers: ')
try:
    provided_input = [int(arg) for arg in provided_input.split()]
except ValueError:
    print('Incorrect input (not all integers), giving up.')
    sys.exit()
    
if len(provided_input) != 2:
    print('Incorrect input (not 2 arguments), giving up.')
    sys.exit()
for_seed, length = provided_input
if length < 0 or length > 100:
    print('Incorrect input (2nd integer not positive, giving up.')
    sys.exit()
seed(for_seed)

L = sample(list(range(length * 10)), length)
pq = PriorityQueue()
for e in L:
    pq.insert(e)
print('The heap that has been generated is: ')
print(pq._data[ : len(pq) + 1])
print('The preferred ordering of data to generate this heap by successsive insertion is:')
print(preferred_sequence())

##test=PriorityQueue()
##test.insert(51)
##test.insert(53)
##test.insert(62)
##test.insert(33)
##test.insert(49)
##test.insert(5)
##test.insert(65)
##print(test._data)
