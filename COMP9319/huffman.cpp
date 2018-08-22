#include <stdio.h>
#include <iostream>
#include <map>
#include <algorithm>
#include <vector>
#include <queue>
#include <fstream>
#include <iterator>
#include <cstring>


#define BUFFERLENGTH 1

typedef std::pair<char, int> HuffmanPair;

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
        return node1->getValue().second > node2->getValue().second;
    }
};

std::map<char, std::vector<bool>> encode(Node* root) {
    std::queue<CodePair> nodeStack;

    std::vector<bool> v = {};
    nodeStack.push(CodePair(root,v));

    std::vector<bool> leftv, rightv;
    CodePair current, left, right;

    std::map<char, std::vector<bool>> out;

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
        std::cout << parent->getValue().second << " " << parent->getValue().first << std::endl;

        //printf("%i %i\n", parent->getValue().second ,parent->getValue().first);
        if (parent->getLeftChild())
            q.push_back(std::pair<Node *, int>(parent->getLeftChild(), level + 1));
        if (parent->getLeftChild())
            q.push_back(std::pair<Node *, int>(parent->getRightChild(), level + 1));
    }
}


// reference: https://stackoverflow.com/questions/8461126/how-to-create-a-byte-out-of-8-bool-values-and-vice-versa
std::vector<bool> from_Byte(unsigned char c)
{
    std::vector<bool> b;
    for (int i=0; i < 8; ++i)
        b.push_back((c & (1<<i)) != 0);
    return b;
}

unsigned char to_Byte(std::vector<bool> b)
{
    unsigned char c = 0;
    int len = b.size();
    //std::cout << len << ", ";

    for (int i=0; i < 8; ++i)
        if (b[i])
            c |= 1 << (i+8-len);
    return c;
}

std::map<unsigned char, int> make_frequency_table(const char * filePath) {

    FILE *filePointer = fopen(filePath, "r");
    std::map<unsigned char, int> frequency_table;

    if (filePointer != NULL)  {
        unsigned char buffer[BUFFERLENGTH+1];
        size_t charactersRead = 0;

        while (charactersRead = fread(buffer, sizeof(unsigned char), BUFFERLENGTH, filePointer) > 0) {
                unsigned char character = *buffer;
                std::map<unsigned char, int>::iterator charInSet = frequency_table.find(character);

                if (charInSet == frequency_table.end()) {
                    frequency_table.insert(std::pair<unsigned char, int>(character,0));
                }
                frequency_table[character] ++;
            
            }
        return frequency_table;
    }
    std::fclose(filePointer);
}

Node* make_tree(std::map<unsigned char, int> frequency_table) {
    std::priority_queue<Node*, std::vector<Node*>, CompareValue> forestPq;

    for (std::map<unsigned char, int>::iterator frequency_iterator = frequency_table.begin();
        frequency_iterator != frequency_table.end(); ++frequency_iterator) {
            HuffmanPair newVal(*frequency_iterator);
            Node *newNode = new Node(newVal);
            forestPq.push(newNode);
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


// https://stackoverflow.com/a/14807477
template<typename A, typename B>
std::pair<B,A> flip_pair(const std::pair<A,B> &p)
{
    return std::pair<B,A>(p.second, p.first);
}

template<typename A, typename B>
std::map<B,A> flip_map(const std::map<A,B> &src)
{
    std::map<B,A> dst;
    std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()), 
                   flip_pair<A,B>);
    return dst;
}
//////////

std::map<unsigned char, int> read_table_from_file(const char * path) {
    std::map<unsigned char, int> out;

    std::ifstream input( path, std::ifstream::binary );
    input.unsetf(std::ios_base::skipws);
    std::vector<unsigned char> buf(1024);
    input.read((char*)&buf[0],1024);

    bool newChar = true;
    unsigned char* current_char = NULL;

    int bytePos = 0;
    unsigned char byteBuffer[4];

    for (auto &i: buf) {
        if (newChar) {
            //std::cout << i;
            current_char = &i;
            out[newChar] = 0;

            newChar = false;
        } 
        else {
            byteBuffer[bytePos] = i;
            bytePos++;

            if (bytePos == 4) {
                int number = (int)(byteBuffer[3] << 24 | byteBuffer[2] << 16 | byteBuffer[1] << 8 | byteBuffer[0]);
                bytePos = 0;

                out[*current_char] = number;
                //std::cout << number << "|\n";
                newChar = true;
            }
        }
    }  

    return out;
}

