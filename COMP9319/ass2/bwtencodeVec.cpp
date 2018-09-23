#include <iostream>
#include <fstream>
#include <algorithm>
#include <list>
#include <stdint.h>
#include <string.h>

using namespace std;

unsigned int getFileSize(string fileName) {
    ifstream input(fileName, ifstream::binary | ifstream::ate);
    return input.tellg();
}

template <typename T>
void serialize(vector<T>* arrayPointer, unsigned int size, const char* fileName) 
{
    FILE *filePointer = fopen(fileName, "wb");
    fwrite(arrayPointer, sizeof(T), size, filePointer);
    fclose(filePointer);
}

template <typename T>
void deserialize(const char* fileName, unsigned int size, vector<T>* arrayPointer, bool deleteFile = false)
{
    FILE *filePointer = fopen(fileName, "rb");
    fread(arrayPointer, sizeof(T), size, filePointer); 
    fclose(filePointer);
    if (deleteFile) remove(fileName);
}

#define VALIDCHARS 126


template<typename T>
static void radixPass(vector<unsigned int>* a, vector<unsigned int>* b, vector<T>* r, unsigned int n, unsigned int K, bool byRank = false)
{ // count occurrences
    // cout << "sorting" << endl;
    // for (int i = 0; i < n; i++) cout << a[i] << " ";
    // cout << "array"<< endl;
    // for (int i = 0; i < n; i++) cout << r[i] << " ";
    // cout << "buffer"<< endl;
    // cout << "buckets: "<<K<< endl;
unsigned int* c = new unsigned int[K + 1]; // counter array
for (int i = 0; i <= K; i++) c[i] = 0; // reset counters
if (byRank)
    for (int i = 0; i < n; i++) c[r->at(i)]++; // count occurrences - this is probably why my one fails
else  
    for (int i = 0; i < n; i++) c[r->at(a->at(i))]++;
for (int i = 0, sum = 0; i <= K; i++) // exclusive prefix sums
{ int t = c[i]; c[i] = sum; sum += t; }
if (byRank)
    for (int i = 0; i < n; i++) {
        //cout << b[c[r[i]]++] << " ";
        b->at(c[r->at(i)]++) = a->at(i); // sort
    }
else
    for (int i = 0; i < n; i++) b->at(c[r->at(a->at(i))]++) = a->at(i); // sort
delete [] c;
}

template <typename T>
unsigned int renameToRank(vector<unsigned int> *a, vector<unsigned int> *b, vector<T> *buffer, int sizeA, int size0) {
    // Rename suffixes to their ranks
    int rank = 0, c0 = -1, c1 = -1, c2 = -1;
    for (int i = 0; i < sizeA; i++) {
        if (buffer->at(a->at(i)) != c0 || buffer->at(a->at(i)+1) != c1 || buffer->at(a->at(i)+2) != c2) {
            rank++; 
            c0 = buffer->at(a->at(i)); 
            c1 = buffer->at(a->at(i)+1); 
            c2 = buffer->at(a->at(i)+2); 
        }
        if (a->at(i) % 3 == 1) { 
            b->at(a->at(i)/3) = rank; 
            // cout << "writing" << rank << " to pos " << a[i]/3 <<endl;
        } // write to R1
        else { 
            b->at(a->at(i)/3 + size0) = rank; 
            // cout << "writing" << rank << " to pos " << a[i]/3 + size0 <<endl;
        } // write to R2
    }
    // cout << endl;
    return rank;
}


