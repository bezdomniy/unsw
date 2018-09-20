#include <iostream>
#include <fstream>
#include <algorithm>
#include <list>
#include <stdint.h>

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

template<typename T>
void bucketSortPass(unsigned int *suffixArray, unsigned int *outArray, T *buffer, unsigned int sortCharIndex, unsigned int alphabetSize) {
    //cout << alphabetSize << endl;
    unsigned int count[alphabetSize];
    unsigned int lengths[alphabetSize];
    
    for (int i=0;i<alphabetSize;i++) {
        count[i]=0;
        lengths[i]=0;
    }

    for (T *inputPtr = buffer; *inputPtr; ++inputPtr) {
        count[inputPtr[0+sortCharIndex]]++; 
    }
    //for (auto i: count) cout << i << " ";

    unsigned int* buckets[alphabetSize];

    for (int i = 0; i < alphabetSize; i++) {
        if (count[i] > 0) {
            buckets[i] = new unsigned int[count[i]];
            //cout << i;
            for (int j = 0; j < count[i]; j++) {
                buckets[i][j]=0;
            }
        }
        else
            buckets[i] = 0;
    }
    //buckets[7] = new unsigned int[count[7]];
    

    //list<unsigned int>::iterator it;
    
    bool firstZeroElement = false;
    if (*suffixArray ==0) {
        (*suffixArray)++;
        firstZeroElement = true;
    }
    // for (unsigned int *inputPtr = suffixArray; *inputPtr; ++inputPtr) {
    //     if (firstZeroElement) {
    //         (*inputPtr)--;
    //         firstZeroElement = false;
    //     }
    //     cout << endl;
    //     cout <<"i:"<< *inputPtr+sortCharIndex<<" ";
    //     cout <<"c:"<< buffer[*inputPtr+sortCharIndex]<<" ";
    //     cout <<"l:"<< lengths[buffer[*inputPtr+sortCharIndex]]<<" ";
    //     cout <<"b:"<<buckets[buffer[*inputPtr+sortCharIndex]][lengths[buffer[*inputPtr+sortCharIndex]]++]<<" ";
    // }
    // cout << endl;
    for (int i=0;i<alphabetSize;i++) {
        lengths[i]=0;
    }

    if (*suffixArray ==0) {
        firstZeroElement = true;
    }
    for (unsigned int *inputPtr = suffixArray; *inputPtr; ++inputPtr) {
        if (firstZeroElement) {
            (*inputPtr)--;
            firstZeroElement = false;
        }

        buckets[buffer[*inputPtr+sortCharIndex]][lengths[buffer[*inputPtr+sortCharIndex]]++] = *inputPtr;
        //it = buckets[int(buffer[*inputPtr+sortCharIndex])].end();
        //buckets[int(buffer[*inputPtr+sortCharIndex])].insert(it, *inputPtr);
    }

    for (int i = 0; i < alphabetSize; i++) {
        for (int j = 0; j < lengths[i]; j++) {
            *outArray = buckets[i][j];
            outArray++;
        }
        if (buckets[i] != 0)
            delete [] buckets[i];
    }
    //cout << sizeof(buckets);
}

template <typename T>
unsigned int renameToRank(unsigned int *a, unsigned int *b, T *buffer, int sizeA, int size0) {
    // Rename suffixes to their ranks
    int rank = 0, c0 = -1, c1 = -1, c2 = -1;
    for (int i = 0; i < sizeA; i++) {
        if (buffer[a[i]] != c0 || buffer[a[i]+1] != c1 || buffer[a[i]+2] != c2) {
            rank++; 
            c0 = buffer[a[i]]; 
            c1 = buffer[a[i]+1]; 
            c2 = buffer[a[i]+2]; 
        }
        if (a[i] % 3 == 1) { 
            b[a[i]/3] = rank; 
        } // write to R1
        else { 
            b[a[i]/3 + size0] = rank; 
        } // write to R2
    }
    return rank;
}

