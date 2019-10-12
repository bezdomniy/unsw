#include <iostream>
#include "rule.h"
#include "sequent.h"
#include "utils.h"
#include "searchTree.h"

int main(int argc, char** argv) {

    std::string input;
    if (argc == 2) {
        input = argv[1];
    } else {
        std::cout << "Incorrect number of arguements." << std::endl;
        exit(0);
    }

    Sequent s(input); 
    // std::vector<std::string> atoms = Utils::extractAtoms(input);

    SearchTree searchTree(s);
    std::vector<std::shared_ptr<Node>>* results = new std::vector<std::shared_ptr<Node>>();

    bool* resultFound = new bool(false);
    searchTree.findTransformations(searchTree.getRootPtr(), results, resultFound);

    if (!results->empty()) {
        std::cout << "true" << std::endl;
        std::shared_ptr<Node> currentNode = results->at(0);

        std::string rule = "P1";
        int counter = 0;
        bool fromDoubleRule = false;

        while (currentNode) {
            counter = currentNode->data.first.print(rule, currentNode->data.second, counter, fromDoubleRule, false);
            rule = currentNode->data.first.rule;
            fromDoubleRule = currentNode->data.second.has_value();
            currentNode = currentNode->parent;
        }
        std::cout << "QED" << std::endl;
    }
    else {
        std::cout << "false" << std::endl;
    }

    delete results;
    delete resultFound;
}
