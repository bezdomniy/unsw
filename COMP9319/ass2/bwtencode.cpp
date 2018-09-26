#include <iostream>
#include <fstream>
#include <algorithm>
#include <list>
#include <stdint.h>
#include <string.h>


#define VALIDCHARS 126
#define DISK_WRITE_CUTOFF 0
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

template <typename S, typename T>
void mergeSuffixes(const char* suffixArray, S SA, unsigned int* SA0, unsigned int* R, T* buffer, unsigned int currentEnd, unsigned int rSize, unsigned int nMod3Suffixes0, unsigned int nMod3Suffixes1) {
    ofstream out(suffixArray, ofstream::binary);

    for (int i = 0, j = 0; i + j < currentEnd;) {
        if (j == nMod3Suffixes0) {
            out.write((char*)&buffer[SA[i++]],4);
        }
        else if (i == rSize) {
            out.write((char*)&buffer[SA0[j++]],4);
        }
        else if (SA[i] % 3 == 1) {
            if (buffer[SA[i]] < buffer[SA0[j]]) {
                out.write((char*)&buffer[SA[i++]],4);
            } 
            else if (buffer[SA[i]] > buffer[SA0[j]]) {
                out.write((char*)&buffer[SA0[j++]],4);
            }
            else if (R[(SA[i]/3)+nMod3Suffixes1] < R[SA0[j]/3]) {
                out.write((char*)&buffer[SA[i++]],4);
            }
            else {
                out.write((char*)&buffer[SA0[j++]],4);
            }
        }
        else {
            if (buffer[SA[i]] < buffer[SA0[j]]) {
                out.write((char*)&buffer[SA[i++]],4);
            } 
            else if (buffer[SA[i]] > buffer[SA0[j]]) {
                out.write((char*)&buffer[SA0[j++]],4);
            }
            else if (buffer[SA[i]+1] < buffer[SA0[j]+1]) {
                out.write((char*)&buffer[SA[i++]],4);
            }        
            else if (buffer[SA[i]+1] > buffer[SA0[j]+1]) {
                out.write((char*)&buffer[SA0[j++]],4);
            }
            else if (R[(SA[i]/3)+1]  < R[(SA[j]/3)+nMod3Suffixes1+1]) {
                out.write((char*)&buffer[SA[i++]],4);
            }
            else {
                out.write((char*)&buffer[SA0[j++]],4);
            }
        }
    }

    out.close();
}

template <typename S, typename T>
void mergeSuffixesToBWTFile(const char* suffixArray, S SA, unsigned int* SA0, unsigned int* R, T* buffer, unsigned int currentEnd, unsigned int rSize, unsigned int nMod3Suffixes0, unsigned int nMod3Suffixes1) {
    ofstream out(suffixArray);

    bool firstRec = true;
    // fix starting at 1
    for (int i = 0, j = 0; i + j < currentEnd;) {
        if (firstRec) {
            if (j == nMod3Suffixes0 || buffer[SA[i]] < buffer[SA0[j]] || R[(SA[i]/3)+nMod3Suffixes1] < R[SA0[j]/3] || buffer[SA[i]+1] < buffer[SA0[j]+1] || R[(SA[i]/3)+1] < R[(SA[j]/3)+nMod3Suffixes1+1])
                i++;
            else
                j++;

            firstRec = false;
            continue;
        }

        if (j == nMod3Suffixes0) {
            out << buffer[SA[i]-1];
            i++;
        }
        else if (i == rSize) {
            out << (SA0[j] == 0 ? buffer[currentEnd-2] : buffer[SA0[j] - 1]);
            j++;
        }
        else if (SA[i] % 3 == 1) {
            if (buffer[SA[i]] < buffer[SA0[j]]) {
                out << buffer[SA[i]-1];
                i++;
            } 
            else if (buffer[SA[i]] > buffer[SA0[j]]) {
                out << (SA0[j] == 0 ? buffer[currentEnd-2] : buffer[SA0[j] - 1]);
                j++;
            }
            else if (R[(SA[i]/3)+nMod3Suffixes1] < R[SA0[j]/3]) {
                out << buffer[SA[i] - 1];
                i++;
            }
            else {
                out << (SA0[j] == 0 ? buffer[currentEnd-2] : buffer[SA0[j] - 1]);
                j++;
            }
        }
        else {
            if (buffer[SA[i]] < buffer[SA0[j]]) {
                out << buffer[SA[i]-1];
                i++;
            } 
            else if (buffer[SA[i]] > buffer[SA0[j]]) {
                out << (SA0[j] == 0 ? buffer[currentEnd-2] : buffer[SA0[j] - 1]);
                j++;
            }
            else if (buffer[SA[i]+1] < buffer[SA0[j]+1]) {
                out << buffer[SA[i]-1];
                i++;
            }        
            else if (buffer[SA[i]+1] > buffer[SA0[j]+1]) {
                out << (SA0[j] == 0 ? buffer[currentEnd-2] : buffer[SA0[j] - 1]);
                j++;
            }
            else if (R[(SA[i]/3)+1] < R[(SA[j]/3)+nMod3Suffixes1+1]) {
                out << buffer[SA[i] -1];
                i++;
            }
            else {
                out << (SA0[j] == 0 ? buffer[currentEnd-2] : buffer[SA0[j] -1]);
                j++;
            }
        }
    }
    
    out.close();
}

