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

#define BUFFERLENGTH 50000000
int currentEnd = BUFFERLENGTH;
char buffer[BUFFERLENGTH];
vector<unsigned int> rotationIndices(BUFFERLENGTH);
unsigned int* rankR = new unsigned int[currentEnd+2];

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



bool rankCompare(const unsigned int i1, const unsigned int i2) {
    return rankR[i1+1] < rankR[i2+1];
}

void bucketSortPass(unsigned int *startOfArrayPtr, unsigned int sortCharIndex=0) {
    vector<list<unsigned int>> buckets(126);
    list<unsigned int>::iterator it;
    
    bool firstZeroElement = false;
    if (*startOfArrayPtr ==0) {
        (*startOfArrayPtr)++;
        firstZeroElement = true;
    }

    //cout << "pass" << sortCharIndex << "\n";
        
    for (unsigned int *inputPtr = startOfArrayPtr; *inputPtr; ++inputPtr) {
        if (firstZeroElement) {
            (*inputPtr)--;
            firstZeroElement = false;
        }

        
            
        it = buckets[int(buffer[*inputPtr+sortCharIndex])].end();
        buckets[int(buffer[*inputPtr+sortCharIndex])].insert(it, *inputPtr);
    }

    for (auto bucket: buckets) {
        bucket.sort(rankCompare);
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

    const unsigned int rSize = nMod3Suffixes0 + nMod3Suffixes2 +3;

    unsigned int* R = new unsigned int[rSize];
    R[nMod3Suffixes1 + nMod3Suffixes2]=0;
    R[nMod3Suffixes1 + nMod3Suffixes2 + 1]=0;

    unsigned int* R0 = new unsigned int[nMod3Suffixes0];


    for (int i = 0, j = 0, k=0; i < currentEnd; i++) {
        if (i % 3 != 0)
            R[j++] = i;
        else
            R0[k++] = i;
    }

    
    // for (int i = 0; i  < rSize; i++)
    //     cout << R[i] << " ";
    // cout << "\n";
    // for (int i = 0; i  < nMod3Suffixes0 ; i++)
    //     cout << R0[i] << " ";
    // cout << "\n";

    bucketSortPass(R,2);
    bucketSortPass(R,1);
    bucketSortPass(R,0);

    
    for (int i = 0; i < currentEnd; i++) rankR[i] = -1;
    rankR[currentEnd] = 0;
    rankR[currentEnd+1] = 0;

    int j =1;
    for (size_t i = 0; i < rSize; i++) {
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
        rankR[R0[i]] = j;
        j++;
    }

    // for (size_t i = 0; i < currentEnd+2; i++) 
    //     cout << rankR[i] << " ";
    // cout << "\n";

    for (int i = 0, j = 0; i + j < currentEnd;) {
        //cout << "comparing " << buffer[R[i]] << " and " << buffer[R0[j]] << "\n";
        //cout << "comparing: " << buffer[R[i]] << " and " << buffer[R0[j]] << "\n";

        if (j == nMod3Suffixes0) {
            //cout << "inputing0 " << R[i] << " " << buffer[R[i]] << "\n";
            startOfArrayPtr[i+j] = R[i++];
        }

        else if (buffer[R[i]] < buffer[R0[j]]) {
            startOfArrayPtr[i+j] = R[i];
            //cout << "inputing1 " << R[i] << " " << buffer[R[i]] << "\n";
            i++;
        }
            
        else if (buffer[R[i]] > buffer[R0[j]]) {
            startOfArrayPtr[i+j] = R0[j];
            //cout << "inputing2 " << R0[j] << " " << buffer[R0[j]] << "\n";
            j++;
        }
            
        else {
            if (R[i] % 3 == 1) {
                //cout << "inputing3 " << R[i] << " " << buffer[R[i]] << "\n";
                startOfArrayPtr[i+j] = rankR[R[i] +1] < rankR[R0[j] +1] ? R[i++] : R0[j++];
            }
            else {
                if (buffer[R[i]+1] < buffer[R0[j]+1]) {
                    startOfArrayPtr[i+j] = R[i];
                    //cout << "inputing4 next letters: " << buffer[R[i]+1] << " and " << buffer[R0[j]+1] << "\n";
                    i++;
                }
                    
                else if (buffer[R[i]+1] > buffer[R0[j]+1]) {
                    //cout << "inputing5 next letters: " << buffer[R[i]+1] << " and " << buffer[R0[j]+1] << "\n";
                    startOfArrayPtr[i+j] = R0[j];
                    j++;
                }
                else {
                    //cout << "inputing6 " << R[i] << " " << buffer[R[i]] << "\n";
                    startOfArrayPtr[i+j] = rankR[R[i] +2] < rankR[R0[j] +2] ? R[i++] : R0[j++];
                }
            }
        }
        //cout << "i " << i << " j " << j << "\n";
    }
    // for (size_t i = 0; i < currentEnd; i++) 
    //     cout << startOfArrayPtr[i] << " ";
    // cout << "\n";
    delete [] R; delete [] R0; delete [] rankR;
}


main(int argc, char const *argv[])
{
    string fileName = "./warandpeace.txt";
    //long fileLength = getFileSize(fileName);

    ifstream input(fileName);
    input.unsetf(ios_base::skipws);

    ofstream output;
    output.open("./warandpeace.bwt");

    //char inputString[fileLength];
    //unsigned int rotationIndices[fileLength];

    int count = 1;

    while (input.peek() != EOF) {
        unsigned int pos = 0;
        while (input.peek() != EOF && pos < BUFFERLENGTH) {
            input >> buffer[pos];

            //can't iterate through 0 pointer so starting pos at 1
            rotationIndices[pos] = pos;
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
                output << buffer[rotationIndices[i]-1];
            else
                output << buffer[pos-1];
            //cout << rotationIndices[i] <<" ";
        }

        //cout << count * BUFFERLENGTH << "\n";
        count++;
    }
    output.close();


    //cout << "\n";

    return 0;

}

