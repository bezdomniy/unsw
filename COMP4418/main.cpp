#include <iostream>
#include "rule.h"
#include "sequent.h"
#include "utils.h"

int main(int, char**) {


	Sequent s("[abc,(dicks imp boobs) imp (ass)] seq [def,(omg)]");

    std::vector<std::string> left = s.getLeft();

    std::string formulaType = Utils::getFormulaRuleType(left[1]);
    
    Rule r(formulaType);

    r.tranform(s, true, 1);
}
