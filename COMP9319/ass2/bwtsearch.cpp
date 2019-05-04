#include <iostream>
#include <fstream>
#include <cstring>
#include <unordered_set>
#include <set>
#include <deque>

using namespace std;

#define NCHARS 127
#define BLOCKSIZE 515
#define INDEXCUTOFF 9700

size_t fileLength;

size_t getFileSize(const char * fileName) {
    ifstream input(fileName, ifstream::binary | ifstream::ate);
    size_t ret = input.tellg();
    input.close();
    return ret;
}


// Returns the occurrence of character ch before index ix in the bwt file
unsigned int getOcc(const char* bwtFileName, const string * indexFileName, char* ch, unsigned int ix) {
    if (ix==0) return 0;
    unsigned int* c = new unsigned int[NCHARS + 1]; 
    unsigned int occ;
    unsigned int offset = ix/BLOCKSIZE;

    // If the file is big enough to warrant indexing - use the index 
    if (fileLength > INDEXCUTOFF) {
        // Index file to speed up calc (so you don't have to count the whole file each time)
        ifstream indexStream (*indexFileName, ofstream::binary);

        // If the index is the end of a block, reduce offset by 1
        if (ix % BLOCKSIZE == 0)
            offset--;

        // Find relevant block in index file
        indexStream.seekg(offset*(NCHARS+1)*sizeof(unsigned int));

        // Read block values of each character into array
        unsigned int buffer;
        for (int i = 0; i < NCHARS + 1; i++) {
            indexStream.read((char *)&buffer,sizeof(unsigned int));
            c[i] = buffer;
        }

        // Initialise occurrence at block value
        occ = c[*ch];
        
        indexStream.close();
    } else {
        occ = 0;
    }
    delete [] c;

    // count backwards from block until the index and decrement the value of occ each time ch is encountered
    // or count forward if the file is too small to have an index
    ifstream bwtStream (bwtFileName, ifstream::ate);
    char charBuffer;
    unsigned int fileEnd = bwtStream.tellg();
    unsigned int countBackFrom = (offset+1)*BLOCKSIZE < fileEnd ? (offset+1)*BLOCKSIZE : fileEnd;

    // If the file is big enough to warrant indexing - use the index - backwards pass from block value to index
    if (fileLength > INDEXCUTOFF) {
        for (int i = countBackFrom-1; i >= ix; i--) {
            bwtStream.seekg(i*sizeof(char));
            bwtStream.get(charBuffer);

            if (charBuffer == *ch) {
                occ--;
            }
        }
    }
    // Otherwise just use the file - pass from start to end
    else {
        for (int i = 0; i < ix; i++) {
            bwtStream.seekg(i);
            bwtStream.get(charBuffer);

            if (charBuffer == *ch) {
                occ++;
            }
        }
    }
    
    bwtStream.close();

    return occ;
}

// Get the count (C) array from the index file
void getCount(unsigned int* c, char delim, const string * indexFileName, const char * bwtFileName) { 

    // If the file is big enough to warrant indexing - use the index
    if (fileLength > INDEXCUTOFF) {
        ifstream inStream (*indexFileName, ofstream::binary);

        unsigned int offset = fileLength/BLOCKSIZE;

        if (fileLength % BLOCKSIZE == 0)
            offset--;

        inStream.seekg(offset*(NCHARS+1)*sizeof(unsigned int));

        unsigned int buffer;

        for (int i = 0; i < NCHARS + 1; i++) {
            inStream.read((char *)&buffer,sizeof(unsigned int));
            c[i] = buffer;
        }

        inStream.close();
    }
    // Otherwise just use the file
    else {
        ifstream inStream (bwtFileName);

        char* readBuffer = new char[fileLength];
        // for (int i = 0; i < fileLength; i++) {
            // cout << fileLength <<endl;
        inStream.read(readBuffer,fileLength);

        // for (int i = 0; i < fileLength; i++) cout << readBuffer[i] << " ";
        // cout << endl;
        for (int j = 0; j < fileLength; j++) c[readBuffer[j]]++;
        // }
        inStream.close();
        delete [] readBuffer;
    }
    
    // Get sums of consecutive character counts for final C array
    c[0] = c[(int)delim];
    for (int i = 1, sum = c[0]; i <= NCHARS; i++) {
        if (i != (int)delim) {
            int t = c[i]; 
            c[i] = sum; 
            sum += t; 
        } else {
            c[i] = sum; 
            sum += 0;
        }
    }
        
}

// Build the index file in blocks
void computeOccurrences(const char * bwtFileName, const string * indexFileName) {
    char* readBuffer = new char[BLOCKSIZE];
    unsigned int* c = new unsigned int[NCHARS + 1]; 
    for (int i = 0; i <= NCHARS; i++) c[i] = 0; 

    ofstream outStream(*indexFileName, ofstream::binary);
    ifstream inStream (bwtFileName);

    for (int i = 0; i < fileLength; i+=BLOCKSIZE) {
        inStream.read(readBuffer,BLOCKSIZE);

        for (int j = 0; j < inStream.gcount(); j++) c[readBuffer[j]]++;
        outStream.write((char*)c, (NCHARS + 1)*sizeof(unsigned int));
    }
    

    inStream.close();
    outStream.close();

    delete [] readBuffer;
    delete [] c;
}

