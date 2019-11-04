#include "searchTree.h"

SearchTree::SearchTree(Sequent& rootNodeValue)
{
    this->rootNode = std::make_shared<Node>();
    std::pair<Sequent, std::optional<Sequent>> v;
    v.first = rootNodeValue;
    this->rootNode->data = v;
    this->rootNode->parent = nullptr;
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

// Loop through list of formulas and apply sequent rule to decompose where possible
void SearchTree::childrenFromSequent(Sequent& s,std::vector<std::string>& formulas, const std::shared_ptr<Node>& current, bool left)
{
    for (int i = 0; i < formulas.size(); i++)
    {
        if (Rule::checkMatch(formulas.at(i))) {
            std::shared_ptr<Node> newNodePtr = std::make_shared<Node>();
            newNodePtr->parent = current;
            newNodePtr->data = Rule::tranform(s, left, i);

            addChild(newNodePtr, current);
        }
    }
}

// Recursively decompose formulas in sequent until you are left with a formula that satisfies sequent rule P1
void SearchTree::findTransformations(const std::shared_ptr<Node>& current, std::vector<std::shared_ptr<Node>>* outVec, bool* resultFound)
{
    if (!(*resultFound)) {
        childrenFromSequent(current->data.first, current->data.first.getLeft(), current, true);
        childrenFromSequent(current->data.first, current->data.first.getRight(), current, false);

        if (current->data.second.has_value()) {
            childrenFromSequent(current->data.second.value(), current->data.second.value().getLeft(), current, true);
            childrenFromSequent(current->data.second.value(), current->data.second.value().getRight(), current, false);
        }

        for (auto& node: current->children) {
            findTransformations(node, outVec, resultFound);
        }

        if (current->children.empty()) {
            if (Utils::checkFirstRule(current->data.first.getLeft(), current->data.first.getRight())) {
                *resultFound = true;
                outVec->push_back(current);
            }
            else if (current->data.second.has_value()) {
                if (Utils::checkFirstRule(current->data.second.value().getLeft(), current->data.second.value().getRight())) {
                    *resultFound = true;
                    outVec->push_back(current);
                }
            }
        }
    }  
}