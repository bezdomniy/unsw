#include <stdio.h>
#include <iostream>
#include <map>
#include <algorithm>
#include <vector>
#include <queue>
#include <fstream>
#include <iterator>


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
    std::cout << len << ", ";

    for (int i=0; i < 8; ++i)
        if (b[i])
            c |= 1 << (i+8-len);
    return c;
}

std::map<char, int> make_frequency_table(FILE *filePointer) {
    char buffer[BUFFERLENGTH+1];
    size_t charactersRead = 0;
    
    std::map<char, int> frequency_table;

    while (charactersRead = fread(buffer, sizeof(char), BUFFERLENGTH, filePointer) > 0) {
            char character = *buffer;
            std::map<char, int>::iterator charInSet = frequency_table.find(character);

            if (charInSet == frequency_table.end()) {
                frequency_table.insert(std::pair<char, int>(character,0));
            }
            frequency_table[character] ++;
           
        }
    return frequency_table;
}

Node* make_tree(std::map<char, int> frequency_table) {
    std::priority_queue<Node*, std::vector<Node*>, CompareValue> forestPq;

    for (std::map<char, int>::iterator frequency_iterator = frequency_table.begin();
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

int main(int argc, char const *argv[])
{
    FILE *filePointer = fopen("example1.txt", "r");
    std::map<char, int> frequency_table;
    
    if (filePointer != NULL) 
        frequency_table = make_frequency_table(filePointer);
        
    Node* current = make_tree(frequency_table);
    std::map<char, std::vector<bool>> codes = encode(current);

    print_tree(current);

/*     for (auto const &code: codes) {
        for (auto const &i: code.second)
            std::cout << i;
        std::cout << " | " << code.first << "\n";
    } */
        
    std::string path("/mnt/c/dev/unsw/COMP9319/output.huffman");
    std::ofstream FILE(path, std::ofstream::binary);
    for (auto const &code: codes) {
        FILE.write(&code.first,sizeof(code.first));

        char code_byte = to_Byte(code.second);
        std::cout << "writing: " << int(code.first) << ", ";
        for (auto const &bv: code.second) {
            std::cout << bv;
        }
        std::cout << "\n"; 
        FILE.write(&code_byte,sizeof(code_byte));
    }

    std::ifstream input( path, std::ifstream::binary );
    input.unsetf(std::ios_base::skipws);

    std::istream_iterator<char> begin(input), end;
    std::vector<char> buf(begin,end);
    //std::cout << buf.size() << "\n";


    for (auto const &i: buf) {
        // could be any char ... what do?
        if ((i >= 'a' && i <= 'z') || (i >= 'A' && i <= 'Z')) {
            std::cout << i;
        }
        else {
            std::vector<bool> ch = from_Byte(i);
             for (auto const &j: ch) {
                std::cout << j << "\0";
            } 
        }
        std::cout << "\n"; 
    } 
    
    
            
    return 0;
}
