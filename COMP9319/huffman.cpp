//#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <vector>
#include <queue>
#include <fstream>
//#include <iterator>
//#include <cstring>
//#include <list>
#include <ctime>

#define BUFFERLENGTH 1

typedef std::pair<unsigned char, int> HuffmanPair;

class Node {
    public:
        Node(HuffmanPair value, Node *left, Node *right);
        Node(HuffmanPair value);
        Node();
        Node *getLeftChild() const;
        Node *getRightChild() const;

        bool operator== (const Node &other) const
            {
                return this->value.first == other.value.first;
            }
            
        HuffmanPair getValue() const {
            return this->value;
        }
        void updateFrequency(int newFreq) {
            this->value.second = newFreq;
        }

    private:
        HuffmanPair value;
        Node *leftChild = NULL;
        Node *rightChild = NULL;
};
Node::Node(HuffmanPair v, Node *left, Node *right) {
    value = v;
    leftChild = left;
    rightChild = right;
};
Node::Node(HuffmanPair v) {
    value = v;
}
Node::Node(void) {
}

Node *Node::getLeftChild() const {
    return leftChild;
}
Node *Node::getRightChild() const {
    return rightChild;
}

typedef std::pair<Node *, std::vector<bool>> CodePair;

struct CompareValue
{
    bool operator()(const Node* node1, const Node* node2) const
    {

        if (node1->getValue().second != node2->getValue().second)
            return node1->getValue().second > node2->getValue().second;
        else {
/*             if (node1->getValue().first != 0x00) {
                std::cout << node1->getValue().second << ", " << node2->getValue().second << " ";
                printf("0x%x | ",node1->getValue().first);
                printf("0x%x\n",node2->getValue().first); 
            }  */

            return node1->getValue().first < node2->getValue().first;
        } 
            
    }
};

std::unordered_map<unsigned char, std::vector<bool>> encode(Node* root) {
    std::queue<CodePair> nodeStack;

    std::vector<bool> v;
    nodeStack.push(CodePair(root,v));

    std::vector<bool> leftv, rightv;
    CodePair current, left, right;

    std::unordered_map<unsigned char, std::vector<bool>> out;

    while (!nodeStack.empty()) {
        current = nodeStack.front();
        nodeStack.pop();

        if (current.first->getLeftChild() != NULL) {
            leftv = current.second;
            leftv.push_back(0);
            left = CodePair(current.first->getLeftChild(),leftv);

            rightv = current.second;
            rightv.push_back(1);
            right = CodePair(current.first->getRightChild(),rightv);

            nodeStack.push(left);
            nodeStack.push(right);
        }
        else {
            out[current.first->getValue().first] = current.second;
        }
    }

    return out;

} 

void print_tree(Node *t) {
    std::deque<std::pair<Node *, int>> q;

    q.push_back(std::pair<Node *, int>(t, 0));
    int curlevel = -1;
    while (!q.empty()) {
        Node *parent = q.front().first;
        int level = q.front().second;
        q.pop_front();
        if (curlevel != level) {
            curlevel = level;
            std::cout << "Level " << curlevel << std::endl;
        }
        std::cout << parent->getValue().second << " ";

        printf("0x%x",parent->getValue().first);
        
        std::cout << std::endl;

        //printf("%i %i\n", parent->getValue().second ,parent->getValue().first);
        if (parent->getLeftChild())
            q.push_back(std::pair<Node *, int>(parent->getLeftChild(), level + 1));
        if (parent->getLeftChild())
            q.push_back(std::pair<Node *, int>(parent->getRightChild(), level + 1));
    }
}


// reference: https://stackoverflow.com/questions/8461126/how-to-create-a-byte-out-of-8-bool-values-and-vice-versa
std::deque<bool> from_Byte(unsigned char c, int validBitsInLastByte = 0)
{
    std::deque<bool> b;
    if (validBitsInLastByte > 0) {
        for (int i=0; i < validBitsInLastByte; ++i) 
            b.push_back((c & (1<<i)) != 0);  
        for (int i=0; i < validBitsInLastByte; ++i)
            std::cout << ((c & (1<<i)) != 0);
        std::cout << "\n";
    }
    else {
        for (int i=0; i < 8; ++i) 
            b.push_back((c & (1<<i)) != 0);  
        for (int i=0; i < 8; ++i)
            std::cout << ((c & (1<<i)) != 0);
        std::cout << "\n";
    }

/*     for (int i=0; i < 8; ++i)
        std::cout << ((c & (1<<i)) != 0);
    std::cout << "\n"; */
      
    return b;
}

