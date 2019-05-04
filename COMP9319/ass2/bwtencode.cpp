#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>

using namespace std;

int getFileSize(string fileName) {
    ifstream input(fileName, ios::ate | ios::binary );
    return input.tellg();
}

int currentEnd = 0;
unsigned int* buffer;
char delim;

// Comparator function for use with std::sort
bool suffixCompare(const unsigned int i1, const unsigned int i2) {
    unsigned int endL1 = (unsigned int) (currentEnd - i1 )*sizeof(unsigned int);
    unsigned int endL2 = (unsigned int) (currentEnd - i2 )*sizeof(unsigned int);

    // Compare subsequent elements until a difference in integer is found
    int res = 0;
    for (int i = 0; i < endL1 < endL2 ? endL1 : endL2; i++) {
        if (*(buffer + i1 + i) != *(buffer + i2 + i)) {
            res = *(buffer + i1 + i) - *(buffer + i2 + i);
            break;
        }
    }

    // If the end of 1 suffix is reached, favour the shorter one
    if ( res == 0 ) {
        return (endL1 - endL2) > 0;
    }
        
    else {
        return res < 0 ;
    }
};



main(int argc, char const *argv[])
{
    string delimIn = argv[1];
    delim = *argv[1];

    // Quick and dirty way to parse the new line character (included a tab just because)
    if (delimIn.at(0) == '\\') {
        if (delimIn.at(1) == 'n')
            delim = '\n';
        else if (delimIn.at(1) == 't')
            delim = '\t';
    }

    const char* tempFolder = argv[2];
    const char* fileName = argv[3];
    const char* bwtFileName = argv[4];

    const string auxPath = string(fileName).substr(0, strlen(fileName)-4) + ".aux";
    unsigned int fileLength = getFileSize(fileName);

    buffer = new unsigned int[fileLength+1];
    unsigned int* rotationIndices = new unsigned int[fileLength+1];

    buffer[fileLength] = 0;
    rotationIndices[fileLength] = fileLength;

    // Original text file input stream
    ifstream input(fileName);
    input.unsetf(ios_base::skipws);

    // bwt file output stream
    ofstream output;
    output.open(bwtFileName);

    // Assume full 127 chars, didn't have time to reduce to only the number needed
    const short int nChars = 127;
    const unsigned int CHAROFFSET = (fileLength/4);

    // position in the input
    unsigned int pos = 0;

    // Which number delimiter we are up to - starting at 1 to allow 0 to be the last one
    unsigned int delimPos = 1;

    char buf;
    while (input.peek() != EOF) {
        input.read((char *)&buf,sizeof(char));
        buffer[pos] = (int)buf;

        // If its a delimiter, output a unique value for it
        if (buffer[pos] == (int)delim) {
            if (pos != fileLength-1) {
                buffer[pos] = delimPos;
            } 
            else {
                buffer[pos] = 0;
            }
            delimPos++;
        }
        // Otherwise, output the ascii value of the character plus the offset (see readme for details)
        else {
            buffer[pos] = buffer[pos] + CHAROFFSET;
        }

        rotationIndices[pos] = pos ;
        pos++;
    }

    currentEnd = fileLength;

    // Sort the suffix array using custom comparator
    sort(rotationIndices,rotationIndices+fileLength, suffixCompare);

    // Output stream for aux file.
    ofstream auxOutput(auxPath, ios::binary);

    unsigned int auxPos = 0;

    // Write suffix array minus 1 to file, wrapping around at 0 (see readme for details)
    for (int i = 0; i < currentEnd; i++) {
        if (rotationIndices[i] > 0) {
            if (buffer[rotationIndices[i]-1] < CHAROFFSET) {
                auxPos = buffer[rotationIndices[i]-1];
                auxOutput.write(reinterpret_cast<const char *>(&auxPos), sizeof(pos));
                output << delim;
            }
            else {
                output << (char)(buffer[rotationIndices[i]-1] - CHAROFFSET);
            }
        }
        else {
            if (buffer[pos-1] < CHAROFFSET) {
                auxPos = buffer[pos-1];
                auxOutput.write(reinterpret_cast<const char *>(&auxPos), sizeof(pos));
                output << delim;
            }
            else {
                output << (char)(buffer[pos-1]-CHAROFFSET);
            }
        }

    }

    delete [] buffer;
    delete [] rotationIndices;
    output.close();
    auxOutput.close();


    return 0;

}
