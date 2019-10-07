#include "searchTree.h"

SearchTree::SearchTree(Sequent& rootNodeValue)
{
    this->rootNode = std::make_shared<Node>();
    std::pair<Sequent, std::optional<Sequent>> v;
    v.first = rootNodeValue;
    this->rootNode->data = v;
    this->rootNode->parent = nullptr;
    // this->current = &this->rootNode;
}

SearchTree::~SearchTree()
{
}

const std::shared_ptr<Node>& SearchTree::getRootPtr()
{
    return this->rootNode;
}

void SearchTree::addChild(const std::shared_ptr<Node>& child, const std::shared_ptr<Node>& current) 
{
    current->children.push_back(child);
}


void SearchTree::childrenFromSequent(const std::vector<std::string>& s, const std::shared_ptr<Node>& current, bool left)
{
    for (int i = 0; i < s.size(); i++)
    {
        if (Rule::checkMatch(s.at(i))) {
            std::shared_ptr<Node> newNodePtr = std::make_shared<Node>();
            newNodePtr->parent = current;
            newNodePtr->data = Rule::tranform(current->data.first, left, i);

            addChild(newNodePtr, current);

            // newNodePtr->data.first.print();
            // if (newNodePtr->data.second.has_value()) {
            //     newNodePtr->data.second.value().print();
            // }

        }
    }
}

void SearchTree::findTransformations(const std::shared_ptr<Node>& current)
{
    childrenFromSequent(current->data.first.getLeft(), current, true);
    childrenFromSequent(current->data.first.getRight(), current, false);

    if (current->data.second.has_value()) {
        childrenFromSequent(current->data.second.value().getLeft(), current, true);
        childrenFromSequent(current->data.second.value().getRight(), current, false);
    }

    for (auto& node: current->children) {
        findTransformations(node);
    }

    if (current->children.empty()) {
        current->data.first.print();

        if (current->data.second.has_value()) {
            current->data.second.value().print();
        }
        std::cout << std::endl;
    }
        
}