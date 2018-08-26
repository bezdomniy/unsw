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

#define FREQUENCY_BYTES 3

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
            return node1->getValue().first < node2->getValue().first;
        } 
            
    }
};

// https://stackoverflow.com/a/2453456
template <typename T1, typename T2>
struct less_second {
    typedef std::pair<T1, T2> type;
    bool operator ()(type const& a, type const& b) const {
        return a.second < b.second;
    }
};

// https://stackoverflow.com/a/2390938
bool is_empty(std::ifstream& pFile)
{
    return pFile.peek() == std::ifstream::traits_type::eof();
}

std::unordered_map<unsigned char, std::vector<bool>> make_code_map(Node* root) {
    std::queue<CodePair> nodeStack;
    std::unordered_map<unsigned char, std::vector<bool>> out;


    std::vector<bool> leftv, rightv;
    CodePair current, left, right;

    std::vector<bool> v;
    if (root->getLeftChild() == NULL) {
        v.push_back(0);
        out[root->getValue().first] = v;
        return out;
    }

    nodeStack.push(CodePair(root,v));

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
    }
    else {
        for (int i=0; i < 8; ++i) 
            b.push_back((c & (1<<i)) != 0);  
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

/* unsigned char int_to_Byte(int& b)
{
    unsigned char c = 0;
    for (int i=0; i < 8; ++i) {
        if (b.front()) 
            c |= 1 << i;
        b.pop();
    }
    return c;
} */

std::map<unsigned char, int> make_frequency_table(const char * filePath) {
    FILE *filePointer = fopen(filePath, "rb");
    std::map<unsigned char, int> frequency_table;

    if (filePointer != NULL)  {
        unsigned char buffer[1+1];
        size_t charactersRead = 0;

        std::ifstream input( filePath, std::ifstream::binary );
        input.unsetf(std::ios_base::skipws);

        while (charactersRead = fread(buffer, sizeof(unsigned char), 1, filePointer) > 0) {
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
            if (frequency_iterator->second != 0) {
                HuffmanPair newVal(*frequency_iterator);
                Node *newNode = new Node(newVal);
                forestPq.push(newNode);
            }
        }

    if (forestPq.size() != 1) {
        while (forestPq.size() > 1) {
                Node* min1 = forestPq.top();
                forestPq.pop();
                Node* min2 = forestPq.top();
                forestPq.pop();

                HuffmanPair newVal(NULL, min1->getValue().second + min2->getValue().second);
                Node *newNode = new Node(newVal,min2,min1);
                forestPq.push(newNode);
        }
    }
    return forestPq.top();
}

std::pair<std::map<unsigned char, int>, int> read_table_from_file(const char * path) {
    std::map<unsigned char, int> out;
    

    std::ifstream input( path, std::ifstream::binary );
    input.unsetf(std::ios_base::skipws);
    std::vector<unsigned char> buf(1024);
    input.read((char*)&buf[0],1024);

    bool newChar = true;
    unsigned char* current_char = NULL;
    int bytePos = 0;
    unsigned char byteBuffer[FREQUENCY_BYTES];

    bool firstByte = true;
    int validBitsInLastByte = (int)buf[FREQUENCY_BYTES-1];

    for (unsigned char &i: buf) {
        unsigned int number;
        if (!newChar) {
            if (out.find(i) != out.end()) {
                break;
            }
            //printf("\n0x%x", i);
            current_char = &i;
            out[i] = 0;

            newChar = true;

            if (number == 0) {
                if (out[*current_char] == 0)
                    out.erase(*current_char);
                    break;
                }
            out[*current_char] = number;
        } 
        else {
            byteBuffer[bytePos] = i;
            bytePos++;

            if (bytePos == FREQUENCY_BYTES) {
                if (firstByte) {
                    firstByte = false;
                    byteBuffer[FREQUENCY_BYTES-1] = 0x0;
                }

                switch (FREQUENCY_BYTES) {
                    case 4:
                        number = (unsigned int)(byteBuffer[3] << 24 | byteBuffer[2] << 16 | byteBuffer[1] << 8 | byteBuffer[0]);
                    case 3:
                        number = (unsigned int)(byteBuffer[2] << 16 | byteBuffer[1] << 8 | byteBuffer[0]);
                    case 2:
                        number = (unsigned int)(byteBuffer[1] << 8 | byteBuffer[0]);
                    case 1:
                        number = (unsigned int)(byteBuffer[0]);
                    default:
                        number = (unsigned int)(byteBuffer[2] << 16 | byteBuffer[1] << 8 | byteBuffer[0]);
                }


                bytePos = 0;


                newChar = false;
            }
        }
    }
    //validBitsInLastByte =  (int)buf.back();
    //validBitsInLastByte =  (int)buf[0];

    return std::pair<std::map<unsigned char, int>, int>(out,validBitsInLastByte);
}

std::string read_data_from_file(const char * path, Node* root, int validBitsInLastByte) {
    std::string out;

    bool singleNodeTree = false;
    if (root->getLeftChild() == NULL)
        singleNodeTree = true;

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
                if (singleNodeTree)
                    bitBuffer.pop_front();
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

int search_encoded_file(const char * path, Node* root, int validBitsInLastByte, const char * searchTerm) {
    int position = 0;

    bool singleNodeTree = false;
    if (root->getLeftChild() == NULL)
        singleNodeTree = true;

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
                //out += current->getValue().first;
                current = root;

                position++;

                if (singleNodeTree)
                    bitBuffer.pop_front();
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

    return position;
}

void write_to_file(const char * inPath, std::map<unsigned char, int> frequency_table, std::unordered_map<unsigned char, std::vector<bool>> codes, const char * outPath) {
    std::ofstream outFile(outPath, std::ofstream::binary);
    outFile.unsetf(std::ios_base::skipws);

    bool singleNodeTree = false;
    if (codes.size() == 1)
        singleNodeTree = true;

    std::vector<std::pair<unsigned char, int> > frequency_table_sorted_by_value(frequency_table.begin(), frequency_table.end());
    std::sort(frequency_table_sorted_by_value.begin(), frequency_table_sorted_by_value.end(), less_second<unsigned char, int>());

    // writing frequency table
    int dataSize = 0;
    for (const std::pair<unsigned char, int> &freq: frequency_table_sorted_by_value) {
        outFile.write((char*)&freq.second,FREQUENCY_BYTES);
        outFile.write((char*)&freq.first,sizeof(freq.first));
        
        dataSize += FREQUENCY_BYTES+1;
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

    if (singleNodeTree)
        inFile >> byteBuffer;
    
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

    unsigned short validBitsInLastByte = bitBuffer.size();
    //std::cout << validBitsInLastByte;
    if (validBitsInLastByte > 0) {
        writeBytes = to_Byte(bitBuffer);
        outFile.write((char*)&writeBytes,sizeof(writeBytes));
    }

    // write last byte as validBitsInLastByte
    //outFile.seekp(1023);
    outFile.seekp(FREQUENCY_BYTES-1);
    outFile.write((char*)&validBitsInLastByte,1);
    
    inFile.close();
    outFile.close();
}


int main(int argc, char const *argv[])
{
    const char * originalPath; // = "./example1.txt";
    const char * encodedPath; // = "./output.huffman";
    const char * decodedPath; // = "./example1.out";
    const char * searchTerm; // = "apple";
    
    std::string option = argv[1];

    if (option == "-e") {
        originalPath = argv[2];
        encodedPath = argv[3];

        std::ifstream inFile(originalPath, std::ifstream::binary );
        if (is_empty(inFile)) {
            FILE *fp = fopen(encodedPath, "w");
            fclose(fp);
        }
        else {
//            clock_t begin = clock();
            std::map<unsigned char, int> frequency_table = make_frequency_table(originalPath);
//            clock_t end = clock();
//            double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
//            std::cout << "1: " << elapsed_secs << "\n"; 

            Node* current = make_tree(frequency_table);
            //print_tree(current);
            std::unordered_map<unsigned char, std::vector<bool>> codes = make_code_map(current);
//            begin = clock();
            write_to_file(originalPath, frequency_table, codes, encodedPath);  
//            end = clock();
//            elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
//            std::cout << "2: " << elapsed_secs << "\n"; //this one takes too long  
        }
    }
    else if (option == "-d") {
        encodedPath = argv[2];
        decodedPath = argv[3];

        std::ifstream inFile(encodedPath, std::ifstream::binary );
        if (is_empty(inFile)) {
            FILE *fp = fopen(decodedPath, "w");
            fclose(fp);
        }
        else {
            std::pair<std::map<unsigned char, int>, int> frequency_table_in = read_table_from_file(encodedPath);
            Node* root = make_tree(frequency_table_in.first);
            //print_tree(root);
            std::string outdata = read_data_from_file(encodedPath, root, frequency_table_in.second);
            std::ofstream out(decodedPath);
            out << outdata;
            out.close();
        }
    }
    else if (option == "-s") {
        searchTerm = argv[2];
        encodedPath = argv[3];

        std::pair<std::map<unsigned char, int>, int> frequency_table_in = read_table_from_file(encodedPath);
        Node* root = make_tree(frequency_table_in.first);

    }


    



    
            
    return 0;
}
