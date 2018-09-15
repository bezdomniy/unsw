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

void bucketSortSuffixArray(unsigned int *startOfArrayPtr, unsigned int sortCharIndex=0) {
    vector<list<unsigned int>> buckets(126);

    list<unsigned int>::iterator it;
    for (unsigned int *inputPtr = startOfArrayPtr; *inputPtr; ++inputPtr) {
        //(*inputPtr)--;
        cout << "sorting on: " << buffer[*inputPtr+sortCharIndex] << "\n";
        it = buckets[int(buffer[*inputPtr+sortCharIndex])].end();
        buckets[int(buffer[*inputPtr+sortCharIndex])].insert(it, *inputPtr);
    }

    for (auto bucket: buckets) {
        for (const auto element: bucket) {
            //cout << "changing: " << *startOfArrayPtr << " to: " << element << "\n";
            *startOfArrayPtr = element;
            startOfArrayPtr++;
        }
    }
    cout <<"end pass\n";
}


void ds3SuffixArray(unsigned int *startOfArrayPtr) {
    int nMod3Suffixes0 = (currentEnd+2)/3;
    int nMod3Suffixes1 = (currentEnd+1)/3;
    int nMod3Suffixes2 = (currentEnd+0)/3;

    unsigned int R[nMod3Suffixes1 + nMod3Suffixes2 + 2];
    R[nMod3Suffixes1 + nMod3Suffixes2]=0;
    R[nMod3Suffixes1 + nMod3Suffixes2 + 1]=0;

    int sample;
    for (int i = 0, j = 0; i < currentEnd; i++) {
        sample = i % 3;
        if (sample != 0)
            R[j++] = i;
    }

    for (auto i: R)
        cout << i <<" ";
    cout<<"\n";
    bucketSortSuffixArray(R,2);
        for (auto i: R)
        cout << i <<" ";
    cout<<"\n";
    bucketSortSuffixArray(R,1);
        for (auto i: R)
        cout << i <<" ";
    cout<<"\n";
    bucketSortSuffixArray(R,0);

    for (auto i: R)
        cout << i <<" ";
}


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

    while (input.peek() != EOF) {
        unsigned int pos = 0;
        while (input.peek() != EOF && pos < BUFFERLENGTH) {
            input >> buffer[pos];

            //can't iterate through 0 pointer so starting pos at 1
            rotationIndices[pos] = pos+1;
            pos++;
        }

        if (pos == BUFFERLENGTH) {
            //bucketSortSuffixArray(&rotationIndices[0]);
        }
        else {
            currentEnd = pos;
            rotationIndices.resize(pos);

            ds3SuffixArray(&rotationIndices[0]);
            //bucketSortSuffixArray(&rotationIndices[0]);
        }

        // vector<int> test = {1,2,3};
        // vector<int>::iterator ite = test.begin();
        // cout << *ite;
        // ite++;
        // cout << *ite;

        for (int i = 0; i < currentEnd; i++) {
            // subtract 1 from suffix index to get bwt
            if (rotationIndices[i] > 0)
                cout << buffer[rotationIndices[i]-1];
            else
                cout << buffer[pos-1];
        }

        //cout << count * BUFFERLENGTH << "\n";
        count++;
    }
    output.close();


    cout << "\n";

    return 0;

}

