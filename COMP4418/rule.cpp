#include "rule.h"

#include <iostream>

Rule::Rule()
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

std::pair<Sequent, std::optional<Sequent>> Rule::tranform(Sequent& s, bool left, unsigned int formulaIndex)
{
    std::vector<std::string> formulas;
    std::vector<std::string> otherFormulas;
    if (left) {
        formulas = s.getLeft();
        otherFormulas = s.getRight();
    }
    else {
        formulas = s.getRight();
        otherFormulas = s.getLeft();
    }

    std::string formula = formulas[formulaIndex];
    std::string formulaType = Utils::getFormulaRuleType(formula);
    boost::regex pattern("(" + formulaType + ")(?![^()]*(?:\\([^()]*\\))?\\))");

    std::vector<std::string> tokens;
    Utils::splitIntoVector(formula, tokens, pattern);

    std::pair<Sequent, std::optional<Sequent>>  out;
    Sequent result1;
    Sequent result2;

    for (auto& s: tokens) {
        std::cout << s << ", ";
    }
    std::cout << std::endl;

    switch(s_mapRuleTypes[formulaType]) {
    case rAnd: 
        if (left) {
            formulas.erase(formulas.begin() + formulaIndex);
            formulas.push_back(tokens[0]);
            formulas.push_back(tokens[1]);

            result1 = Sequent(formulas, otherFormulas);

            out.first = result1;
        }
        else {
            formulas.erase(formulas.begin() + formulaIndex);
            formulas.push_back(tokens[0]);

            result1 = Sequent(otherFormulas, formulas);

            formulas.pop_back();
            formulas.push_back(tokens[1]);

            result2 = Sequent(otherFormulas, formulas);

            out.first = result1;
            out.second = result2;
        }
        break;    
    case rOr : 
        if (left) {
            formulas.erase(formulas.begin() + formulaIndex);
            formulas.push_back(tokens[0]);

            result1 = Sequent(formulas, otherFormulas);

            formulas.pop_back();
            formulas.push_back(tokens[1]);

            result2 = Sequent(formulas, otherFormulas);

            out.first = result1;
            out.second = result2;
        }
        else {
            formulas.erase(formulas.begin() + formulaIndex);
            formulas.push_back(tokens[0]);
            formulas.push_back(tokens[1]);

            result1 = Sequent(otherFormulas, formulas);

            out.first = result1;
        }
        break;
    case rNeg: 
        formulas.erase(formulas.begin() + formulaIndex);
        otherFormulas.push_back(Utils::removeBrackets(tokens[1]));

        if (left) {
            result1 = Sequent(formulas, otherFormulas);
        }
        else {
            result1 = Sequent(otherFormulas, formulas);
        }

        out.first = result1;
        
        break;    
    case rImp : 
        if (left) {
            formulas.erase(formulas.begin() + formulaIndex);
            formulas.push_back(tokens[1]);

            result1 = Sequent(formulas, otherFormulas);

            formulas.pop_back();
            otherFormulas.push_back(tokens[0]);

            result2 = Sequent(formulas, otherFormulas);

            out.first = result1;
            out.second = result2;
        }
        else {
            formulas.erase(formulas.begin() + formulaIndex);
            formulas.push_back(tokens[1]);
            otherFormulas.push_back(tokens[0]);

            result1 = Sequent(otherFormulas, formulas);

            out.first = result1;
        }
        break;
    case rIff: 
        if (left) {
            formulas.erase(formulas.begin() + formulaIndex);
            formulas.push_back(tokens[0]);
            formulas.push_back(tokens[1]);

            result1 = Sequent(formulas, otherFormulas);

            formulas.pop_back();
            formulas.pop_back();

            otherFormulas.push_back(tokens[0]);
            otherFormulas.push_back(tokens[1]);

            result2 = Sequent(formulas, otherFormulas);

            out.first = result1;
            out.second = result2;
        }
        else {
            formulas.erase(formulas.begin() + formulaIndex);
            formulas.push_back(tokens[0]);
            otherFormulas.push_back(tokens[1]);

            result1 = Sequent(otherFormulas, formulas);

            formulas.pop_back();
            otherFormulas.pop_back();

            otherFormulas.push_back(tokens[0]);
            formulas.push_back(tokens[1]);

            result2 = Sequent(otherFormulas, formulas);

            out.first = result1;
            out.second = result2;
        }
        break;    
    default : 
        std::cout << '6';
        break;
    }

    // std::cout << out << std::endl;

    return out;

}