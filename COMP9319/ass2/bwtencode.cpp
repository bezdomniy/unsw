#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <stdlib.h>
using namespace std;

int getFileSize(string fileName) {
    ifstream input(fileName, ifstream::binary | ifstream::ate);
    return input.tellg();
}


#define BUFFERLENGTH 40
char buffer[BUFFERLENGTH];
unsigned int rotationIndices[BUFFERLENGTH];

int circularCompare(const unsigned int *i1, const unsigned int *i2) {
    unsigned int endL1 = (unsigned int) (BUFFERLENGTH - *i1 );
    unsigned int endL2 = (unsigned int) (BUFFERLENGTH - *i2 );

    if (endL1 < endL2) {
        int bothWrappedAround = memcmp(buffer + endL1, buffer + endL2, BUFFERLENGTH - endL2);
        //cout << buffer + endL1 << " | " << buffer + endL2 << " : len: " << BUFFERLENGTH - endL2 << " res: " << bothWrappedAround  << "\n";
        if (bothWrappedAround == 0) {
            int oneWrapAround = memcmp(buffer + endL1 + BUFFERLENGTH - endL2, buffer, BUFFERLENGTH - endL1);
            //cout << "deeper1: " << buffer + endL1 + BUFFERLENGTH - endL2 << " | " << buffer << " : len: " << BUFFERLENGTH - endL1 << " res: " << oneWrapAround  << "\n";
            if (oneWrapAround == 0)
                return memcmp(buffer + *i1,buffer + *i2, endL1);
            return oneWrapAround;
        }
        return bothWrappedAround;
    }
    else {
        int bothWrappedAround = memcmp(buffer + endL1, buffer + endL2, BUFFERLENGTH - endL1);
        //cout << buffer + endL1 << " | " << buffer + endL2 << " : len: " << BUFFERLENGTH - endL1 << " res: " << bothWrappedAround  << "\n";
        if (bothWrappedAround == 0) {
            int oneWrapAround = memcmp(buffer, buffer + endL2 + BUFFERLENGTH - endL1, BUFFERLENGTH - endL2);
            //cout << "deeper2: " << buffer + endL2 + BUFFERLENGTH - endL1 << " | " << buffer << " : len: " << BUFFERLENGTH - endL2 << " res: " << oneWrapAround  << "\n";
            if (oneWrapAround == 0)
                return memcmp(buffer + *i1,buffer + *i2, endL2);
            return oneWrapAround;
        }
        return bothWrappedAround;
    }
        
};


main(int argc, char const *argv[])
{
    string fileName = "./example1.txt";
    //long fileLength = getFileSize(fileName);

    ifstream input(fileName);
    input.unsetf(ios_base::skipws);

    ofstream output;
    output.open("./example1.bwt");

    //char inputString[fileLength];
    //unsigned int rotationIndices[fileLength];

    int count = 1;
    unsigned int* indexPointer = rotationIndices;
    while (input.peek() != EOF) {
        unsigned int pos = 0;
        while (input) {
            input >> buffer[pos];
            rotationIndices[pos] = pos;
            pos++;

            if (pos == BUFFERLENGTH)
                break;
        }

        //unsigned int* indexPointer = rotationIndices;

        if (pos == BUFFERLENGTH) {
            qsort(rotationIndices, BUFFERLENGTH, sizeof(unsigned int),
                (int(*)(const void *, const void *))circularCompare);
        }
        else {
            unsigned int trimmedRotationIndicies[pos];
            copy_n(rotationIndices, pos, trimmedRotationIndicies);
            qsort(trimmedRotationIndicies, pos, sizeof(unsigned int),
                (int(*)(const void *, const void *))circularCompare);
            indexPointer = trimmedRotationIndicies;
        }


        //for (const auto i: rotationIndices) {
        for (int i = 0; i < BUFFERLENGTH; i++) {
            if (i > pos)
                break;
            cout << buffer[BUFFERLENGTH-rotationIndices[i]-1];
        }

        cout << count * 10000 << "\n";
        count++;
    }
    output.close();

    // for (const auto c:buffer) {
    //     cout << c;
    // }
    // cout << "|\n";

    cout << "\n";
    //for (const auto i:indexPointer) {
    for (int i = 0; i < BUFFERLENGTH; i++) {
        cout << *(indexPointer+i) << " ";
    }

    vector<int> v = {1,2,3};
    v.resize(2);
    for (const auto i: v) cout << i;

    return 0;

}