template <typename T>
void dc3SuffixArray(vector<unsigned int> *suffixArray, vector<T> *buffer, unsigned int currentEnd, unsigned int alphabetSize, unsigned int level=0) {
    unsigned int nMod3Suffixes0 = (currentEnd+2)/3;
    unsigned int nMod3Suffixes1 = (currentEnd+1)/3;
    unsigned int nMod3Suffixes2 = (currentEnd+0)/3;
    // cout<<currentEnd<<endl;
    // cout<<nMod3Suffixes0<<endl;
    // cout<<nMod3Suffixes1<<endl;
    
    const unsigned int rSize = nMod3Suffixes1 + nMod3Suffixes2;

    vector<unsigned int> SA(rSize+3);
    vector<unsigned int> R(rSize+3);

    

    for (int i = 0; i < rSize+3; i++) R.at(i) = 0;
    // R[rSize-3]=0;
    // R[rSize-2]=0;
    // R[rSize-1]=0;

    for (int i = 0; i < rSize+3; i++) SA.at(i) = 0;
    // SA[rSize-3]=0;
    // SA[rSize-2]=0;
    // SA[rSize-1]=0;
    

    //cout << currentEnd <<"\n";
    for (int i = 0, j = 0; i < currentEnd; i++) {
        if (i % 3 != 0) {
            //R[j++] = i;
            if (i % 3 == 1) { 
                R.at(i/3) = i; 
            } // write to R1
            else { 
                R.at(i/3 + nMod3Suffixes1) = i; 
            }
        }
    }

    radixPass<T>(&R,&SA,buffer+2,rSize,alphabetSize);
    radixPass<T>(&SA,&R,buffer+1,rSize,alphabetSize);
    radixPass<T>(&R,&SA,buffer,rSize,alphabetSize);

    // for (int i = 0; i < rSize; i++) cout << SA[i] << " ";
    // cout << " b12 sorted" << endl;    
    // for (int i = 0; i < rSize; i++) cout << SA[i] << " ";
    // cout << " b12 sorted" << endl;

    unsigned int rank = renameToRank<T>(&SA, &R, buffer, rSize, nMod3Suffixes1);
    
    // for (int i = 0; i < rSize; i++) cout << R[i] << " ";
    // cout << " b12 ranked" << endl;
    //cout << rank<< " " <<rSize-3 << endl;
//    cout << rSize << "\n";
    if (rank < rSize) {
        cout << "entering recursion "<<level<< endl;
        // for (int i = 0; i < rSize; i++) cout << R[i] << " ";
        //     cout << " R12 in" << endl;

        // string levelStr = to_string(level);
        // unsigned int sizePlus = 3;
        // if (level==0) sizePlus=1;
        // char bufPrefix[] = "tempBuf";
        // const char *fileBuf = strcat(bufPrefix, levelStr.c_str());
        // serialize<T>(buffer, currentEnd+sizePlus, fileBuf);

        // char saPrefix[] = "tempSA";
        // const char *fileSA = strcat(saPrefix, levelStr.c_str());
        // serialize<unsigned int>(suffixArray, currentEnd+sizePlus, fileSA);

        // delete [] buffer; 
        // delete [] suffixArray;

        dc3SuffixArray<unsigned int>(&SA, &R, rSize, rank,level+1);
        cout << "exiting recursion level " << level << endl;

        // buffer = new T[currentEnd+sizePlus];
        // deserialize<T>(fileBuf, currentEnd+sizePlus, buffer,true);
        // suffixArray = new unsigned int[currentEnd+sizePlus];
        // deserialize<unsigned int>(fileSA, currentEnd+sizePlus, suffixArray,true);

        for (int i = 0; i < rSize+3; i++) cout << R.at(i) << " ";
        cout << "R" << endl;
        for (int i = 0; i < rSize+3; i++) cout << SA.at(i) << " ";
        cout << "SA" << endl;
        
        for (int i = 0, j = 0, k = nMod3Suffixes1; i < currentEnd; i++) {
            if (i % 3 == 1) {
                R.at(j++) = i;
            }
            else if (i % 3 == 2) {
                R.at(k++) = i;
            }
        }

        for (int i = 0; i < rSize; i++) SA.at(i) = R.at(SA.at(i));
        
        for (int i = 0; i < rSize; i++) {
            if (SA.at(i) % 3 == 1) { 
                R.at(SA.at(i)/3) = i+1; 
            }
            else { 
                R.at(SA.at(i)/3 + nMod3Suffixes1) = i+1; 
            }
        }
    }  
    
   
    vector<unsigned int> SA0(nMod3Suffixes0);
    vector<unsigned int> R0(nMod3Suffixes0);
    for (int i = 0; i < nMod3Suffixes0; i++) SA0.at(i) = 0;
    for (int i = 0; i < nMod3Suffixes0; i++) R0.at(i) = 0;

    for (int i = 0, j = 0; i < currentEnd; i++) {
        if (i % 3 == 0) 
            SA0.at(j++) = i;
    }

    radixPass<unsigned int>(&SA0,&R0,&R,nMod3Suffixes0,rSize,true);
    // for (int i = 0; i < nMod3Suffixes0; i++) cout << R0[i] << " ";
    // cout << " b0 sorted" << endl;
    radixPass<T>(&R0,&SA0,buffer,nMod3Suffixes0,alphabetSize);
    // delete [] R0;


    // cout << endl;
    // for (int i = 0; i < rSize; i++) cout << SA[i] << " ";
    // cout << " SA sorted" << endl;
    // for (int i = 0; i < nMod3Suffixes0; i++) cout << SA0[i] << " ";
    // cout << " SA0 sorted" << endl;

    for (int i = 0, j = 0; i + j < currentEnd;) {
        // cout << "comparing: " << SA[i] << " " << SA0[j] << " " << buffer[SA[i]] <<  buffer[SA[i]+1] <<  buffer[SA[i]+2];
        // cout << " and "<< buffer[SA0[j]]<< buffer[SA0[j]+1]<< buffer[SA0[j]+2]<< " - ";

        if (j == nMod3Suffixes0) {
            suffixArray->at(i+j) = SA.at(i++);
        }
        else if (i == rSize) {
            suffixArray->at(i+j) = SA0.at(j++);
        }
        else if (SA.at(i) % 3 == 1) {
            if (buffer->at(SA.at(i)) < buffer->at(SA0.at(j))) {
                suffixArray->at(i+j) = SA.at(i++);
            } 
            else if (buffer->at(SA.at(i)) > buffer->at(SA0.at(j))) {
                suffixArray->at(i+j) = SA0.at(j++);
            }
            else {
                suffixArray->at(i+j) = R.at((SA.at(i)/3)+nMod3Suffixes1) < R.at(SA0.at(j)/3) ? SA.at(i++) : SA0.at(j++);
            }
        }
        else {
            if (buffer->at(SA.at(i)) < buffer->at(SA0.at(j))) {
                suffixArray->at(i+j) = SA.at(i++);
            } 
            else if (buffer->at(SA.at(i)) > buffer->at(SA0.at(j))) {
                suffixArray->at(i+j) = SA0.at(j++);
            }
            else if (buffer->at(SA.at(i)+1) < buffer->at(SA0.at(j)+1)) {
                suffixArray->at(i+j) = SA.at(i++);
            }        
            else if (buffer->at(SA.at(i)+1) > buffer->at(SA0.at(j)+1)) {
                suffixArray->at(i+j) = SA0.at(j++);
            }
            else {
                // cout << "choosing9 rankbased " << buffer[SA[i]] <<buffer[SA[i]+1]<<buffer[SA[i]+2] << " " << buffer[SA0[j]]<<buffer[SA0[j]+1]<<buffer[SA0[j]+2]<<endl;
                // cout << "ranks: " << R[(SA[i]/3)+1] << " and: " << R[(SA[j]/3)+nMod3Suffixes1+1] << endl;
                //suffixArray[i+j] = R[i+2]  < R[nMod3Suffixes1+j+2] ? SA[i++] : SA0[j++];
                suffixArray->at(i+j) = R.at((SA.at(i)/3)+1)  < R.at((SA.at(j)/3)+nMod3Suffixes1+1) ? SA.at(i++) : SA0.at(j++);
            }
        }
        // cout << endl;
    }

    // delete [] R; delete [] SA; delete [] SA0; //delete [] R0; 
}




