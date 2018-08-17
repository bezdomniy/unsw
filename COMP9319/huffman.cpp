#include <stdio.h>
#include <iostream>
#include <map>
#include <algorithm>

#define BUFFERLENGTH 1

typedef struct Node {
    unsigned int data;
    char letter;
    struct node *leftChild;
    struct node *rightChild;
} Node;

typedef std::pair<char, int> HuffmanPair;
struct CompareValue
{
    bool operator()(const HuffmanPair & left, const HuffmanPair & right) const
    {
        return left.second < right.second;
    }
};

HuffmanPair getMin(std::map<char, int> mymap) 
{
    std::pair<char, int> min 
      = *std::min_element(mymap.begin(), mymap.end(), CompareValue());
    return min;
}

int main(int argc, char const *argv[])
{
    FILE *filePointer = fopen("warandpeace.txt", "r");
    
    char buffer[BUFFERLENGTH+1];
    size_t charactersRead = 0;
    
    std::map<char,int> frequencyTable;
    
    if (filePointer != NULL) {
        while (charactersRead = fread(buffer, sizeof(char), BUFFERLENGTH, filePointer) > 0) {
            char character = *buffer;

            if (frequencyTable.find(character) == frequencyTable.end()) {
                frequencyTable[character] = 1;
            }
            else {
                frequencyTable[character]++;
            }
        }

        for (const auto &p : frequencyTable) {
            std::cout << "frequencyTable[" << p.first << "] = " << p.second << '\n';
        }
    }

    HuffmanPair min = getMin(frequencyTable);


    std::cout << min.first << '\n';

    return 0;
}
