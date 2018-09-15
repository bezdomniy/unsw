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

void bucketSortPass(unsigned int *startOfArrayPtr, unsigned int sortCharIndex=0) {
    vector<list<unsigned int>> buckets(126);
    list<unsigned int>::iterator it;
    
    bool firstZeroElement = false;
    if (*startOfArrayPtr ==0) {
        (*startOfArrayPtr)++;
        firstZeroElement = true;
    }
        
    for (unsigned int *inputPtr = startOfArrayPtr; *inputPtr; ++inputPtr) {
        if (firstZeroElement) {
            (*inputPtr)--;
            firstZeroElement = false;
        }
            
        it = buckets[int(buffer[*inputPtr+sortCharIndex])].end();
        buckets[int(buffer[*inputPtr+sortCharIndex])].insert(it, *inputPtr);
    }

    for (auto bucket: buckets) {
        for (const auto element: bucket) {
            *startOfArrayPtr = element;
            startOfArrayPtr++;
        }
    }
}


void ds3SuffixArray(unsigned int *startOfArrayPtr) {
    unsigned int nMod3Suffixes0 = (currentEnd+2)/3;
    unsigned int nMod3Suffixes1 = (currentEnd+1)/3;
    unsigned int nMod3Suffixes2 = (currentEnd+0)/3;

    unsigned int* R = new unsigned int[nMod3Suffixes1 + nMod3Suffixes2 + 3];
    R[nMod3Suffixes1 + nMod3Suffixes2]=0;
    R[nMod3Suffixes1 + nMod3Suffixes2 + 1]=0;

    unsigned int* R0 = new unsigned int[nMod3Suffixes0];


    int sample;
    for (int i = 0, j = 0, k=0; i < currentEnd; i++) {
        sample = i % 3;
        if (sample != 0)
            R[j++] = i;
        else
            R0[k++] = i;
    }
    // for (auto i: R)
    //       cout << i <<" ";
    // cout << "\n";

    bucketSortPass(R,2);
    bucketSortPass(R,1);
    bucketSortPass(R,0);

    

    unsigned int* rankR = new unsigned int[currentEnd+2];
    for (int i = 0; i <= currentEnd; i++) rankR[i] = -1;
    rankR[currentEnd] = 0;
    rankR[currentEnd+1] = 0;

    int j =1;
    for (size_t i = 0; i < nMod3Suffixes1 + nMod3Suffixes2 + 3; i++) {
        if (R[i] != 0)
            rankR[R[i]] = j;
        j++;
    }

    bucketSortPass(R0,0);
    //int rankR0[currentEnd+3];
    //for (int i = 0; i <= currentEnd; i++) rankR0[i] = -1;
    //rankR0[currentEnd+1] = 0;
    //rankR0[currentEnd+2] = 0;

    j =0;
    //for (const auto i: R0) {
    for (size_t i = 0; i < nMod3Suffixes0; i++) {
        //if (i != 0)
        rankR[R[i]] = j;
        j++;
    }


    // for (auto i: R)
    //       cout << i <<" ";
    // cout << "\n";
    // for (auto i: R0)
    //       cout << i <<" ";
    // cout << "\n";
    // for (auto i: rankR)
    //       cout << i <<" ";
    // cout << "\n";
    unsigned int* out = new unsigned int[currentEnd];
    for (int i = 0, j = 0; i + j < currentEnd;) {
        //cout << "comparing: " << buffer[R[i]] << " and " << buffer[R0[j]] << "\n";
        if (buffer[R[i]] < buffer[R0[j]]) {
            out[i+j] = R[i];
            i++;
        }
            
        else if (buffer[R[i]] > buffer[R0[j]]) {
            out[i+j] = R0[j];
            j++;
        }
            
        else {
            if (R[i] % 3 == 1) {
                out[i+j] = rankR[R[i] +1] < rankR[R0[j] +1] ? R[i++] : R0[j++];
            }
            else {
                if (buffer[R[i]+1] < buffer[R0[j]+1]) {
                    out[i+j] = R[i];
                    i++;
                }
                    
                else if (buffer[R[i]+1] > buffer[R0[j]+1]) {
                    out[i+j] = R0[j];
                    j++;
                }
                else {
                    out[i+j] = rankR[R[i] +2] < rankR[R0[j] +2] ? R[i++] : R0[j++];
                }
            }
        }
    }
    delete [] R; delete [] R0; delete [] rankR;

    // for (auto i: out)
    //     cout << i <<" ";
    delete [] out;
}


main(int argc, char const *argv[])
{
    string fileName = "./warandpeace15.txt";
    //long fileLength = getFileSize(fileName);

    ifstream input(fileName);
    input.unsetf(ios_base::skipws);

    ofstream output;
    output.open("./warandpeace15.bwt");

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
            currentEnd = pos+1;
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
                output << buffer[rotationIndices[i]-1];
            else
                output << buffer[pos-1];
        }

        //cout << count * BUFFERLENGTH << "\n";
        count++;
    }
    output.close();


    //cout << "\n";

    return 0;

}

