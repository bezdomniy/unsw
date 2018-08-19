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
        HuffmanPair getValue();
        Node *getLeftChild();
        Node *getRightChild();

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

Node *Node::getLeftChild() {
    return leftChild;
}
Node *Node::getRightChild() {
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
    bool operator()(const Node &node1, const Node &node2) const
    {
        return node1.getValue().second < node2.getValue().second;
    }
};

Node getMin(std::unordered_set<Node, nodeHash> nodeSet) 
{
    Node min 
      = *std::min_element(nodeSet.begin(), nodeSet.end(), CompareValue());

    return min;
}

int main(int argc, char const *argv[])
{
    FILE *filePointer = fopen("example1.txt", "r");
    
    char buffer[BUFFERLENGTH+1];
    size_t charactersRead = 0;
    
    std::unordered_set<Node, nodeHash> forest;
    std::priority_queue<Node, std::vector<Node>, CompareValue> forestPq;
    
    if (filePointer != NULL) {
        while (charactersRead = fread(buffer, sizeof(char), BUFFERLENGTH, filePointer) > 0) {
            char character = *buffer;

            HuffmanPair newPair(character,1);
            Node newNode(newPair);
            std::unordered_set<Node, nodeHash>::iterator nodeInSet = forest.find(newNode);

            if (nodeInSet != forest.end()) {
                forest.erase(newNode);
                int newFreq = nodeInSet->getValue().second + 1;
                newNode.updateFrequency(newFreq);
            }
            forest.insert(newNode);
        }

        for (std::unordered_set<Node, nodeHash>::iterator forestIterator = forest.begin();
            forestIterator != forest.end(); ++forestIterator) {
                forestPq.push(*forestIterator);
            }

        /*for (const auto &p : forest) {
            std::cout << "forest[" << p.getValue().first << "] = " << p.getValue().second << '\n';
        } */

        if (!forestPq.empty()) {
            Node test = forestPq.top();
            forestPq.pop();
            std::cout << test.getValue().first;
        }
        
    }

    //printf("%c",getMin(forest).getValue().first);

 /*   while (forest.size() > 1) {
        Node min1 = getMin(forest);
        forest.erase(min1);
        Node min2 = getMin(forest);
        forest.erase(min2);

        HuffmanPair newVal(NULL, min1.getValue().second + min2.getValue().second);
        Node newNode(newVal,&min1,&min2);
        forest.insert(newNode);
    }*/

/*     
    Node *current = root;

    //printf("%c\n",current.getValue().second);
    
    while (current->getRightChild() != NULL) {
        printf("%i\n",current->getValue().second);
        current = current->getRightChild();
    }*/
 
    return 0;
}