std::string read_data_from_file(const char * path, std::map<char, std::vector<bool>> codeMap) {
    std::string out;

    std::map<std::vector<bool>, char> codes = flip_map(codeMap);

    std::ifstream input(path, std::ifstream::binary );
    input.unsetf(std::ios_base::skipws);

    input.seekg(1024);

    char byteBuffer;
    std::vector<bool> nextBits;

    std::vector<bool> bitBuffer;

    while (input) {
        input >> byteBuffer;
        nextBits = from_Byte(byteBuffer);

        for (bool c: nextBits) {
            //out += c?'1':'0';
            bitBuffer.insert(bitBuffer.end(), c);

            if (codes[bitBuffer]) {
                out += codes[bitBuffer];
                bitBuffer.clear();
            }
        }
    }
    input.close();

    return out;
}

void write_to_file(const char * inPath, std::map<unsigned char, int> frequency_table, std::map<char, std::vector<bool>> codes, const char * outPath) {
    FILE *inFilePointer = fopen(inPath, "r");
    if (inFilePointer != NULL) {
        std::ofstream FILE(outPath, std::ofstream::binary);

        // writing frequency table
        int dataSize = 0;
        for (auto &freq: frequency_table) {
            FILE.write((char*)&freq.first,sizeof(freq.first));
            FILE.write((char*)&freq.second,sizeof(freq.second));

            dataSize += 5;
        }

        for (int i = 0; i < (1024-dataSize); i++) {
            FILE.write("\x00",1);
        }
        

        // writing data
        unsigned char buffer[BUFFERLENGTH+1];
        size_t charactersRead = 0;

        std::vector<bool> bitBuffer;
        char writeBytes;
        
        while (charactersRead = fread(buffer, sizeof(unsigned char), BUFFERLENGTH, inFilePointer) > 0) {
            unsigned char character = *buffer;
            std::vector<bool> code = codes[character];

            bitBuffer.insert(bitBuffer.end(), code.begin(), code.end());

            if (bitBuffer.size() > 8) {
                std::vector<bool> outByte(bitBuffer.begin(), bitBuffer.begin()+8);
                bitBuffer.erase(bitBuffer.begin(), bitBuffer.begin() + 8);

                writeBytes = to_Byte(outByte);
                FILE.write(&writeBytes,sizeof(writeBytes));
            }
        }
        // read last bits if any remain
        if (bitBuffer.size() > 0) {
            writeBytes = to_Byte(bitBuffer);
            FILE.write(&writeBytes,sizeof(writeBytes));
        }
    }
    std::fclose(inFilePointer);
}

int main(int argc, char const *argv[])
{
    const char * inPath = "./warandpeace.txt";
    const char * outPath = "./output.huffman";
    
    std::map<unsigned char, int> frequency_table;
    

    frequency_table = make_frequency_table(inPath);
        
    Node* current = make_tree(frequency_table);
    std::map<char, std::vector<bool>> codes = encode(current);

    //print_tree(current);

    //write_to_file(inPath, frequency_table, codes, outPath);
    std::string outdata = read_data_from_file(outPath, codes);
    std::cout << outdata;

    

    

/*     std::map<unsigned char, int> output_frequency_table = read_table_from_file(outPath);

    for (auto const &x: output_frequency_table) {
        std::cout << x.first << " | " << x.second << "\n";
    } */


/*     std::vector<bool> test({0,0,0,0,1});

    unsigned char test_b = to_Byte(test);

    std::vector<bool> test_2 = from_Byte(test_b);

    std::cout << test_b << "\n";
    for (auto const &i: test) {
        std::cout << i;
    }
    std::cout << "\n";
    for (auto const &i: test_2) {
        std::cout << i;
    } */

/*     unsigned int y = 65000;
    std::cout << y << " | " << sizeof(y) << "\n";
    std::vector<unsigned char> te = int_to_charvec(y);

    //char te = y;
    std::cout << "\n|";
    for (auto const &i: te) {
        std::cout << i;
    }
    std::cout << "|\n";

    int out = 0;
    for (auto const &i: te) {
        from_Byte(i);
        out += (int)i;
        std::cout << "|"<< i;
    }
    std::cout << out; */
    
            
    return 0;
}