template <typename T>
void ds3SuffixArray(unsigned int *suffixArray, T *buffer, unsigned int currentEnd, unsigned int alphabetSize) {
    unsigned int nMod3Suffixes0 = (currentEnd+2)/3;
    unsigned int nMod3Suffixes1 = (currentEnd+1)/3;
    unsigned int nMod3Suffixes2 = (currentEnd+0)/3;

    const unsigned int rSize = nMod3Suffixes0 + nMod3Suffixes2 +3;

    unsigned int* SA = new unsigned int[rSize];
    unsigned int* R = new unsigned int[rSize];

    for (int i = 0; i <= rSize; i++) R[i] = 0;
    R[rSize-3]=0;
    R[rSize-2]=0;
    R[rSize-1]=0;

    for (int i = 0; i <= rSize; i++) SA[i] = 0;
    SA[rSize-3]=0;
    SA[rSize-2]=0;
    SA[rSize-1]=0;

    unsigned int* R0 = new unsigned int[nMod3Suffixes0];
    for (int i = 0; i <= nMod3Suffixes0; i++) R0[i] = 0;

    //cout << currentEnd <<"\n";
    for (int i = 0, j = 0, k=0; i < currentEnd; i++) {
        if (i % 3 != 0)
            R[j++] = i;
        else
            R0[k++] = i;
    }

    for (int i = 0; i < currentEnd; i++) cout << suffixArray[i] << " ";
    cout << "b12 index"<< endl;
    for (int i = 0; i < currentEnd; i++) cout << buffer[i] << " ";
    cout << "buffer"<< endl;
        for (int i = 0; i < currentEnd; i++) cout << R[i] << " ";
    cout << "B12"<< endl;

    bucketSortPass<T>(R,SA,buffer,2,alphabetSize);
    bucketSortPass<T>(SA,R,buffer,1,alphabetSize);
    bucketSortPass<T>(R,SA,buffer,0,alphabetSize);

    for (int i = 0; i < rSize; i++) cout << SA[i] << " ";
    cout << " sorted" << endl;

    unsigned int rank = renameToRank<T>(SA, R, buffer, rSize-3, nMod3Suffixes0);
    
    
    for (int i = 0; i < rSize; i++) cout << R[i] << " ";
    cout << " ranked" << endl;
    
//    cout << rSize << "\n";
    if (rank < rSize-3) {
        cout << " entering recursion"<< endl;
        ds3SuffixArray<unsigned int>(SA, R, rSize-3, rank+1);
        cout << "exiting recursion"<< endl;
        
        //unsigned int * temp = new unsigned int[currentEnd];
        for (int i = 0, j = 0, k = ((currentEnd-3)/2); i < currentEnd; i++) {
            if (i % 3 == 1) {
                R[j++] = i;
            }
            else if (i % 3 == 2) {
                R[k++] = i;
            }
        }

        //for (int i = 0; i < rSize-3; i++) R[SA[i]] = i +1;

        // *********** figure out better way! - this seems better
        for (int i = 0; i < rSize-3; i++) SA[i] = R[SA[i]];
        for (int i = 0; i < rSize-3; i++) R[SA[i]] = i+1;
        //delete [] temp;

        for (int i = 0; i < rSize; i++) cout << SA[i] << " ";
            cout << " suffix after recurse"<< endl;
        for (int i = 0; i < rSize; i++) cout << R[i] << " ";
            cout << " rank after recurse"<< endl;
    } 
    //else // generate the suffix array of R directly
    //    for (int i = 0; i < rSize-3; i++) SA[R[i] - 1] = i;

    // for (int i = 0; i < rSize; i++) cout << SA[i] << " ";
    //     cout << " suffix array" << endl;
    

    // rankR = new unsigned int[currentEnd+3];
    // for (int i = 0; i < currentEnd; i++) rankR[i] = -1;
    // rankR[currentEnd] = 0;
    // rankR[currentEnd+1] = 0;

    
    //for (int i = 0; i < currentEnd+2; i++) rankR[i] = 0;
    //for (int i=0; i < rSize; i++) cout << SA[i] << " ";

    // int j = 1;
    // for (size_t i = 0; i < rSize; i++) {
    //     if (SA[i] != 0)
    //         rankR[SA[i]] = j;
    //     j++;
    // }


    
    //for (int i=0, j=0; i < rSize-3; i++) 
    //    if (SA[i] < nMod3Suffixes0) R0[j++] = 3*SA[i];
    for (int i = 0; i < nMod3Suffixes0; i++) cout << R0[i] << " ";
    cout << " R0s before sort"<< endl;
    bucketSortPass<T>(R0,R0,buffer,0, alphabetSize);
    // ************ need to get the 2nd stage sort working
    //bucketSortPass<unsigned int>(R0,R0,R,0, alphabetSize);
    //bucketSortPass(R0,R0,R,0);



    for (int i = 0; i < nMod3Suffixes0; i++) cout << R0[i] << " ";
    cout << " R0s"<< endl;
    //bucketSortPass(R0,R0,buffer,0,true);

    // j = 0;
    // for (size_t i = 0; i < nMod3Suffixes0; i++) {
    //     rankR[R0[i]] = j;
    //     j++;
    // }

    for (int i = 0, j = 0; i + j < currentEnd;) {
        //cout << "comparing " << buffer[R[i]] << " and " << buffer[R0[j]] << "\n";
        //cout << "comparing: " << buffer[R[i]] << " and " << buffer[R0[j]] << "\n";
        // cout << SA[i] << " "<< R0[j] <<endl;
        // cout << buffer[SA[i]] << " "<< buffer[R0[j]] <<endl;
        if (j == nMod3Suffixes0) {
            // cout << "inputing0 " << SA[i] << " " << buffer[R[i]] << "\n";
            suffixArray[i+j] = SA[i++];
        }

        else if (buffer[SA[i]] < buffer[R0[j]]) {
            suffixArray[i+j] = SA[i];
            // cout << "inputing1 " << SA[i] << " "<< R0[i]<< " " << buffer[SA[i]] << "\n";
            i++;
        }
            
        else if (buffer[SA[i]] > buffer[R0[j]]) {
            suffixArray[i+j] = R0[j];
            // cout << "inputing2 " << SA[i] << " "<< R0[j]<< " " << buffer[R0[j]] << "\n";
            j++;
        }
            
        else {
            if (R[i] % 3 == 1) {
                // cout << "inputing3 rankbased" << R[i] << " " << buffer[R[i]] << "\n";
                suffixArray[i+j] = R[i] +1 < R0[j] +1 ? R[i++] : R0[j++];
            }
            else {
                if (buffer[SA[i]+1] < buffer[R0[j]+1]) {
                    suffixArray[i+j] = SA[i];
                    // cout << "inputing4 next letters: " << buffer[SA[i]+1] << " and " << buffer[R0[j]+1] << "\n";
                    i++;
                }
                    
                else if (buffer[SA[i]+1] > buffer[R0[j]+1]) {
                    // cout << "inputing5 next letters: " << buffer[SA[i]+1] << " and " << buffer[R0[j]+1] << "\n";
                    suffixArray[i+j] = R0[j];
                    j++;
                }
                else {
                    // cout << "inputing6 rankbased" << R[i] << " " << buffer[R[i]] << "\n";
                    suffixArray[i+j] = R[SA[i]+2]  < R0[j] +2 ? R[i++] : R0[j++];
                }
            }
        }
        //cout << "i " << i << " j " << j << "\n";
    }

    for (int i = 0; i < currentEnd; i++) cout << suffixArray[i] << " ";
    cout << " out"<< endl;
    // for (size_t i = 0; i < currentEnd; i++) 
    //     cout << suffixArray[i] << " ";
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
    uint8_t* buffer = new uint8_t[currentEnd+3];

    //char tempSuffixArray[] = "./tempSuffixArray.bin";
    //unsigned int* suffixArray = makeMmap<unsigned int>(tempSuffixArray,currentEnd); 
    unsigned int* suffixArray = new unsigned int[currentEnd+3];

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
    for (int i=0;i<3;i++) {
        buffer[pos+i] = 0;
        suffixArray[pos+i] = 0;
    }

    input.close();

    ds3SuffixArray<uint8_t>(suffixArray, buffer, currentEnd, VALIDCHARS);


    for (int i = 0; i < currentEnd; i++) {
        // subtract 1 from suffix index to get bwt
        if (suffixArray[i] > 0)
            cout << buffer[suffixArray[i]-1];
        else
            cout << buffer[pos-1];
        cout << suffixArray[i] <<" ";
    }
    cout << endl;

    delete [] suffixArray; 
    delete [] buffer;

    output.close();

    return 0;

}

