# Implements a function to encode a single strictly positive natural number that in base 2,
# reads as b_1 ... b_n, as b_1b_1 ... b_nb_n, and to encode a sequence of strictly positive
# natural numbers N_1 ... N_k with k >= 2 as N_1* 0 ... 0 N_k* where for all 0 < i <= k,
# N_i* is the encoding of N_i.
# Implements a function to decode a natural number N into a sequence of (one or more)
# strictly positive natural numbers according to the previous encoding scheme,
# or return None in case N does not encode such a sequence.
# Prompts the user to enter a strictly positive natural number, applies the decoding function,
# and prints out base 2 representations of both the encoding number and the decoded sequence
# for verification purposes.
#
# Written by *** and Eric Martin for COMP9021

import sys


def encode(*integers):
    return int('0'.join(''.join(c + c for c in bin(i)[2: ]) for i in integers), 2)

def decode(integer):
    bin_int = bin(integer)[2:]
    bin_output = [[]]
    count=0
    i=1
    
    while i < len(bin_int):
        if bin_int[i] == bin_int[i-1]:
            bin_output[count].append(bin_int[i])
            i+=2
        else:
            bin_output.append([])
            count+=1
            i+=1
    if ['0'] in bin_output or [] in bin_output or i == len(bin_int):
        return None
    
    output = [int("".join(i),2) for i in bin_output]
    return output

integer = input('Input a strictly positive integer: ')

if integer[0] == '0' or not integer.isdigit():
    print('Incorrect input, giving up!')
    sys.exit()
integer = int(integer)
decoding = decode(integer)
if decoding is None:
    print('Incorrect encoding!')
else:
    print('This encodes: ', decoding)
    print('Checking: ')
    print('  In base 2, {} is {}'.format(integer, bin(integer)[2: ]))
    print('  In base 2, {} is: [{}]'.format(decoding, ', '.join(bin(i)[2: ] for i in decoding)))
                    
                    
