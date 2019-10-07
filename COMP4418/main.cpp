#include <iostream>
#include "rule.h"
#include "sequent.h"
#include "utils.h"

int main(int, char**) {

    // P5bRule→`:  Ifλ, ψ, ρ`πandλ, ρ`π, φ, thenλ, φ→ψ, ρ`π
	// Sequent s("[def,(omg)] seq [abc,(dicks imp boobs) imp (ass)]");

    // Sequent s("[λ, φ imp ψ, neg(ρ)] seq [π, neg(x)]");

    // Sequent s("[a, b or c, d] seq [e]");
    Sequent s("[e] seq [a, b and c, d]");

    std::vector<std::string> left = s.getLeft();
    Rule r;

    std::pair<Sequent, std::optional<Sequent>> out = r.tranform(s, false, 1);

    out.first.print();

    if (out.second.has_value())
        out.second.value().print();
}
