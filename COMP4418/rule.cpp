#include "rule.h"

#include <iostream>

Rule::Rule(const std::string ruleType)
{
    this->s_mapRuleTypes["and"] = rAnd;
    this->s_mapRuleTypes["or"] = rOr;
    this->s_mapRuleTypes["neg"] = rNeg;
    this->s_mapRuleTypes["imp"] = rImp;
    this->s_mapRuleTypes["iff"] = rIff;
    
}

Rule::~Rule()
{
}

bool Rule::checkMatch(const std::string formula) 
{
    return boost::regex_match(formula, this->pattern);
}

Sequent Rule::tranform(Sequent& s, bool left, unsigned int formulaIndex)
{
    std::vector<std::string> formulas;
    if (left) {
        formulas = s.getLeft();
    }
    else {
        formulas = s.getRight();
    }

    std::string formula = formulas[formulaIndex];
    std::string formulaType = Utils::getFormulaRuleType(formula);
    boost::regex pattern("(" + formulaType + ")(?![^()]*(?:\\([^()]*\\))?\\))");

    std::vector<std::string> tokens;
    Utils::splitIntoVector(formula, tokens, pattern);

    Sequent out;

    for (auto& s: tokens) {
        std::cout << s << std::endl;
    }
    std::cout << std::endl;

    switch(s_mapRuleTypes[formulaType]) {
    case rAnd: 
        std::cout << '1'; 
        break;    
    case rOr : 
        std::cout << '2';
        break;
    case rNeg: 
        std::cout << '3'; 
        break;    
    case rImp : 
        std::cout << '4' << std::endl;
        break;
    case rIff: 
        std::cout << '5'; 
        break;    
    default : 
        std::cout << '6';
        break;
    }

    // std::cout << out << std::endl;

    return out;

}