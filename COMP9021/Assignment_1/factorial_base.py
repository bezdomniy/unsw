import sys

## Prompts the user to input a number and checks if it is valid.
try:
    input_integer = int(input('Input a nonnegative integer: '))
    if input_integer < 0:
        raise ValueException
except:
    print('Incorrect input, giving up...')
    sys.exit()

integer=input_integer

## Prints factorial base of 0 as a special case.
if integer == 0:
    print('Decimal 0 reads as 0 in factorial base.')
    sys.exit()

count=2
result=[]

## Generates a list of digits in factorial base.
while integer > 0:
    result.append(str(integer % count))
    integer = integer // count
    count += 1

## Prints the original input and the reversed generates list for final answer.
print('Decimal {} reads as {} in factorial base.'.format(input_integer,''.join(result[::-1])))

