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


#define BUFFERLENGTH 50000000
int currentEnd = BUFFERLENGTH;
char buffer[BUFFERLENGTH];
vector<unsigned int> rotationIndices(BUFFERLENGTH);

int suffixCompare(const unsigned int *i1, const unsigned int *i2) {
    unsigned int endL1 = (unsigned int) (currentEnd - *i1 );
    unsigned int endL2 = (unsigned int) (currentEnd - *i2 );

    int res = memcmp(buffer + *i1,buffer + *i2, endL1 < endL2 ? endL1 : endL2);
    if ( res == 0 )
        return endL1 - endL2;
    else
        return res;

};

int circularCompare(const unsigned int *i1, const unsigned int *i2) {
    unsigned int endL1 = (unsigned int) (currentEnd - *i1 );
    unsigned int endL2 = (unsigned int) (currentEnd - *i2 );

    if (endL1 < endL2) {
        int bothWrappedAround = memcmp(buffer + endL1, buffer + endL2, currentEnd - endL2);
        //cout << buffer + endL1 << " | " << buffer + endL2 << " : len: " << BUFFERLENGTH - endL2 << " res: " << bothWrappedAround  << "\n";
        if (bothWrappedAround == 0) {
            int oneWrapAround = memcmp(buffer + endL1 + currentEnd - endL2, buffer, currentEnd - endL1);
            //cout << "deeper1: " << buffer + endL1 + BUFFERLENGTH - endL2 << " | " << buffer << " : len: " << BUFFERLENGTH - endL1 << " res: " << oneWrapAround  << "\n";
            if (oneWrapAround == 0)
                return memcmp(buffer + *i1,buffer + *i2, endL1);
            return oneWrapAround;
        }
        return bothWrappedAround;
    }
    else {
        int bothWrappedAround = memcmp(buffer + endL1, buffer + endL2, currentEnd - endL1);
        //cout << buffer + endL1 << " | " << buffer + endL2 << " : len: " << BUFFERLENGTH - endL1 << " res: " << bothWrappedAround  << "\n";
        if (bothWrappedAround == 0) {
            int oneWrapAround = memcmp(buffer, buffer + endL2 + currentEnd - endL1, currentEnd - endL2);
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
    string fileName = "./warandpeace3.txt";
    //long fileLength = getFileSize(fileName);

    ifstream input(fileName);
    input.unsetf(ios_base::skipws);

    ofstream output;
    output.open("./warandpeace3.bwt");

    //char inputString[fileLength];
    //unsigned int rotationIndices[fileLength];

    int count = 1;

    while (input.peek() != EOF) {
        unsigned int pos = 0;
        while (input.peek() != EOF && pos < BUFFERLENGTH) {
            input >> buffer[pos];
            //rotationIndices.at(pos) = pos;
            rotationIndices[pos] = pos;
            pos++;
        }

        if (pos == BUFFERLENGTH) {
            qsort(&rotationIndices[0], BUFFERLENGTH, sizeof(unsigned int),
                (int(*)(const void *, const void *))suffixCompare);
        }
        else {
            currentEnd = pos;
            rotationIndices.resize(pos);
            qsort(&rotationIndices[0], pos, sizeof(unsigned int),
                (int(*)(const void *, const void *))suffixCompare);
        }


        // subtract 1 from suffix index to get bwt
        for (int i = 0; i < currentEnd; i++) {
            if (rotationIndices[i] > 0)
                rotationIndices[i]--;
            else
                rotationIndices[i] = pos-1;
        }


        for (int i = 0; i < currentEnd; i++) {
            //cout << buffer[currentEnd-rotationIndices[i]-1];
            output << buffer[rotationIndices[i]];
        }

        cout << count * BUFFERLENGTH << "\n";
        count++;
    }
    output.close();


    cout << "\n";

    return 0;

}

