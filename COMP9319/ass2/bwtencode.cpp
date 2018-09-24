#include <iostream>
#include <fstream>
#include <algorithm>
#include <list>
#include <stdint.h>
#include <string.h>


#define VALIDCHARS 126
using namespace std;

unsigned int getFileSize(string fileName) {
    ifstream input(fileName, ifstream::binary | ifstream::ate);
    return input.tellg();
}

template <typename T>
void serialize(T* arrayPointer, unsigned int size, const char* fileName) 
{
    FILE *filePointer = fopen(fileName, "wb");
    fwrite(arrayPointer, sizeof(T), size, filePointer);
    fclose(filePointer);
}

template <typename T>
void deserialize(const char* fileName, unsigned int size, T* arrayPointer)
{
    FILE *filePointer = fopen(fileName, "rb");
    fread(arrayPointer, sizeof(T), size, filePointer); 
    fclose(filePointer);
}

template <typename T>
class fileArray {
    const char * filePath;
    T buffer;

    public:
        fileArray(const char * fileName);
        T operator[] (const unsigned int index);
};

template <typename T>
fileArray<T>::fileArray(const char * path) {
    filePath = path;
    T buffer = 0;
}

template <typename T>
T fileArray<T>::operator[](const unsigned int index) {
    FILE *fp = fopen(filePath, "rb");
    fseek(fp,index*sizeof(T),SEEK_SET);
    fread(buffer,sizeof(T),1,fp);
    fclose(fp);

    return buffer;
}


template<typename T>
static void radixPass(unsigned int* a, unsigned int* b, T* r, unsigned int n, unsigned int K, bool byRank = false) { 
    unsigned int* c = new unsigned int[K + 1]; // counter array
    for (int i = 0; i <= K; i++) c[i] = 0; // reset counters
    if (byRank)
        for (int i = 0; i < n; i++) c[r[i]]++; // count occurrences 
    else  
        for (int i = 0; i < n; i++) c[r[a[i]]]++;
    for (int i = 0, sum = 0; i <= K; i++) {
        int t = c[i]; c[i] = sum; sum += t; 
    }
    if (byRank)
        for (int i = 0; i < n; i++) b[c[r[i]]++] = a[i]; 
    else
        for (int i = 0; i < n; i++) b[c[r[a[i]]]++] = a[i]; 
    delete [] c;
}

template <typename T>
unsigned int renameToRank(unsigned int *a, unsigned int *b, T *buffer, int sizeA, int size0) {
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
        }
        else { 
            b[a[i]/3 + size0] = rank; 
        }
    }
    return rank;
}


