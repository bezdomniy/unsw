#include <iostream>
#include <fstream>
#include <algorithm>
#include <list>
using namespace std;

int getFileSize(string fileName) {
    ifstream input(fileName, ifstream::binary | ifstream::ate);
    return input.tellg();
}

#define BUFFERLENGTH 50000000
int currentEnd = BUFFERLENGTH;
//char buffer[BUFFERLENGTH];
vector<char> buffer(BUFFERLENGTH);
vector<unsigned int> suffixArray(BUFFERLENGTH);
unsigned int* rankR = new unsigned int[currentEnd+2];

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

    j =0;
    for (size_t i = 0; i < nMod3Suffixes0; i++) {
        rankR[R0[i]] = j;
        j++;
    }

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
    string fileName = "./warandpeace3.txt";

    ifstream input(fileName);
    input.unsetf(ios_base::skipws);

    ofstream output;
    output.open("./warandpeace3.bwt");

    //char inputString[fileLength];
    //unsigned int suffixArray[fileLength];

    //int count = 1;

    while (input.peek() != EOF) {
        unsigned int pos = 0;
        while (input.peek() != EOF && pos < BUFFERLENGTH) {
            input >> buffer[pos];
            suffixArray[pos] = pos;
            pos++;
        }

        // if (pos == BUFFERLENGTH) {
        //     //bucketSortSuffixArray(&suffixArray[0]);
        // }
        // else {
            currentEnd = pos;

            suffixArray.resize(currentEnd);
            suffixArray.shrink_to_fit();

            buffer.resize(currentEnd);
            buffer.shrink_to_fit();

            ds3SuffixArray(&suffixArray[0]);
        //}

        // vector<int> test = {1,2,3};
        // vector<int>::iterator ite = test.begin();
        // cout << *ite;
        // ite++;
        // cout << *ite;

        for (int i = 0; i < currentEnd; i++) {
            // subtract 1 from suffix index to get bwt
            if (suffixArray[i] > 0)
                output << buffer[suffixArray[i]-1];
            else
                output << buffer[pos-1];
            //cout << suffixArray[i] <<" ";
        }

        //cout << count * BUFFERLENGTH << "\n";
        //count++;
    }
    output.close();


    //cout << "\n";

    return 0;

}

