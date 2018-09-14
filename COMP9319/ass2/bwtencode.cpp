#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <stdlib.h>

#include <list>
using namespace std;

int getFileSize(string fileName) {
    ifstream input(fileName, ifstream::binary | ifstream::ate);
    return input.tellg();
}

#define BUFFERLENGTH 10000000
int currentEnd = BUFFERLENGTH;
char buffer[BUFFERLENGTH];
vector<unsigned int> rotationIndices(BUFFERLENGTH);

// int suffixCompare(const unsigned int *i1, const unsigned int *i2) {
//     unsigned int endL1 = (unsigned int) (currentEnd - *i1 );
//     unsigned int endL2 = (unsigned int) (currentEnd - *i2 );

//     int res = memcmp(buffer + *i1,buffer + *i2, endL1 < endL2 ? endL1 : endL2);
//     if ( res == 0 )
//         return (endL1 - endL2);
//     else
//         return res ;

// };

bool suffixCompare(const unsigned int i1, const unsigned int i2) {
    unsigned int endL1 = (unsigned int) (currentEnd - i1 );
    unsigned int endL2 = (unsigned int) (currentEnd - i2 );

    int res = memcmp(buffer + i1,buffer + i2, endL1 < endL2 ? endL1 : endL2);
    //cout << "1st:" << buffer + *i1 << "|2nd:" << buffer + *i2 << "|";
    if ( res == 0 ) {
        //printf(" - res: %i\n",(endL1 - endL2) > 0);
        return (endL1 - endL2) > 0;
    }
        
    else {
        //printf(" - res: %i\n",res < 0);
        return res < 0 ;
    }
};

void bucketSortSuffixArray(unsigned int *startOfArrayPtr) {
    vector<list<unsigned int>> buckets(126);

    list<unsigned int>::iterator it;
    for (unsigned int *inputPtr = startOfArrayPtr; *inputPtr; ++inputPtr) {
        (*inputPtr)--;
        it = buckets[int(buffer[*inputPtr])].begin();

        //check the order here
        while (suffixCompare(*it,*inputPtr)) {
            if (it == buckets[int(buffer[*inputPtr])].end()) {
                break;
            }
            it++; 
        }
        //cout << "inserting: " << *inputPtr << " into bucket: \"" << (buffer[*inputPtr]) << "\"\n";
        buckets[int(buffer[*inputPtr])].insert(it, *inputPtr);
        //cout << "inserted.\n";
    }

    for (auto bucket: buckets) {
        // need to change this to compare at insertion
        //bucket.sort(suffixCompare);
        for (const auto element: bucket) {
            //cout << "changing: " << *startOfArrayPtr << " to: " << element << "\n";
            *startOfArrayPtr = element;
            startOfArrayPtr++;
        }
    }
}




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

            //can't iterate through 0 pointer so starting pos at 1
            rotationIndices[pos] = pos +1;
            pos++;
        }

        if (pos == BUFFERLENGTH) {
            bucketSortSuffixArray(&rotationIndices[0]);
        }
        else {
            currentEnd = pos;
            rotationIndices.resize(pos);
            bucketSortSuffixArray(&rotationIndices[0]);
        }

        // vector<int> test = {1,2,3};
        // vector<int>::iterator ite = test.begin();
        // cout << *ite;
        // ite++;
        // cout << *ite;

        for (int i = 0; i < currentEnd; i++) {
            // subtract 1 from suffix index to get bwt
            if (rotationIndices[i] > 0)
                output << buffer[rotationIndices[i]-1];
            else
                output << buffer[pos-1];
        }

        //cout << count * BUFFERLENGTH << "\n";
        count++;
    }
    output.close();


    cout << "\n";

    return 0;

}