unsigned char to_Byte(std::queue<bool>& b)
{
    unsigned char c = 0;
    for (int i=0; i < 8; ++i) {
        if (b.front()) 
            c |= 1 << i;
        b.pop();
    }
    return c;
}

std::map<unsigned char, int> make_frequency_table(const char * filePath) {
    FILE *filePointer = fopen(filePath, "rb");
    std::map<unsigned char, int> frequency_table;

    if (filePointer != NULL)  {
        unsigned char buffer[BUFFERLENGTH+1];
        size_t charactersRead = 0;

        std::ifstream input( filePath, std::ifstream::binary );
        input.unsetf(std::ios_base::skipws);

        //unsigned char character ;
        while (charactersRead = fread(buffer, sizeof(unsigned char), BUFFERLENGTH, filePointer) > 0) {
        //while (input) {
                unsigned char character = *buffer;
                //input >> character;
            //for (unsigned char character: buffer) {
                std::map<unsigned char, int>::iterator charInSet = frequency_table.find(character);

                if (charInSet == frequency_table.end()) {
                    frequency_table.insert(std::pair<unsigned char, int>(character,0));
                }
                frequency_table[character] ++;
            //}
         

            
        }
        return frequency_table;
    }
    std::fclose(filePointer);
}

Node* make_tree(std::map<unsigned char, int> frequency_table) {
    std::priority_queue<Node*, std::vector<Node*>, CompareValue> forestPq;

    for (std::map<unsigned char, int>::iterator frequency_iterator = frequency_table.begin();
        frequency_iterator != frequency_table.end(); ++frequency_iterator) {
            if (frequency_iterator->second != 0) {
                HuffmanPair newVal(*frequency_iterator);
                Node *newNode = new Node(newVal);
                forestPq.push(newNode);
            }
        }

    while (forestPq.size() > 1) {
        Node* min1 = forestPq.top();
        forestPq.pop();
        Node* min2 = forestPq.top();
        forestPq.pop();

        HuffmanPair newVal(NULL, min1->getValue().second + min2->getValue().second);
        Node *newNode = new Node(newVal,min2,min1);
        forestPq.push(newNode);
    }

    return forestPq.top();
}

std::pair<std::map<unsigned char, int>, int> read_table_from_file(const char * path) {
    std::map<unsigned char, int> out;
    int validBitsInLastByte = 0;

    std::ifstream input( path, std::ifstream::binary );
    input.unsetf(std::ios_base::skipws);
    std::vector<unsigned char> buf(1024);
    input.read((char*)&buf[0],1024);

    bool newChar = true;
    unsigned char* current_char = NULL;
    int bytePos = 0;
    unsigned char byteBuffer[4];

    for (unsigned char &i: buf) {
        if (newChar) {
            if (out.find(i) != out.end()) {
                break;
            }
            //printf("\n0x%x", i);
            current_char = &i;
            out[i] = 0;

            newChar = false;
        } 
        else {
            byteBuffer[bytePos] = i;
            bytePos++;

            if (bytePos == 4) {
                int number = (int)(byteBuffer[3] << 24 | byteBuffer[2] << 16 | byteBuffer[1] << 8 | byteBuffer[0]);
                bytePos = 0;

                if (number == 0) {
                    if (out[*current_char] == 0)
                        out.erase(*current_char);
                    break;
                }
                //printf(" number: %i", number);
                out[*current_char] = number;
                //std::cout << number << "|\n";
                newChar = true;
            }
        }
    }
    validBitsInLastByte =  (int)buf.back();
    std::cout << validBitsInLastByte <<"\n";

    return std::pair<std::map<unsigned char, int>, int>(out,validBitsInLastByte);
}

