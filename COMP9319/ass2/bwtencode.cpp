#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <stdlib.h>

#include <forward_list>
using namespace std;

int getFileSize(string fileName) {
    ifstream input(fileName, ifstream::binary | ifstream::ate);
    return input.tellg();
}

#define BUFFERLENGTH 100
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

bool suffixCompare(const unsigned int *i1, const unsigned int *i2) {
    unsigned int endL1 = (unsigned int) (currentEnd - *i1 );
    unsigned int endL2 = (unsigned int) (currentEnd - *i2 );

    int res = memcmp(buffer + *i1,buffer + *i2, endL1 < endL2 ? endL1 : endL2);
    cout << "1st:" << buffer + *i1 << "|2nd:" << buffer + *i2 << "|";
    if ( res == 0 ) {
        printf(" - res: %i\n",(endL1 - endL2) > 0);
        return (endL1 - endL2) > 0;
    }
        
    else {
        printf(" - res: %i\n",res < 0);
        return res < 0 ;
    }

        

};

void bucketSortSuffixArray(unsigned int *startOfArrayPtr) {
    vector<forward_list<unsigned int*>> buckets(126);

    for (unsigned int *inputPtr = startOfArrayPtr; *inputPtr; ++inputPtr) {
        (*inputPtr)--;
        forward_list<unsigned int*>::iterator it = buckets[int(buffer[*inputPtr])].before_begin();
        cout << "inserting: \"" << (buffer[*inputPtr]) << "\"\n";
        buckets[int(buffer[*inputPtr])].insert_after(it, inputPtr);
    }

    //int i = 0;
    //unsigned int * next;
    for (auto bucket: buckets) {
        bucket.sort(suffixCompare);
        //qsort(&bucket[0], currentEnd, sizeof(unsigned int),
       //         (int(*)(const void *, const void *))suffixCompare);
        for (const auto element: bucket) {
            //next = (startOfArrayPtr + i);
            //*next = *element;
            //i++;
            cout << "changing: " << *startOfArrayPtr << " to: " << *element << "\n";
            *startOfArrayPtr = *element;
            
            startOfArrayPtr++;
        }
    }
}




main(int argc, char const *argv[])
{
    string fileName = "./example1.txt";
    //long fileLength = getFileSize(fileName);

    ifstream input(fileName);
    input.unsetf(ios_base::skipws);

    ofstream output;
    output.open("./example1.bwt");

    //char inputString[fileLength];
    //unsigned int rotationIndices[fileLength];

    int count = 1;

    while (input.peek() != EOF) {
        unsigned int pos = 0;
        while (input.peek() != EOF && pos < BUFFERLENGTH) {
            input >> buffer[pos];

            //can't iterate through 0 pointer so starting pos at 1
            rotationIndices[pos] = pos +1;
            pos++;
        }

        if (pos == BUFFERLENGTH) {
            //qsort(&rotationIndices[0], BUFFERLENGTH, sizeof(unsigned int),
            //    (int(*)(const void *, const void *))suffixCompare);

            bucketSortSuffixArray(&rotationIndices[0]);
        }
        else {
            currentEnd = pos;
            rotationIndices.resize(pos);
            bucketSortSuffixArray(&rotationIndices[0]);
            //qsort(&rotationIndices[0], pos, sizeof(unsigned int),
            //    (int(*)(const void *, const void *))suffixCompare);
        }


        for (int i = 0; i < currentEnd; i++) {
            // subtract 1 from suffix index to get bwt
            if (rotationIndices[i] > 0)
                cout << buffer[rotationIndices[i]-1];
            else
                cout << buffer[pos-1];
        }

        cout << count * BUFFERLENGTH << "\n";
        count++;
    }
    output.close();


    cout << "\n";

    return 0;

}

