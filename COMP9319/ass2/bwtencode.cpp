#include <iostream>
#include <fstream>
#include <cstring>
#include <stdlib.h>
using namespace std;



int getFileSize(string fileName) {
    ifstream input(fileName, ifstream::binary | ifstream::ate);
    return input.tellg();
}

int circlularMemcmp(const unsigned char* p1, const unsigned char* p2, size_t lengthToEnd, size_t lengthFromStart) {
    if (lengthToEnd == 0 && lengthFromStart == 0)
        return 0;

    int i = 0;
    while (i < lengthToEnd) {
        if (*p1 != *p2)
            return *p1 - *p2;
        p1++;
        p2++;
        i++;
    }
    //if ()
    
}

#define BUFFERLENGTH 8
char buffer[BUFFERLENGTH];
unsigned int rotationIndices[BUFFERLENGTH];

int circularCompare(const unsigned int *i1, const unsigned int *i2) {
    unsigned int endL1 = (unsigned int) (BUFFERLENGTH - *i1 + 1);
    unsigned int endL2 = (unsigned int) (BUFFERLENGTH - *i2 + 1);
        
    int indexToEnd = memcmp(buffer + *i1,buffer + *i2, endL1 < endL2 ? endL1 : endL2);

    cout << *i1<< buffer + *i1 << " | " << *i2 << buffer + *i2 << " : res: " << indexToEnd  << "\n";

    if (indexToEnd == 0) {
        if (endL1 < endL2) {
            int toEndOfFirst = memcmp(buffer, buffer + endL1, BUFFERLENGTH - endL1);
            if (toEndOfFirst == 0) {
                return memcmp(buffer + endL2, buffer, BUFFERLENGTH - endL2);
            }
            return toEndOfFirst;
        }
        else {
            int toEndOfFirst = memcmp(buffer + endL2, buffer, BUFFERLENGTH - endL2);
            if (toEndOfFirst == 0) {
                return memcmp(buffer + endL1, buffer, BUFFERLENGTH - endL1);
            }
            return toEndOfFirst;
        }
    }

    return indexToEnd;
};

void printBWT(char str[], int index[1]) {

}

main(int argc, char const *argv[])
{
    string fileName = "./example1.txt";
    //long fileLength = getFileSize(fileName);

    ifstream input(fileName);
    input.unsetf(ios_base::skipws);

    //char inputString[fileLength];
    //unsigned int rotationIndices[fileLength];

    unsigned int pos = 0;
    while (input) {
        input >> buffer[pos];
        rotationIndices[pos] = pos;
        pos++;
    }

    qsort(rotationIndices, BUFFERLENGTH, sizeof(unsigned int),
            (int(*)(const void *, const void *))circularCompare);

    for (const auto i: rotationIndices) {
        cout << buffer[BUFFERLENGTH-i-1];
    }
    cout << "\n";
    for (int i = 0; i <= BUFFERLENGTH; i++) {
        cout << buffer[i];
    }
    cout << "\n";

    for (const auto i:rotationIndices) {
        cout << i << " ";
    }
    cout << BUFFERLENGTH;
    return 0;
}



//     #include <stdio.h>
// #include <iostream>
// #include <map>
// #include <algorithm>
// #include <vector>
// #include <queue>
// #include <stack>;