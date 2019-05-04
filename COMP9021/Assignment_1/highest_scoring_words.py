import sys

## Prompts the user to input a number and checks if it is valid.

letters = ''.join((input("Enter between 3 and 10 lowercase letters: ")).split())
if len(letters) < 3 or len(letters) > 10:
    print('Incorrect input, giving up...')
    sys.exit()
else:
    for letter in letters:
        if letter.isupper() or not letter.isalpha():
            print('Incorrect input, giving up...')
            sys.exit()

## Opens the input file and converts it to a list.
file = 'wordsEn.txt'
raw_words = open(file).readlines()
words=[]
for word in raw_words:
    words.append(''.join(word.split()))

## Creates a dictionary of all letter scores.
all_scores = {'a':2,'b':5,'c':4,'d':4,'e':1,'f':6,'g':5,'h':5,'i':1,
          'j':7,'k':6,'l':3,'m':5,'n':2,'o':3,'p':5,'q':7,'r':2,
          's':1,'t':2,'u':4,'v':6,'w':6,'x':7,'y':5,'z':7,"'":0}

## Creates a dictionary of letter scores for user input letters, including how many times
## each letter appears in the input.
def create_scores(letters):
    letter_scores = {}
    for letter in letters:
        if letter not in letter_scores:
            letter_scores.update({letter:[all_scores[letter],1]})
        else:
            letter_scores[letter][1]+=1
    return letter_scores

## Searches through the list of words until it finds one that can be made from the input
## letters. It then iterates over its letters and adds the scores to get the high_score,
## and appends the word to the high_score_words list. If it finds another words scoring the
## same, it will append this word also, and if it finds a higher scoring word, it updates
## the high_score and resets the high_score_words list with this new word.

high_score = 0
high_score_words = []

for word in words:
    score=0

    new_letter_scores=create_scores(letters)

    for letter in word:
        if letter in new_letter_scores and new_letter_scores[letter][1] > 0:
            score+=new_letter_scores[letter][0]
            new_letter_scores[letter][1]-=1
        else:
            score=0
            break
    if score > high_score:
        high_score = score
        high_score_words = [word]
    elif score == high_score:
        high_score_words.append(word)

if high_score == 0:
    print('No word is built from those letters.')
elif len(high_score_words) == 1:
    print('The highest score is {}.'.format(high_score))
    print('The highest scoring word is {}.'.format(high_score_words[0]))
else:
    print('The highest score is {}.'.format(high_score))
    print('The highest scoring words are, in alphabetical order:')
    for word in high_score_words:
        print('    ',word)
