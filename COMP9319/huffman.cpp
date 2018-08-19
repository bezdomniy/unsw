#include <stdio.h>
#include <iostream>
#include <map>
#include <algorithm>

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
HuffmanPair Node::getValue() {
    return value;
}
Node *Node::getLeftChild() {
    return leftChild;
}

Node *Node::getRightChild() {
    return rightChild;
}


struct CompareValue
{
    bool operator()(const HuffmanPair & left, const HuffmanPair & right) const
    {
        return left.second < right.second;
    }
};

HuffmanPair getMin(std::map<char, int> mymap) 
{
    std::pair<char, int> min 
      = *std::min_element(mymap.begin(), mymap.end(), CompareValue());
    return min;
}

int main(int argc, char const *argv[])
{
    FILE *filePointer = fopen("warandpeace.txt", "r");
    
    char buffer[BUFFERLENGTH+1];
    size_t charactersRead = 0;
    
    std::map<char,int> frequencyTable;
    
    if (filePointer != NULL) {
        while (charactersRead = fread(buffer, sizeof(char), BUFFERLENGTH, filePointer) > 0) {
            char character = *buffer;

            if (frequencyTable.find(character) == frequencyTable.end()) {
                frequencyTable[character] = 1;
            }
            else {
                frequencyTable[character]++;
            }
        }

        /*for (const auto &p : frequencyTable) {
            std::cout << "frequencyTable[" << p.first << "] = " << p.second << '\n';
        }*/
    }

    HuffmanPair min1 = getMin(frequencyTable);
    frequencyTable.erase(min1.first);
    HuffmanPair min2 = getMin(frequencyTable);
    frequencyTable.erase(min2.first);
    Node right(min1);
    Node left(min2);

    HuffmanPair val(NULL, min1.second + min2.second);

    Node *root = new Node(val, &left, &right);

    printf("%c,%i\n",root->getValue().first,root->getValue().second);
    printf("%c,%i\n",root->getLeftChild()->getValue().first,root->getLeftChild()->getValue().second);
    printf("%c,%i\n\n",root->getRightChild()->getValue().first,root->getRightChild()->getValue().second);
    
    //printf("'%c,%c,%c' | ",root.getValue().first,root.getLeftChild().getValue().first,root.getRightChild().getValue().first);
    while (!frequencyTable.empty()) {
        HuffmanPair min = getMin(frequencyTable);
        frequencyTable.erase(min.first);

        HuffmanPair newVal(NULL, root->getValue().second + min.second);
        Node newNode(min);

        if (min.second < root->getValue().second) {
            Node *newRoot = new Node(newVal, root, &newNode);
            root = newRoot;
            //std::cout << "here";
        }
        else {
            Node *newRoot = new Node(newVal, &newNode, root);
            root = newRoot;
            //std::cout << "here2";
        }

        printf("%c,%i\n",root->getValue().first,root->getValue().second);
        printf("%c,%i\n",root->getLeftChild()->getValue().first,root->getLeftChild()->getValue().second);
        printf("%c,%i\n\n",root->getRightChild()->getValue().first,root->getRightChild()->getValue().second);

        
        //printf("'%c,%c,%c' | ",root.getValue().first,root.getLeftChild().getValue().first,root.getRightChild().getValue().first);
        //std::cout << root.getValue().first << root.getLeftChild().getValue().first << root.getRightChild().getValue().first;
    }
    //std::cout << root.getValue().first << root.getLeftChild().getValue().first << root.getRightChild().getValue().first ;

    Node *current = root;

    //printf("%c\n",current.getValue().second);
    
    while (current->getLeftChild() != NULL) {
        printf("%i\n",current->getValue().second);
        current = current->getLeftChild();
    }

    return 0;
}
