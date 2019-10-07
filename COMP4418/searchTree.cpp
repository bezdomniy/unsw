#include "searchTree.h"

SearchTree::SearchTree(Sequent& rootNodeValue)
{
    std::pair<Sequent, std::optional<Sequent>> v;
    v.first = rootNodeValue;
    this->rootNode.data = v;
    this->rootNode.parent = nullptr;
    // this->current = &this->rootNode;
}

SearchTree::~SearchTree()
{
}

Node* SearchTree::getRootPtr()
{
    return &this->rootNode;
}

void SearchTree::addChild(Node* child, Node* current) 
{
    current->children.push_back(child);
}


void SearchTree::childrenFromSequent(const std::vector<std::string>& s, Node* current, bool left)
{
    for (int i = 0; i < s.size(); i++)
    {
        if (Rule::checkMatch(s.at(i))) {
            Node newNode;
            newNode.parent = current;
            newNode.data = Rule::tranform(current->data.first, left, i);

            addChild(&newNode, current);

            newNode.data.first.print();
            if (newNode.data.second.has_value()) {
                newNode.data.second.value().print();
            }

        }
    }
}

void SearchTree::findTransformations(Node* current)
{
    childrenFromSequent(current->data.first.getLeft(), current, true);
    childrenFromSequent(current->data.first.getRight(), current, false);

    if (current->data.second.has_value()) {
        childrenFromSequent(current->data.second.value().getLeft(), current, true);
        childrenFromSequent(current->data.second.value().getRight(), current, false);
    }
        
}