main(int argc, char const *argv[])
{
    string fileName = "./example2.txt";

    ifstream input(fileName);
    input.unsetf(ios_base::skipws);

    ofstream output;
    output.open("./example2.bwt");


    unsigned int currentEnd = getFileSize(fileName);
    vector<uint8_t> buffer(currentEnd+3);
    vector<unsigned int> suffixArray(currentEnd+3);

    // for (int i = 0; i < currentEnd; i++) {
    //     cout << suffixArray[i] << " ";
    // }
    //cout <<  " ";

    //char buffer[currentEnd];
    //unsigned int suffixArray[currentEnd];

    
    
    unsigned int pos = 0;
    while (input.peek() != EOF) {
        input >> buffer.at(pos);
        suffixArray.at(pos) = pos;
        pos++;
    }

    for (int i=0;i<3;i++) {
        buffer.at(pos+i) = 0;
        suffixArray.at(pos+i) = 0;
    }

    input.close();

    dc3SuffixArray<uint8_t>(&suffixArray, &buffer, currentEnd+1, VALIDCHARS);

    // for (int i = 0; i < currentEnd+1; i++) {
    //     cout << suffixArray[i] <<" ";
    // }
    // cout << endl;
    
    for (int i = 1; i < currentEnd+1; i++) {
        // subtract 1 from suffix index to get bwt
        if (suffixArray.at(i) > 0)
            output << buffer.at(suffixArray.at(i)-1);
        else
            output << buffer.at(pos-1);
    }
    

    // delete [] suffixArray; 
    // delete [] buffer;

    output.close();

    return 0;

}