// Retrieve the record numbers for each of the instance between first and last.
// If -i is specificed take the record numbers as input, and print the corresponding record text
template <typename T>
T getRecords(pair<unsigned int, unsigned int> firstLast, unsigned int* count, char delim, const char * bwtFileName, const string * indexFileName, const string * auxFileName, string* option) {
    T result;

    ifstream bwtStream (bwtFileName);
    ifstream auxStream (*auxFileName);

    char charBuffer;
    unsigned int nextIndex;

    char recordBuffer;
    unsigned int delimsInFile = getOcc(bwtFileName, indexFileName, &delim, fileLength);

    // initialise deque for use in -i
    deque<char> outRecord;
        
    for (int i = firstLast.first; i <= firstLast.second; i++) {
        bwtStream.seekg(i);
        bwtStream.get(charBuffer);

        // If the first char isn't the delim and option is not -i
        if (charBuffer != delim && *option != "-i") {
            nextIndex = count[charBuffer] + getOcc(bwtFileName, indexFileName, &charBuffer, i);
        }
        // Otherwise, allow index to be delim 
        else {
            if (i != getOcc(bwtFileName, indexFileName, &delim, fileLength))
                nextIndex = i;
            else
                nextIndex = 0; 
        }
 
        // Push to the front of the deque to print later
        if (*option == "-i")
            outRecord.push_front(charBuffer);

        // backward search step until the previous delimiter is reached
        while (charBuffer != delim) {
            bwtStream.seekg(nextIndex);
            bwtStream.get(charBuffer);

            // Push to the front of the deque to print later
            if (*option == "-i")
                outRecord.push_front(charBuffer);

            if (charBuffer != delim) {
                nextIndex = count[charBuffer] + getOcc(bwtFileName, indexFileName, &charBuffer, nextIndex);
            }
        }

        // If we are using -i, just pop the deque to print out this record and don't worry about the rest of this loop step
        if (*option == "-i") {
            char out;
            outRecord.pop_front();
            while (outRecord.size() > 1) {
                out = outRecord.front();
                cout << out;
                outRecord.pop_front();
            }
            outRecord.pop_front();
            cout << endl;
            continue;
        }

        // Otherwise, look up the aux file to get the record that the pattern belongs to
        unsigned int pos = getOcc(bwtFileName, indexFileName, &delim, nextIndex);
        auxStream.seekg(pos*sizeof(unsigned int));
        auxStream.read((char *)&pos,sizeof(unsigned int));
        pos ++;
        result.insert(pos);
    }

    bwtStream.close();
    auxStream.close();

    return result;
}

// Driver function to do backward search with options given (see readme for details on how each option works)
template <typename T>
void backwardSearch(const char * pattern, char delim, const char * bwtFileName, const string * indexFileName, const string * auxFileName, string* option) {
    unsigned int i = strlen(pattern) - 1;
    char ch = pattern[i];

    unsigned int* C = new unsigned int[NCHARS + 1]; 
    for (int i = 0; i <= NCHARS; i++) C[i]=0;
    
    getCount(C, delim, indexFileName, bwtFileName);

    // for (int i = 0; i < NCHARS+1; i++) cout << C[i] << " ";
    // cout << endl;

    unsigned int first = C[ch];
    unsigned int last;

    for (int j = 0; j < NCHARS+1; j++)
        if (C[ch+j] != C[ch]) {
            last = C[ch+j] -1;
            break;
        }

    pair<unsigned int, unsigned int> ret{0,-1};

    if (*option == "-m" || *option == "-n" || *option == "-a") {
        while (first <= last && i >= 1) {
            ch = pattern[--i];

            first = C[ch] + getOcc(bwtFileName, indexFileName, &ch, first);
            last = C[ch] + getOcc(bwtFileName, indexFileName, &ch, last+1) -1;
        }
        
        if (last < first) {
            return ;
        }

        ret.first = first;
        ret.second = last;

    }

    if (*option == "-m") {
        cout << (ret.second -ret.first+1) << endl;
    }
    else if (*option == "-n" || *option == "-a") {
        T recs = getRecords<T>(ret, C, delim, bwtFileName, indexFileName, auxFileName, option);

        if (*option == "-n") {
            cout << recs.size() << endl;
            return ;
        }
        else {
            typename T::iterator iter = recs.begin();
            while(iter != recs.end()) {
                cout << (*iter) << endl;
                iter++;
            } 
            return ;
        }
    }
    else if (*option == "-i") {
        char patternArray[strlen(pattern)];
        strcpy(patternArray,pattern);

        char * t =  strtok(patternArray," ");
        ret.first = atoi(t) ;
        t =  strtok(NULL," ");
        ret.second = atoi(t) ;
        T r = getRecords<T>(ret, C, delim, bwtFileName, indexFileName, auxFileName, option);
    }

    delete [] C;
    return ;

}



main(int argc, char const *argv[])
{
    string delimIn = argv[1];
    char delim = *argv[1];

    // Quick and dirty way to parse the new line character (included a tab just because)
    if (delimIn.at(0) == '\\') {
        if (delimIn.at(1) == 'n')
            delim = '\n';
        else if (delimIn.at(1) == 't')
             delim = '\t';
    }

    const char* fileName = argv[2];
    const char* tempFolder = argv[3];
    string option = argv[4];
    const char* searchTerm = argv[5];

    fileLength = getFileSize(fileName);

    const string indexFileName = string(tempFolder) + "/file.ix";
    const string auxFileName = string(fileName).substr(0, strlen(fileName)-4) + ".aux";
    
    // make index file if the file is big enough to warrant indexing
    if (fileLength > INDEXCUTOFF)
        computeOccurrences(fileName, &indexFileName);

    // do search - with unordered_set if ordered one is not needed
    if (option == "-a" || option == "-i")
        backwardSearch<set<unsigned int>>(searchTerm, delim, fileName, &indexFileName, &auxFileName, &option);
    else
        backwardSearch<unordered_set<unsigned int>>(searchTerm, delim, fileName, &indexFileName, &auxFileName, &option);

    return 0;
}
