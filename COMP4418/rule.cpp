#include "rule.h"

// Checks if formula and be decomposed according to sequent rules
bool Rule::checkMatch(const std::string formula) 
{
    std::string formulaType = Utils::getFormulaRuleType(formula);
    return formulaType.compare("atom") != 0;
}

// 
std::pair<Sequent, std::optional<Sequent>> Rule::tranform(Sequent& s, bool left, unsigned int formulaIndex)
{
    enum RuleTypes {  notDefined, 
                            rAnd, 
                            rOr, 
                            rNeg, 
                            rImp,
                            rIff,
                            rend };
                        
    std::map<std::string, RuleTypes> s_mapRuleTypes = {{"and", rAnd}, {"or", rOr}, {"neg", rNeg},
                        {"imp", rImp}, {"iff", rIff}};

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

    std::regex pattern("(" + formulaType + ")(?![^()]*(?:\\([^()]*\\))?\\))");

    std::vector<std::string> tokens;
    Utils::splitIntoVector(formula, tokens, pattern);

    std::pair<Sequent, std::optional<Sequent>>  out;
    Sequent result1;
    Sequent result2;

    switch(s_mapRuleTypes[formulaType]) {
    case rAnd: 
        if (left) {
            formulas.erase(formulas.begin() + formulaIndex);
            formulas.push_back(Utils::removeBrackets(tokens[0]));
            formulas.push_back(Utils::removeBrackets(tokens[1]));

            result1 = Sequent(formulas, otherFormulas, "P3B");

            out.first = result1;
        }
        else {
            formulas.erase(formulas.begin() + formulaIndex);
            formulas.push_back(Utils::removeBrackets(tokens[0]));

            result1 = Sequent(otherFormulas, formulas, "P3A");

            formulas.pop_back();
            formulas.push_back(Utils::removeBrackets(tokens[1]));

            result2 = Sequent(otherFormulas, formulas, "P3A");

            out.first = result1;
            out.second = result2;
        }
        break;    
    case rOr : 
        if (left) {
            formulas.erase(formulas.begin() + formulaIndex);
            formulas.push_back(Utils::removeBrackets(tokens[0]));

            result1 = Sequent(formulas, otherFormulas, "P4B");

            formulas.pop_back();
            formulas.push_back(Utils::removeBrackets(tokens[1]));

            result2 = Sequent(formulas, otherFormulas, "P4B");

            out.first = result1;
            out.second = result2;
        }
        else {
            formulas.erase(formulas.begin() + formulaIndex);
            formulas.push_back(Utils::removeBrackets(tokens[0]));
            formulas.push_back(Utils::removeBrackets(tokens[1]));

            result1 = Sequent(otherFormulas, formulas, "P4A");

            out.first = result1;
        }
        break;
    case rNeg: 
        formulas.erase(formulas.begin() + formulaIndex);
        otherFormulas.push_back(Utils::removeBrackets(tokens[1]));

        if (left) {
            result1 = Sequent(formulas, otherFormulas, "P2B");
        }
        else {
            result1 = Sequent(otherFormulas, formulas, "P2A");
        }

        out.first = result1;
        
        break;    
    case rImp : 
        if (left) {
            formulas.erase(formulas.begin() + formulaIndex);
            formulas.push_back(Utils::removeBrackets(tokens[1]));

            result1 = Sequent(formulas, otherFormulas, "P5B");

            formulas.pop_back();
            otherFormulas.push_back(Utils::removeBrackets(tokens[0]));

            result2 = Sequent(formulas, otherFormulas, "P5B");

            out.first = result1;
            out.second = result2;
        }
        else {
            formulas.erase(formulas.begin() + formulaIndex);
            formulas.push_back(Utils::removeBrackets(tokens[1]));
            otherFormulas.push_back(Utils::removeBrackets(tokens[0]));

            result1 = Sequent(otherFormulas, formulas, "P5A");

            out.first = result1;
        }
        break;
    case rIff: 
        if (left) {
            formulas.erase(formulas.begin() + formulaIndex);
            formulas.push_back(Utils::removeBrackets(tokens[0]));
            formulas.push_back(Utils::removeBrackets(tokens[1]));

            result1 = Sequent(formulas, otherFormulas, "P6B");

            formulas.pop_back();
            formulas.pop_back();

            otherFormulas.push_back(Utils::removeBrackets(tokens[0]));
            otherFormulas.push_back(Utils::removeBrackets(tokens[1]));

            result2 = Sequent(formulas, otherFormulas, "P6B");

            out.first = result1;
            out.second = result2;
        }
        else {
            formulas.erase(formulas.begin() + formulaIndex);
            formulas.push_back(Utils::removeBrackets(tokens[0]));
            otherFormulas.push_back(Utils::removeBrackets(tokens[1]));

            result1 = Sequent(otherFormulas, formulas, "P6A");

            formulas.pop_back();
            otherFormulas.pop_back();

            otherFormulas.push_back(Utils::removeBrackets(tokens[0]));
            formulas.push_back(Utils::removeBrackets(tokens[1]));

            result2 = Sequent(otherFormulas, formulas, "P6A");

            out.first = result1;
            out.second = result2;
        }
        break;    
    default : 
        break;
    }

    return out;

}