#include <iostream>
#include "rule.h"
#include "sequent.h"
#include "utils.h"
#include "searchTree.h"

int main(int, char**) {

    // TODO dedup outputs
    
    // std::string input = "[] seq [(neg p) or p]"; 
    // TODO seems to be removing brackets on neg when splitting e.g. (neg a) or (neg b)

    // std::string input = "[p imp q,(neg r) imp (neg q) ] seq [p imp r]";
    // std::string input = "[p] seq [(neg p)]";
    // std::string input = "[neg (p or q)] seq [neg p]"; // TODO not working with brackets, only without - seems fine.
    std::string input = "[p iff q] seq [(q iff r) imp (p iff r)]"; 

	// Sequent s("[def,(omg)] seq [abc,(dicks imp boobs) imp (ass)]");
    // Sequent s("[λ, φ imp ψ, neg(ρ)] seq [π, neg(x)]");
    // Sequent s("[a, b iff c, d] seq [e]");

    // Sequent s("[neg(e)] seq [a, b iff c, d]");

    // Sequent s("[neg(p or q)] seq [neg(p)]");
    // Sequent s("[p] seq [q imp p]");

    // Sequent s("[p imp q,(neg r) imp (neg q) ] seq [p imp r]"); // neg seems to work - need to test more

    // Sequent s("[p iff e] seq [(q iff r) imp (p iff r)]");

    Sequent s(input); 
    std::vector<std::string> atoms = Utils::extractAtoms(input);

    // for (auto& c: v) {
    //     std::cout << c << ", ";
    // }
    // std::cout << std::endl;

    SearchTree searchTree(s);
    std::vector<std::shared_ptr<Node>>* results = new std::vector<std::shared_ptr<Node>>();

    bool* resultFound = new bool(false);
    searchTree.findTransformations(searchTree.getRootPtr(), atoms, results, resultFound);

    if (!results->empty()) {
        std::cout << "true" << std::endl;
        std::shared_ptr<Node> currentNode = results->at(0);

        std::string rule = "P1";

        while (currentNode) {
            // currentNode->data.first.setRule("P1");
            currentNode->data.first.print(rule);
            rule = currentNode->data.first.rule;
            currentNode = currentNode->parent;
        }
        std::cout << "QED" << std::endl;
    }
    else {
        std::cout << "false" << std::endl;
    }

    // for (auto& nodePtr: *results) {
    //     nodePtr->data.first.print();

    //     if (nodePtr->data.second.has_value()) {
    //         nodePtr->data.second.value().print();
    //     }
    //     std::cout << std::endl;
    // }

    delete results;
    delete resultFound;
}
