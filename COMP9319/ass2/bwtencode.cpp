#include <iostream>
#include <fstream>
#include <algorithm>
#include <list>
#include <stdint.h>
#include <string.h>


#define VALIDCHARS 126
#define DISK_WRITE_CUTOFF 3
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
    FILE * fp;
    size_t offset;
    T buffer[sizeof(T)];

    public:
        fileArray<T>();
        fileArray<T>(FILE * filePointer, size_t off = 0);
        ~fileArray<T>();
        void setOffset(size_t off);
        T operator[] (const unsigned int index);
};

template <typename T>
fileArray<T>::fileArray(FILE * filePointer, size_t off) {
    fp = filePointer;
    offset = off;
}

template <typename T>
fileArray<T>::~fileArray() {}

template <typename T>
T fileArray<T>::operator[](const unsigned int index) {
    fseek(fp,index * sizeof(T) + offset * sizeof(T),SEEK_SET);
    fread(&buffer,sizeof(T),1,fp);
    return *buffer;
}

template <typename T>
void fileArray<T>::setOffset(size_t off) {
    offset = off;
}

template<typename T>
static void radixPass(unsigned int* a, unsigned int* b, T r, unsigned int n, unsigned int K, bool byRank = false) { 
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
unsigned int renameToRank(unsigned int *a, unsigned int *b, T buffer, int sizeA, int size0) {
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
void mergeSuffixes(unsigned int* suffixArray, unsigned int* SA, unsigned int* SA0, unsigned int* R, T buffer, unsigned int currentEnd, unsigned int rSize, unsigned int nMod3Suffixes0, unsigned int nMod3Suffixes1) {
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
}

template <typename T>
T* dc3SuffixArray(unsigned int *suffixArray, T *buffer, unsigned int currentEnd, unsigned int alphabetSize, unsigned int level=0) {
    unsigned int nMod3Suffixes0 = (currentEnd+2)/3;
    unsigned int nMod3Suffixes1 = (currentEnd+1)/3;
    unsigned int nMod3Suffixes2 = (currentEnd+0)/3;

    string levelStr = to_string(level);
    unsigned int sizePlus = 3;
    if (level == 0) sizePlus = 1;

    char bufPrefix[] = "tempBuf";
    const char *fileBuf = strcat(bufPrefix, levelStr.c_str());
    FILE *fp = fopen(fileBuf, "rb");
    fileArray<T> bufferFile(fp,2);
    
    if (level > DISK_WRITE_CUTOFF) {
        serialize<T>(buffer, currentEnd+sizePlus, fileBuf);
        delete [] buffer; 
    }

    // try:
    // read R/SA/both into a file straight away and delete the array(s)
    // then rewrite operations to lseek positions from the file
    
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

    unsigned int rank;

    if (level > DISK_WRITE_CUTOFF) {
        radixPass<fileArray<T>>(R,SA,bufferFile,rSize,alphabetSize);
        bufferFile.setOffset(1);
        radixPass<fileArray<T>>(SA,R,bufferFile,rSize,alphabetSize);
        bufferFile.setOffset(0);
        radixPass<fileArray<T>>(R,SA,bufferFile,rSize,alphabetSize);

        rank = renameToRank<fileArray<T>>(SA, R, bufferFile, rSize, nMod3Suffixes1);
    }
    else {
        radixPass<T*>(R,SA,buffer+2,rSize,alphabetSize);
        radixPass<T*>(SA,R,buffer+1,rSize,alphabetSize);
        radixPass<T*>(R,SA,buffer,rSize,alphabetSize);

        rank = renameToRank<T*>(SA, R, buffer, rSize, nMod3Suffixes1);
    }


    if (rank < rSize) {
        cout << "entering recursion level "<< level << endl;
        R = dc3SuffixArray<unsigned int>(SA, R, rSize, rank,level+1);
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
  
    unsigned int* SA0 = new unsigned int[nMod3Suffixes0];
    unsigned int* R0 = new unsigned int[nMod3Suffixes0];
    for (int i = 0; i < nMod3Suffixes0; i++) SA0[i] = 0;
    for (int i = 0; i < nMod3Suffixes0; i++) R0[i] = 0;

    for (int i = 0, j = 0; i < currentEnd; i++) {
        if (i % 3 == 0) 
            SA0[j++] = i;
    }
        

    radixPass<unsigned int*>(SA0,R0,R,nMod3Suffixes0,rSize,true);
    if (level > DISK_WRITE_CUTOFF)
        radixPass<fileArray<T>>(R0,SA0,bufferFile,nMod3Suffixes0,alphabetSize);
    else
        radixPass<T*>(R0,SA0,buffer,nMod3Suffixes0,alphabetSize);
    delete [] R0;

    if (level > DISK_WRITE_CUTOFF)
        mergeSuffixes<fileArray<T>>(suffixArray, SA, SA0, R, bufferFile, currentEnd, rSize, nMod3Suffixes0, nMod3Suffixes1);
    else
        mergeSuffixes<T*>(suffixArray, SA, SA0, R, buffer, currentEnd, rSize, nMod3Suffixes0, nMod3Suffixes1);

    if (level > DISK_WRITE_CUTOFF) {
        buffer = new T[currentEnd + sizePlus];
        deserialize<T>(fileBuf, currentEnd + sizePlus, buffer);
    }

    fclose(fp);
    delete [] R; delete [] SA; delete [] SA0;

    return buffer;
}



main(int argc, char const *argv[])
{
    string fileName = "./warandpeace.txt";

    ifstream input(fileName);
    input.unsetf(ios_base::skipws);

    ofstream output;
    output.open("./warandpeace.bwt");

    unsigned int currentEnd = getFileSize(fileName);
    uint8_t* buffer = new uint8_t[currentEnd+3];

    unsigned int* suffixArray = new unsigned int[currentEnd+3];

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

    buffer = dc3SuffixArray<uint8_t>(suffixArray, buffer, currentEnd+1, VALIDCHARS);

    // for (int i = 0; i < currentEnd+1; i++) {
    //     cout << suffixArray[i] <<" ";
    // }
    // cout << endl;
    
    for (int i = 1; i < currentEnd+1; i++) {
        // subtract 1 from suffix index to get bwt
        if (suffixArray[i] > 0)
            output << buffer[suffixArray[i]-1];
        else
            output << buffer[pos-1];
    }
    
    delete [] suffixArray; 
    delete [] buffer;

    output.close();

    return 0;

}

