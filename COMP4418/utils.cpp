#include "utils.h"

void Utils::splitIntoVector(const std::string& str, std::vector<std::string>& vec, const std::string& delim)
{
    std::size_t current, previous = 0;
    current = str.find(delim);

    while (current != std::string::npos) {
        vec.push_back(str.substr(previous, current - previous));
        previous = current + delim.length();
        current = str.find(delim, previous);
    }
    vec.push_back(str.substr(previous, current - previous));


    //     std::string leftString = sequentString.substr(1, seqPos - 1);
    // std::string rightString = sequentString.substr(seqPos + 4, sequentString.length() - seqPos - 4 - 1);
}

void Utils::splitIntoVector(const std::string& str, std::vector<std::string>& vec, boost::regex& regex)
{
    
    boost::sregex_token_iterator it(str.begin(), str.end(), regex, -1);
    boost::sregex_token_iterator end;

    while (it != end) {
        vec.push_back(*it++);
    }

}

std::string Utils::getFormulaRuleType(const std::string& formula)
{
    std::string patternString = "(and|imp|or|iff|neg)(?![^()]*(?:\\([^()]*\\))?\\))"; // regex for (formulas) ruleType (formulas)
    // except for neg, which is ruleType(formulas)

    boost::regex pattern(patternString);
    boost::smatch match;

    if (boost::regex_search(formula, match, pattern))
    {
        return std::string(match[0].first, match[0].second);
    } 
    return std::string("atom");
}

#include <iostream>
std::string Utils::removeBrackets(const std::string& str) {
    boost::regex regex("^ *\\(|\\) *$");

    // std::string out = boost::regex_replace(str, regex, "");
    // std::cout << "in: " << str << ", out: " << out << std::endl;

    return boost::regex_replace(str, regex, ""); 
}