template <typename T>
void dc3SuffixArray(T *buffer, unsigned int currentEnd, unsigned int alphabetSize, unsigned int level=0, const char* outName="") {
    unsigned int nMod3Suffixes0 = (currentEnd+2)/3;
    unsigned int nMod3Suffixes1 = (currentEnd+1)/3;
    unsigned int nMod3Suffixes2 = (currentEnd+0)/3;

    // string levelStr = to_string(level);
    // unsigned int sizePlus = 3;
    // if (level == 0) sizePlus = 1;

    // char bufPrefix[] = "tempBuf";
    // const char *fileBuf = strcat(bufPrefix, levelStr.c_str());
    // FILE *fp = fopen(fileBuf, "rb");
    // fileArray<T> bufferFile(fp,2);
    
    // if (level > DISK_WRITE_CUTOFF) {
    //     serialize<T>(buffer, currentEnd+sizePlus, fileBuf);
    //     delete [] buffer; 
    // }

    // try:
    // only delete buffer in the second half... to save on R0 + SA0 space
    
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

    // if (level > DISK_WRITE_CUTOFF) {
    //     radixPass<fileArray<T>>(R,SA,bufferFile,rSize,alphabetSize);
    //     bufferFile.setOffset(1);
    //     radixPass<fileArray<T>>(SA,R,bufferFile,rSize,alphabetSize);
    //     bufferFile.setOffset(0);
    //     radixPass<fileArray<T>>(R,SA,bufferFile,rSize,alphabetSize);

    //     rank = renameToRank<fileArray<T>>(SA, R, bufferFile, rSize, nMod3Suffixes1);
    // }
    // else {
        radixPass<T*>(R,SA,buffer+2,rSize,alphabetSize);
        radixPass<T*>(SA,R,buffer+1,rSize,alphabetSize);
        radixPass<T*>(R,SA,buffer,rSize,alphabetSize);

        rank = renameToRank<T*>(SA, R, buffer, rSize, nMod3Suffixes1);
    // }

    if (rank < rSize) delete [] SA;

    unsigned int* SA0 = new unsigned int[nMod3Suffixes0];
    unsigned int* R0 = new unsigned int[nMod3Suffixes0];
    for (int i = 0; i < nMod3Suffixes0; i++) SA0[i] = 0;
    for (int i = 0; i < nMod3Suffixes0; i++) R0[i] = 0;

    for (int i = 0, j = 0; i < currentEnd; i++) {
        if (i % 3 == 0) 
            SA0[j++] = i;
    }
        
    radixPass<unsigned int*>(SA0,R0,R,nMod3Suffixes0,rSize,true);
    
    // if (level > DISK_WRITE_CUTOFF)
    //     radixPass<fileArray<T>>(R0,SA0,bufferFile,nMod3Suffixes0,alphabetSize);
    // else
    radixPass<T*>(R0,SA0,buffer,nMod3Suffixes0,alphabetSize);

    delete [] R0;

    // from previous (higher level)
    string levelStrPlus1 = to_string(level+1);
    char saOutPrefix[9] = "tempSA";
    const char *fileOut = strcat(saOutPrefix, levelStrPlus1.c_str());

    FILE *fp = fopen(fileOut, "rb");
    fileArray<unsigned int> SAFile(fp);

    if (rank < rSize) {
        cout << "entering into recursion level "<< level+1 << endl;
        dc3SuffixArray<unsigned int>(R, rSize, rank,level+1);
        cout << "exiting from recursion level "<<level+1<< endl;


        for (int i = 0, j = 0, k = nMod3Suffixes1; i < currentEnd; i++) {
            if (i % 3 == 1) {
                R[j++] = i;
            }
            else if (i % 3 == 2) {
                R[k++] = i;
            }
        }

        //delete [] SA;



        // SA = new unsigned int[rSize + 3];
        // deserialize<unsigned int>(fileOut, rSize + 3, SA);
        

        // for (int i = 0; i < rSize; i++) SA[i] = R[SA[i]];
        
        for (int i = 0; i < rSize; i++) {
            if (SAFile[i] % 3 == 1) { 
                R[SAFile[i]/3] = i+1; 
            } 
            else { 
                R[SAFile[i]/3 + nMod3Suffixes1] = i+1; 
            }
        }
    }  

    string levelStr = to_string(level);
    char saPrefix[9] = "tempSA";
    const char *fileSA = strcat(saPrefix, levelStr.c_str());

    if (level == 0) {
        mergeSuffixesToBWTFile<fileArray<unsigned int>, T>(outName, SAFile, SA0, R, buffer, currentEnd, rSize, nMod3Suffixes0, nMod3Suffixes1);
    }
    // else if (level > DISK_WRITE_CUTOFF) {
    //     mergeSuffixes<fileArray<T>>(suffixArray, SA, SA0, R, bufferFile, currentEnd, rSize, nMod3Suffixes0, nMod3Suffixes1);
    // }
    else {
        // mergeSuffixes<T>(fileSA, SAFile, SA0, R, buffer, currentEnd, rSize, nMod3Suffixes0, nMod3Suffixes1);
        mergeSuffixes<fileArray<unsigned int>, T>(fileSA, SAFile, SA0, R, buffer, currentEnd, rSize, nMod3Suffixes0, nMod3Suffixes1);
    }

    // if (fp!=NULL) fclose(fp);
            
    delete [] R; 
    // delete [] SA; 
    delete [] SA0;

    // if (level > DISK_WRITE_CUTOFF) {
    //     buffer = new T[currentEnd + sizePlus];
    //     deserialize<T>(fileBuf, currentEnd + sizePlus, buffer);
    // }

    //return fileSA;
}