std::string read_data_from_file(const char * path, Node* root, int validBitsInLastByte) {
    std::string out;

    std::ifstream input(path, std::ifstream::binary );
    input.unsetf(std::ios_base::skipws);
    input.seekg(1024);

    unsigned char byteBuffer;
    std::deque<bool> bitBuffer;
    Node* current = root;

    input >> byteBuffer;

    while (input) {
        

        if (input.peek() == EOF)
            bitBuffer = from_Byte(byteBuffer, validBitsInLastByte);
        else
            bitBuffer = from_Byte(byteBuffer);
        input >> byteBuffer;

        while (!bitBuffer.empty()) {
            if ((current->getLeftChild() == NULL)) {
                out += current->getValue().first;
                current = root;
            }
            else if (bitBuffer.front()) {
                current = current->getRightChild();
                bitBuffer.pop_front();
            }
                
            else {
                current = current->getLeftChild();
                bitBuffer.pop_front();
            }
        }
    }
    
    input.close();

    return out;
}

void write_to_file(const char * inPath, std::map<unsigned char, int> frequency_table, std::unordered_map<unsigned char, std::vector<bool>> codes, const char * outPath) {
    std::ofstream outFile(outPath, std::ofstream::binary);
    outFile.unsetf(std::ios_base::skipws);

    // writing frequency table
    int dataSize = 0;
    for (auto &freq: frequency_table) {
        outFile.write((char*)&freq.first,sizeof(freq.first));
        outFile.write((char*)&freq.second,sizeof(freq.second));

        dataSize += 5;
    }
    // pad the rest of the bits
    for (int i = 0; i < (1024-dataSize); i++) {
        outFile.write("\x00",1);
    }

        
    // writing data
    unsigned char byteBuffer;
    std::queue<bool> bitBuffer;
    unsigned char writeBytes;
    std::vector<bool> code;
    
    std::ifstream inFile(inPath, std::ifstream::binary );
    inFile.unsetf(std::ios_base::skipws);
        
    while (inFile) {
        inFile >> byteBuffer;
        code = codes.at(byteBuffer);

        for (const bool bit: code) {
            bitBuffer.push(bit);
        }
            
        while (bitBuffer.size() > 8) {
            writeBytes = to_Byte(bitBuffer); //this is slow
            outFile.write((char*)&writeBytes,sizeof(writeBytes));
        }
    }
    // read last bits if any remain
    //unsigned short remainder = bitBuffer.size();
    unsigned short validBitsInLastByte = bitBuffer.size();
    if (validBitsInLastByte > 0) {
        // ############################################# Fix issue with last byte
        writeBytes = to_Byte(bitBuffer);
        outFile.write((char*)&writeBytes,sizeof(writeBytes));
    }

    // write last byte as validBitsInLastByte
    outFile.seekp(1023);
    outFile.write((char*)&validBitsInLastByte,sizeof(char));
    
    inFile.close();
    outFile.close();
}

int main(int argc, char const *argv[])
{
    const char * inPath = "./example1.txt";
    const char * outPath = "./output.huffman";

/*     const char * inPath = "./image.bmp";
    const char * outPath = "./image.huffman"; */
    
    bool encode_or_decode = false;

    if (encode_or_decode) {
        clock_t begin = clock();
        std::map<unsigned char, int> frequency_table = make_frequency_table(inPath);
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        std::cout << "1: " << elapsed_secs << "\n"; 

        Node* current = make_tree(frequency_table);
        //print_tree(current);
        std::cout << "2\n";
        std::unordered_map<unsigned char, std::vector<bool>> codes = encode(current);
        std::cout << "3\n";
        begin = clock();
        write_to_file(inPath, frequency_table, codes, outPath);  
        end = clock();
        elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        std::cout << "4: " << elapsed_secs << "\n"; //this one takes too long  
        
    }
    else {
        std::pair<std::map<unsigned char, int>, int> frequency_table_in = read_table_from_file(outPath);
        Node* root = make_tree(frequency_table_in.first);
        //print_tree(root);
        //std::unordered_map<unsigned char, std::vector<bool>> outCodes = encode(root);
        //std::cout << "3\n"; 
        clock_t begin = clock();
        std::string outdata = read_data_from_file(outPath, root, frequency_table_in.second);
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        std::cout << "1: " << elapsed_secs << "\n"; //this one takes too long

        //for (auto const &f: frequency_table_in) {
        //    printf("0x%x | %i\n", f.first, f.second);
        //} 

        std::ofstream out("./exmaple1.out");
        //std::ofstream out("./imageout.bmp");
        out << outdata;
        out.close();
        std::cout << "2\n";
    }
    

    



    
            
    return 0;
}
