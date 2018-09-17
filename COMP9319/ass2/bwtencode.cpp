#include <iostream>
#include <fstream>
#include <algorithm>
#include <list>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

using namespace std;

unsigned int* rankR;

unsigned int getFileSize(string fileName) {
    ifstream input(fileName, ifstream::binary | ifstream::ate);
    return input.tellg();
}

template<typename T>
T* makeMmap(char* fileName, unsigned int size) {
    int fd = open(fileName, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    unsigned int fileSize = size * sizeof(T);
    lseek(fd, fileSize-1, SEEK_SET);
    write(fd, "", 1);
    return static_cast<T*>(mmap(0, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
}

bool rankCompare(const unsigned int i1, const unsigned int i2) {
    return rankR[i1+1] < rankR[i2+1];
}

#define VALIDCHARS 126
void bucketSortPass(unsigned int *startOfArrayPtr, char *buffer, unsigned int sortCharIndex=0, bool withRank = false) {
    unsigned int count[VALIDCHARS];
    unsigned int lengths[VALIDCHARS];
    
    for (int i=0;i<VALIDCHARS;i++) {
        count[i]=0;
        lengths[i]=0;
    }

    for (char *inputPtr = buffer; *inputPtr; ++inputPtr) {
        count[inputPtr[0+sortCharIndex]]++; 
    }
    //for (auto i: count) cout << i << " ";

    unsigned int* buckets[126];

    for (int i = 0; i < VALIDCHARS; i++) {
        if (count[i] > 0) 
            buckets[i] = new unsigned int[count[i]];
        else
            buckets[i] = 0;
    }


    //list<unsigned int>::iterator it;
    
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

        buckets[int(buffer[*inputPtr+sortCharIndex])][lengths[int(buffer[*inputPtr+sortCharIndex])]++] = *inputPtr;
        //it = buckets[int(buffer[*inputPtr+sortCharIndex])].end();
        //buckets[int(buffer[*inputPtr+sortCharIndex])].insert(it, *inputPtr);
    }

    for (int i = 0; i < VALIDCHARS; i++) {
        if (withRank)
            sort(buckets[i], buckets[i]+lengths[i], rankCompare);
            //buckets[i].sort(rankCompare);
        //for (const auto element: buckets[i]) {
        for (int j = 0; j < lengths[i]; j++) {
            *startOfArrayPtr = buckets[i][j];
            startOfArrayPtr++;
        }
        if (buckets[i] != 0)
            delete [] buckets[i];
    }
    //cout << sizeof(buckets);
}


void ds3SuffixArray(unsigned int *startOfArrayPtr, char *buffer, unsigned int currentEnd) {
    unsigned int nMod3Suffixes0 = (currentEnd+2)/3;
    unsigned int nMod3Suffixes1 = (currentEnd+1)/3;
    unsigned int nMod3Suffixes2 = (currentEnd+0)/3;

    const unsigned int rSize = nMod3Suffixes0 + nMod3Suffixes2 +3;

    unsigned int* R = new unsigned int[rSize];
    for (int i = 0; i <= rSize; i++) R[i] = 0;
    R[rSize-3]=0;
    R[rSize-2]=0;

    unsigned int* R0 = new unsigned int[nMod3Suffixes0];
    for (int i = 0; i <= nMod3Suffixes0; i++) R0[i] = 0;


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

    bucketSortPass(R,buffer,2);
    bucketSortPass(R,buffer,1);
    bucketSortPass(R,buffer,0);

    
    //for (int i = 0; i < currentEnd; i++) rankR[i] = -1;
    //rankR[currentEnd] = 0;
    //rankR[currentEnd+1] = 0;

    rankR = new unsigned int[currentEnd+2];
    //for (int i = 0; i < currentEnd+2; i++) rankR[i] = 0;
    
    int j = 1;
    for (size_t i = 0; i < rSize; i++) {
        if (R[i] != 0)
            rankR[R[i]] = j;
        j++;
    }
    
    bucketSortPass(R0,buffer,0,true);

    j = 0;
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
    string fileName = "./example2.txt";

    ifstream input(fileName);
    input.unsetf(ios_base::skipws);

    ofstream output;
    output.open("./example2.bwt");

    unsigned int currentEnd = getFileSize(fileName);
    char* buffer = new char[currentEnd];

    //char tempSuffixArray[] = "./tempSuffixArray.bin";
    //unsigned int* suffixArray = makeMmap<unsigned int>(tempSuffixArray,currentEnd); 
    unsigned int* suffixArray = new unsigned int[currentEnd];

    // for (int i = 0; i < currentEnd; i++) {
    //     cout << suffixArray[i] << " ";
    // }
    //cout <<  " ";

    //char buffer[currentEnd];
    //unsigned int suffixArray[currentEnd];

    

    unsigned int pos = 0;
    while (input.peek() != EOF) {
        input >> buffer[pos];
        suffixArray[pos] = pos;
        pos++;
    }

    input.close();

    ds3SuffixArray(suffixArray, buffer, currentEnd);


    for (int i = 0; i < currentEnd; i++) {
        // subtract 1 from suffix index to get bwt
        if (suffixArray[i] > 0)
            cout << buffer[suffixArray[i]-1];
        else
            cout << buffer[pos-1];
        //cout << suffixArray[i] <<" ";
    }

    delete [] suffixArray; 
    delete [] buffer;

    output.close();

    return 0;

}

