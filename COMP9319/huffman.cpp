#include <stdio.h>
#include <iostream>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <vector>
#include <queue>

#define BUFFERLENGTH 1

typedef std::pair<char, int> HuffmanPair;

class Node {
    public:
        Node(HuffmanPair value, Node *left, Node *right);
        Node(HuffmanPair value);
        Node();
        //HuffmanPair getValue();
        Node *getLeftChild() const;
        Node *getRightChild() const;

/*         bool operator< (const Node &other) const
            {
                return this->value.second < other.value.second;
            } */

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

struct nodeHash {
    size_t operator() (const Node &node ) const {
        return node.getValue().first;
    }
};

/*
struct nodeEqual {
    bool operator() (const Node &node1, const Node &node2) const {
        return node1.getValue().first == node2.getValue().first;
    }
};*/

struct CompareValue
{
    bool operator()(const Node* node1, const Node* node2) const
    {
        return node1->getValue().second > node2->getValue().second;
    }
};

/* Node getMin(std::unordered_set<Node, nodeHash> nodeSet) 
{
    Node min 
      = *std::min_element(nodeSet.begin(), nodeSet.end(), CompareValue());

    return min;
} */

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

        /*for (const auto &p : forest) {
            std::cout << "forest[" << p.getValue().first << "] = " << p.getValue().second << '\n';
        } */

    }

    while (forestPq.size() > 1) {
        Node* min1 = forestPq.top();
        forestPq.pop();
        Node* min2 = forestPq.top();
        forestPq.pop();

        //printf("%i\n",min1.getValue().second);
        //printf("%i\n",min2.getValue().second);

        HuffmanPair newVal(NULL, min1->getValue().second + min2->getValue().second);
        Node *newNode = new Node(newVal,min1,min2);
        forestPq.push(newNode);
    }

    Node* current = forestPq.top();

    printf("%i\n",current->getValue().second); 
    //printf("%i\n",current->getRightChild()->getValue().second); 
    printf("%i\n",current->getLeftChild()->getValue().second); 

    //printf("%i\n",current->getLeftChild()->getRightChild()->getValue().second); 
    //std::cout << (current->getLeftChild()->getLeftChild() != NULL) << "\n";
    printf("%i\n",current->getLeftChild()->getLeftChild()->getValue().second);  
    

    /* Node* current = forestPq.top();

    //printf("%c\n",current.getValue().second);
    
    while (current->getLeftChild() != NULL) {
        printf("%i\n",current->getValue().second);
        current = current->getLeftChild();
    } */ 
 
    return 0;
}
