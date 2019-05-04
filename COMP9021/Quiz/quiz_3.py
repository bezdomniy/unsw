# Prompts the user for an arity N (a nonnegative integer) and a term
# where the function symbols are assumed to consist of nothing but alphabetic
# characters and underscores, with spaces allowed at the beginning
# and around parentheses and commas.
# Checks that the term is syntactically correct according to the definition of a term
# and also in that all function symbols are of arity N or 0.
#
# Written by *** and Eric Martin for COMP9021


import sys
import string

def is_syntactically_correct(term, arity):
    correct_values = [x for x in string.ascii_letters]
    correct_values.append('_')

    rules = {'L1':{*correct_values,'('},
             '(':{'L1','L2'},
             'L2':{*correct_values,',',')'},
             ',':{'L1','L2'},
             ')':{',',')','ε'}}

    correct=True
    
    bracket_values = {'(': 1,')': -1,',': 0}
    bracket_count=0
    
    term += 'ε'

    term = term.replace(' ', '')
    term_list = []
    word = ''
    for c in term:
        if c in ',()':
            if word:
                term_list.append(word)
                word = ''
            term_list.append(c)
        elif c in correct_values or c == "ε":
            word += c
        else:
            correct = False
    if word:
        term_list.append(word)
    
    for word in range(len(term_list)-1):
        if set(term_list[word]).issubset(set(correct_values)) or (term_list[word] in ['(',')',',']):
            if term_list[word] in ['(',')',',']:
                bracket_count+=bracket_values[term_list[word]]
            else:
                if term_list[word+1] in rules['L1']:
                    term_list[word]='L1'
                else:
                    term_list[word]='L2'
    

    args=[]
    for word in range(len(term_list)):
        if term_list[word] == 'L1':
            count_args=0
            start_count=0
            for i in range(word,len(term_list)):
                if term_list[i] in ['(',')',',']:
                    start_count+=bracket_values[term_list[i]]
                    if start_count==0:
                        break
                if start_count == 1:
                    if term_list[i] in ['L1','L2']:
                        count_args+=1
            args.append(count_args)

    for _ in args:
        if _ != arity:
            correct=False
    if args==[] and arity !=0:
        correct=False
     
    if bracket_count != 0:
        correct=False
        
    for i in range(len(term_list)-1):
        if term_list[i+1] in rules[term_list[i]]:
            continue
        else:
            correct=False
            break

    return correct
   
try:
    arity = int(input('Input an arity : '))
    if arity < 0:
        raise ValueError
except ValueError:
    print('Incorrect arity, giving up...')
    sys.exit()
print('A term should contain only letters, underscores, commas, parentheses, spaces.')
term = input('Input a term: ')
if is_syntactically_correct(term, arity):
    print('Good, the term is syntactically correct.')
else:
    print('Unfortunately, the term is syntactically incorrect.')
