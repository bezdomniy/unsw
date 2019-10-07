#pragma once
#include <string>
#include <vector>
#include <utility>
#include <optional>
#include <memory>
#include "rule.h"
#include "sequent.h"

struct Node
{
    std::shared_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children;
    std::pair<Sequent, std::optional<Sequent>> data;
};

class SearchTree
{
private:
    std::shared_ptr<Node> rootNode;

    void addChild(const std::shared_ptr<Node>&, const std::shared_ptr<Node>&);
    void childrenFromSequent(const std::vector<std::string>&, const std::shared_ptr<Node>&, bool);
public:
    SearchTree(Sequent&);
    ~SearchTree();

    void findTransformations(const std::shared_ptr<Node>&);
    const std::shared_ptr<Node>& getRootPtr();
};


