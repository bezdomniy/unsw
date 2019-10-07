#include <iostream>
#include "rule.h"
#include "sequent.h"
#include "utils.h"
#include "searchTree.h"

int main(int, char**) {

	// Sequent s("[def,(omg)] seq [abc,(dicks imp boobs) imp (ass)]");
    // Sequent s("[λ, φ imp ψ, neg(ρ)] seq [π, neg(x)]");
    // Sequent s("[a, b iff c, d] seq [e]");

    Sequent s("[neg(e)] seq [a, b iff c, d]");

    SearchTree searchTree(s);
    searchTree.findTransformations(searchTree.getRootPtr());

    // std::vector<std::string> left = s.getLeft();
    // std::pair<Sequent, std::optional<Sequent>> out = Rule::tranform(s, false, 1);

    // out.first.print();
    // if (out.second.has_value())
    //     out.second.value().print();
}
