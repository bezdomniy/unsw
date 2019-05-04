bwtencode:

My solution takes the very simplest approach to bwt encoding:
  1. I read the input file into an integer array (buffer) with each value being the ascii value of character plus an offset value to allow each delimiter have its own unique value starting at 0 and the largest possible value being N/4 where N is the length of the input file. This maximum offset is based on the assignment spec saying that there will not be more than N/4 delimiters in any file.
  2. As I said, the delimiters are assigned unique values - all being lower than any value of the non-delimiter characters.
  3. I initialise a suffix array (rotationIndices) with numbers 0 to N + 1, and then I simply use std::sort to sort them based on the corresponding character value in the buffer. I do this with a custom comparator which compares suffixes integer by integer until they are different. If the suffixes are identical until the end of one of the suffixes, the shorter suffix wins.
  4. I iterate over the sorted suffix array and write the corresponding (suffix index - 1) buffer character to the bwt output file - remembering to remove the character offset mentioned in point #1 before converting back to char.
  5. If the character is a delimiter, I simply write the delimiter value, and write the position of the character to the aux file for later use in search.

I spent way too long implementing the dc3 linear time algorithm, and even longer trying to figure out a clever sequence of temporary files to make the recursive steps run within the memory requirements. After I was unsuccessful, and after reading a forum post about the nature of the data that it will be tested on (no long duplicate substrings) I decided to just submit the simple, nonlinear time implementation.

This implementation is not the best, and will fail on files with long sequences of common suffixes. The reason I tried the dc3 algorithm is because I tested this solution with a file of a book appended 3 times, and it failed to complete in any reasonable time. 

This method could also be improved by figuring out a better way to sort the file taking into account delimiters. If I had more time, I would have tried to do it without converting the original char array into an int array, because this increases the memory usage 4 folder for this array. I did not use any temporary files for this part.


bwtsearch:

My solution uses a block-wise method to write the index file for use in calculating the C and the Occ functions. I iterate through the file and every 515 characters, I write 127 * 4 bytes to the index file. This denotes the number of each of the possible ascii characters up to that point in the file.

When I want to calculate Occ, I simply find the closest block after the desired index given to the function, and iterate backwards until the previous block, subtracting from the Occ of the desired character each time one is encountered.

Calculating the C function is very simple, I just take the values in the last block of the index file.

Options:
    I built the follow 4 options sequentially, and each one largely builds off the previous one. 

    -m:
        For this I simply implemented the backward search algorithm presented in the lectures. Start at the end of the pattern string and calculate the first and last elements occurrence for each character using C and Occ as explained above. This then returns last-first+1 to get the non-unique count.
        
    -n:
        To get the unique count, we then need to figure out the records which each of the non-unique instances of the pattern belong to. This is done by doing backward search until a delimiter is reached. We then find what number delimiter it is in the bwt file, and then we look up the aux file to get the corresponding position number for this delimiter (this is done for each instance of the pattern). We put these into a set, to remove duplicates, then simply output the size of the set.
    
    -a:
        The is very similar to -n. The only difference is that we use an ordered set so the record numbers are sorted, and we iterate over the set and output the elements instead of just outputting the size.
        
    -i:
        This is similar to -n. We specify the beginning and end records in the argument, then doing backward search. We don't need the aux file, because we are already iterating backwards over the record. To print this properly, we push each character to the front of a double-ended queue (deque) until the previous delimiter is reached. We then pop from the front of the deque to print the record (this is done for each record). 
