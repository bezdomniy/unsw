#pragma once
#include <string>
#include <vector>
#include <utility>
#include <optional>
#include "rule.h"
#include "sequent.h"

struct Node
{
    Node* parent;
    std::vector<Node*> children;
    std::pair<Sequent, std::optional<Sequent>> data;
};

class SearchTree
{
private:
    Node rootNode;
    // Node* current;

    void addChild(Node*, Node*);
    void childrenFromSequent(const std::vector<std::string>&, Node*, bool);
public:
    SearchTree(Sequent&);
    ~SearchTree();

    void findTransformations(Node*);
    Node* getRootPtr();
};