main(int argc, char const *argv[])
{
    string fileName = "./warandpeace.txt";
    const char* outName = (const char*)"./warandpeace.bwt";

    ifstream input(fileName);
    input.unsetf(ios_base::skipws);

    unsigned int currentEnd = getFileSize(fileName);
    uint8_t* buffer = new uint8_t[currentEnd+3];

    
    // unsigned int* suffixArray = new unsigned int[currentEnd+3];
    // ofstream output(outName);

    // unsigned int* suffixArray;

    unsigned int pos = 0;
    while (input.peek() != EOF) {
        input >> buffer[pos];
        //suffixArray[pos] = pos;
        pos++;
    }
    for (int i=0;i<3;i++) {
        buffer[pos+i] = 0;
        //suffixArray[pos+i] = 0;
    }
    //cout << pos << endl;
    //cout << currentEnd+1-1 << endl;

    input.close();

    dc3SuffixArray<uint8_t>(buffer, currentEnd+1, VALIDCHARS, 0, outName);

    // for (int i = 0; i < currentEnd+1; i++) {
    //     cout << suffixArray[i] <<" ";
    // }
    // cout << endl;
    
    // for (int i = 1; i < currentEnd+1; i++) {
    //     // subtract 1 from suffix index to get bwt
    //     if (suffixArray[i] > 0)
    //         output << buffer[suffixArray[i]-1];
    //     else
    //         output << buffer[pos-1];
    // }
    
    // delete [] suffixArray; 
    delete [] buffer;



    return 0;

}