template <typename T>
void dc3SuffixArray(unsigned int *suffixArray, T *buffer, unsigned int currentEnd, unsigned int alphabetSize, unsigned int level=0) {
    unsigned int nMod3Suffixes0 = (currentEnd+2)/3;
    unsigned int nMod3Suffixes1 = (currentEnd+1)/3;
    unsigned int nMod3Suffixes2 = (currentEnd+0)/3;

    // read buffer into a file straight away and delete the array
    // then rewrite operations to lseek positions from the file
    // re-read the file back after SA,R have been deleted in the end
    // make sure to return the address for the new versions of the vars

    const unsigned int rSize = nMod3Suffixes1 + nMod3Suffixes2;

    unsigned int* SA = new unsigned int[rSize+3];
    unsigned int* R = new unsigned int[rSize+3];

    for (int i = 0; i < rSize+3; i++) R[i] = 0;
    // R[rSize-3]=0;
    // R[rSize-2]=0;
    // R[rSize-1]=0;

    for (int i = 0; i < rSize+3; i++) SA[i] = 0;
    // SA[rSize-3]=0;
    // SA[rSize-2]=0;
    // SA[rSize-1]=0;

    //cout << currentEnd <<"\n";
    for (int i = 0, j = 0; i < currentEnd; i++) {
        if (i % 3 != 0) {
            if (i % 3 == 1) { 
                R[i/3] = i; 
            } 
            else { 
                R[i/3 + nMod3Suffixes1] = i; 
            }
        }
    }

    radixPass<T>(R,SA,buffer+2,rSize,alphabetSize);
    radixPass<T>(SA,R,buffer+1,rSize,alphabetSize);
    radixPass<T>(R,SA,buffer,rSize,alphabetSize);

    unsigned int rank = renameToRank<T>(SA, R, buffer, rSize, nMod3Suffixes1);

    if (rank < rSize) {
    // if (false) {
        cout << "entering recursion level "<< level << endl;
        dc3SuffixArray<unsigned int>(SA, R, rSize, rank,level+1);
        cout << "exiting recursion level "<<level<< endl;

        for (int i = 0, j = 0, k = nMod3Suffixes1; i < currentEnd; i++) {
            if (i % 3 == 1) {
                R[j++] = i;
            }
            else if (i % 3 == 2) {
                R[k++] = i;
            }
        }

        for (int i = 0; i < rSize; i++) SA[i] = R[SA[i]];
        
        for (int i = 0; i < rSize; i++) {
            if (SA[i] % 3 == 1) { 
                R[SA[i]/3] = i+1; 
            } 
            else { 
                R[SA[i]/3 + nMod3Suffixes1] = i+1; 
            }
        }
    }  

    string levelStr = to_string(level);

    char rPrefix[] = "tempR";
    const char *fileR = strcat(rPrefix, levelStr.c_str());
    serialize<unsigned int>(R, rSize+3, fileR);
    // delete [] R; 

    // char saPrefix[] = "tempSA";
    // const char *fileSA = strcat(saPrefix, levelStr.c_str());
    // serialize<unsigned int>(SA, rSize+3, fileSA);
    // delete [] SA;
    
    unsigned int* SA0 = new unsigned int[nMod3Suffixes0];
    unsigned int* R0 = new unsigned int[nMod3Suffixes0];
    for (int i = 0; i < nMod3Suffixes0; i++) SA0[i] = 0;
    for (int i = 0; i < nMod3Suffixes0; i++) R0[i] = 0;

    for (int i = 0, j = 0; i < currentEnd; i++) {
        if (i % 3 == 0) 
            SA0[j++] = i;
    }
    
    // R = new unsigned int[rSize+3];
    // deserialize<unsigned int>(fileR, rSize+3, R);
    
    radixPass<unsigned int>(SA0,R0,R,nMod3Suffixes0,rSize,true);
    radixPass<T>(R0,SA0,buffer,nMod3Suffixes0,alphabetSize);
    delete [] R0;

    // SA = new unsigned int[rSize+3];
    // deserialize<unsigned int>(fileSA, rSize+3, SA);

    for (int i = 0, j = 0; i + j < currentEnd;) {
        if (j == nMod3Suffixes0) {
            suffixArray[i+j] = SA[i++];
        }
        else if (i == rSize) {
            suffixArray[i+j] = SA0[j++];
        }
        else if (SA[i] % 3 == 1) {
            if (buffer[SA[i]] < buffer[SA0[j]]) {
                suffixArray[i+j] = SA[i++];
            } 
            else if (buffer[SA[i]] > buffer[SA0[j]]) {
                suffixArray[i+j] = SA0[j++];
            }
            else {
                suffixArray[i+j] = R[(SA[i]/3)+nMod3Suffixes1] < R[SA0[j]/3] ? SA[i++] : SA0[j++];
            }
        }
        else {
            if (buffer[SA[i]] < buffer[SA0[j]]) {
                suffixArray[i+j] = SA[i++];
            } 
            else if (buffer[SA[i]] > buffer[SA0[j]]) {
                suffixArray[i+j] = SA0[j++];
            }
            else if (buffer[SA[i]+1] < buffer[SA0[j]+1]) {
                suffixArray[i+j] = SA[i++];
            }        
            else if (buffer[SA[i]+1] > buffer[SA0[j]+1]) {
                suffixArray[i+j] = SA0[j++];
            }
            else {
                suffixArray[i+j] = R[(SA[i]/3)+1]  < R[(SA[j]/3)+nMod3Suffixes1+1] ? SA[i++] : SA0[j++];
            }
        }
    }
    delete [] R; delete [] SA; delete [] SA0;
}



main(int argc, char const *argv[])
{
    string fileName = "./warandpeace.txt";

    ifstream input(fileName);
    input.unsetf(ios_base::skipws);

    ofstream output;
    output.open("./warandpeace.bwt");

    // unsigned int currentEnd = getFileSize(fileName);
    // uint8_t* buffer = new uint8_t[currentEnd+3];

    // unsigned int* suffixArray = new unsigned int[currentEnd+3];

    // unsigned int pos = 0;
    // while (input.peek() != EOF) {
    //     input >> buffer[pos];
    //     suffixArray[pos] = pos;
    //     pos++;
    // }
    // for (int i=0;i<3;i++) {
    //     buffer[pos+i] = 0;
    //     suffixArray[pos+i] = 0;
    // }

    // input.close();

    // dc3SuffixArray<uint8_t>(suffixArray, buffer, currentEnd+1, VALIDCHARS);

    // // for (int i = 0; i < currentEnd+1; i++) {
    // //     cout << suffixArray[i] <<" ";
    // // }
    // // cout << endl;
    
    // for (int i = 1; i < currentEnd+1; i++) {
    //     // subtract 1 from suffix index to get bwt
    //     if (suffixArray[i] > 0)
    //         output << buffer[suffixArray[i]-1];
    //     else
    //         output << buffer[pos-1];
    // }
    
    // delete [] suffixArray; 
    // delete [] buffer;

    output.close();

    const char *str = "./testR0";
    fileArray<unsigned int> test(str);

    cout << test[1] << endl;

    return 0;

}

