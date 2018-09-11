#include <stdio.h>
#include <iostream>
#include <map>
#include <algorithm>
#include <vector>
#include <queue>
#include <stack>

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

struct CompareValue
{
    bool operator()(const Node* node1, const Node* node2) const
    {
        return node1->getValue().second > node2->getValue().second;
    }
};

std::map<char, std::vector<bool>> encode(Node* root) {
    std::queue<std::pair<Node *, std::vector<bool>>> nodeStack;

    std::vector<bool> v = {};
    nodeStack.push(std::pair<Node *, std::vector<bool>>(root,v));

    std::vector<bool> leftv, rightv;
    std::pair<Node *, std::vector<bool>> current, left, right;

    std::map<char, std::vector<bool>> out;

    while (!nodeStack.empty()) {
        current = nodeStack.front();
        nodeStack.pop();

        if (current.first->getLeftChild() != NULL) {
            leftv = current.second;
            leftv.push_back(0);
            left = std::pair<Node *, std::vector<bool>>(current.first->getLeftChild(),leftv);

            rightv = current.second;
            rightv.push_back(1);
            right = std::pair<Node *, std::vector<bool>>(current.first->getRightChild(),rightv);

            nodeStack.push(left);
            nodeStack.push(right);
        }
        else {
            out[current.first->getValue().first] = current.second;
            for (std::vector<bool>::const_iterator i = current.second.begin(); i != current.second.end(); ++i)
                std::cout << *i << ' ';
            std::cout << " | " << current.first->getValue().first << "\n";


            
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
        if (parent->getLeftChild())
            q.push_back(std::pair<Node *, int>(parent->getLeftChild(), level + 1));
        if (parent->getLeftChild())
            q.push_back(std::pair<Node *, int>(parent->getRightChild(), level + 1));
    }
}

int main(int argc, char const *argv[])
{
    FILE *filePointer = fopen("example1.txt", "r");
    
    char buffer[BUFFERLENGTH+1];
    size_t charactersRead = 0;
    
    std::map<char, int> frequency_table;
    std::priority_queue<Node*, std::vector<Node*>, CompareValue> forestPq;

    if (filePointer != NULL) {
        while (charactersRead = fread(buffer, sizeof(char), BUFFERLENGTH, filePointer) > 0) {
            char character = *buffer;
            std::map<char, int>::iterator charInSet = frequency_table.find(character);

            if (charInSet == frequency_table.end()) {
                frequency_table.insert(std::pair<char, int>(character,0));
            }
            frequency_table[character] ++;
           
        }

        for (std::map<char, int>::iterator frequency_iterator = frequency_table.begin();
            frequency_iterator != frequency_table.end(); ++frequency_iterator) {
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

    Node* current = forestPq.top();
    encode(current);
 
    return 0;
}