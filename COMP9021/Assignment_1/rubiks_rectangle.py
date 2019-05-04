import sys

## Prompts the user to input the desired final configuration and checks the validity of the input.

try:
    raw_input = [int(x) for x in input('Input initial configuration: ').replace(" ","")]
    if {x for x in raw_input} != {1,2,3,4,5,6,7,8} or len(raw_input) != 8:
        raise ValueException
except:
    print('Your input is incorrect, try again.')
    sys.exit()

## Defines the rubiks rectangle object, using a list of 2 lists (one for each row), taking a list of integers as input.
## This defines the hashing algorithm to check whether a rectangle is in a set of rectangles based on its digit layout.
## The functions are defined to perform the 3 transformations allowed.
    
class rubiks_rect(object):
    def __init__(self,input_digits):
        self.digits = [[],[]]
        for i in range(0,len(input_digits)//2):
            self.digits[0].append(input_digits[i])
        for j in range(1,5):
            self.digits[1].append(input_digits[-j])

    def __hash__(self):
        return hash(tuple(x for x in self.digits[0]+self.digits[1]))

    def __eq__(self,other):
        return (tuple(x for x in self.digits[0]+self.digits[1])==tuple(x for x in other.digits[0]+other.digits[1]))
    
    def row_exchange(self):
        output = rubiks_rect([*self.digits[-1],*self.digits[0][::-1]])
        return output

    def right_circular_shift(self):
        output = rubiks_rect([self.digits[0][3],self.digits[0][0],self.digits[0][1],self.digits[0][2], self.digits[1][2],self.digits[1][1],self.digits[1][0],self.digits[1][3]])
        return output

    def middle_clockwise_rotation(self):
        output=rubiks_rect([self.digits[0][0],self.digits[1][1],self.digits[0][1],self.digits[0][3],self.digits[1][3],self.digits[0][2],self.digits[1][2],self.digits[1][0]])
        return output

## Defines the initial configuration, final state from user input, and the first element in the set of states seen so far.

initial_state=rubiks_rect([1,2,3,4,5,6,7,8])
final_state=rubiks_rect(raw_input)

states_so_far = {initial_state}

## This function find the minimal number of steps needed to go from the inital state to the user input final state.
## It takes the unique states seen so far and the number of steps that have been tested as an arguement.
## It checks whether the final state is in the set of states seen so far, and if this returns False then it runs
## each of the 3 transformations on the states generated in the previous run of the function, and appends the set of
## unique states seen so far with the results.
## The function then calls itself with the new set of results and the iteration count as arguements, and continues
## recursively until the initial condition is satisfied, at which point it returns the count of how many iterations
## it has taken to reach the final state.

def find_least_moves(input_states,count=0):
    result=set(input_states)
    if final_state not in result:
        for state in input_states:
            set_of_moves=[state.row_exchange,state.right_circular_shift,state.middle_clockwise_rotation]
            for move in range(len(set_of_moves)):
                new_state = set_of_moves[move]()
                if new_state not in states_so_far:
                    states_so_far.add(new_state)
                    result.add(new_state)

        result = result-input_states
        count+=1

        return find_least_moves(result,count)
    return count

output = find_least_moves({initial_state})

if output in (0,1):
    print('{} step is needed to reach the final configuration.'.format(output))
else:
    print('{} steps are needed to reach the final configuration.'.format(output))
