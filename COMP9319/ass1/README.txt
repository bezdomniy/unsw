Program: huffman
Source: huffman.cpp
Language: C++11
Author: Ilia Chibaev
StudentID: z3218424

Huffman Tree:
I implemented a basic binary tree with a character, frequency of occurrence, and pointers for the left and right child nodes.

Top build the tree, my program:
1. Runs through the file to create a frequency table (of pairs: character, frequency). 
2. Creates a leaf node from each pair in the table and add them to a priority queue, ordered in descending order of frequency.
3. Until the priority queue has only 1 node, my program pops 2 nodes from the priority queue and creates a new node with each of these nodes as children. The value of this new node is the sum of the values of the child nodes. It then adds this new node back into the queue.
4. The root node of the tree is then this remaining node in the priority queue.

Header:
My header stores the frequency table. Each entry is stored in 4 bytes, with the first 3 bytes being the frequency, and the 4th byte being the character. Since I am using unsigned char, I am able to store all 256 extended ascii characters, which is needed to encode binary files.

I chose 4 bytes per entry because this is the maximum size that will allow capacity for 256 unique characters in the allowed header space (4 * 256 = 1024).

I needed to store an additional byte that represents the number of valid bits in the last byte of the encoded file, otherwise the last few decoded characters would be invalid. Since I am already potentially using the whole space of the header file (as shown in the previous paragraph), I decided to store this value in the third byte (through a quirk of implementation, my frequency bytes are written backwards). In order to avoid the chance that it will overwrite some valid data about the first character (if its frequency is greater than 65536), I sort the frequency table in descending order of value before writing it to the file. Of course if the least common character in the file appears more than 65536 times, then my implementation will fail. To mitigate this (though not if 256 unique characters all appear at least 65536 times) I could store another bit that identifies whether the file has 256 unique characters, and if not, store this additional byte in the last byte of the header instead.

Search
My search implementation is a very simple sliding window search. I ran out of time to implement something more sophisticated, but this works fine and it reasonably fast (though not on larger files).

I use the same code as I used to decode my .huffman file, but instead of writing each character to a file, I add the front of a double-ended queue buffer. When the buffer becomes the same length as the search string, I compare these and increment a match counter if there is a match. I then pop a character from the back of the queue.

I do this window search across the whole file and output the match counter when I reach the end.