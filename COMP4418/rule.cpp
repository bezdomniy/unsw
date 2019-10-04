#include "rule.h"

Rule::Rule(const std::string ruleType)
{
    this->type = ruleType;

    std::string patternString = ""; // regex for (formulas) ruleType (formulas)

    this->pattern = std::regex(patternString);
    
}

Rule::~Rule()
{
}

bool Rule::checkMatch(const std::string formula) 
{
    return std::regex_match(formula, this->pattern);
}

std::string Rule::tranform(Sequent& sequent, bool transformLeft)
{

}