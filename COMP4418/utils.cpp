#include "utils.h"

void Utils::splitIntoVector(const std::string& str, std::vector<std::string>& vec, const std::string& delim)
{
    std::size_t current, previous = 0;
    current = str.find(delim);

    while (current != std::string::npos) {
        vec.push_back(Utils::trim(str.substr(previous, current - previous)));
        previous = current + delim.length();
        current = str.find(delim, previous);
    }
    vec.push_back(Utils::trim(str.substr(previous, current - previous)));
}

void Utils::splitIntoVector(const std::string& str, std::vector<std::string>& vec, std::regex& regex)
{
    
    std::sregex_token_iterator it(str.begin(), str.end(), regex, -1);
    std::sregex_token_iterator end;

    while (it != end) {
        vec.push_back(Utils::trim(*it++));
    }

}

std::string Utils::getFormulaRuleType(const std::string& formula)
{
    // regex for extracting ruleType in: "(formulas) ruleType (formulas)" or "ruleType atom"
    std::string patternString = "(and|imp|or|iff|neg)(?![^()]*(?:\\([^()]*\\))?\\))"; 

    std::regex pattern(patternString);
    std::smatch match;

    if (std::regex_search(formula, match, pattern))
    {
        return std::string(match[0].first, match[0].second);
    } 
    return std::string("atom");
}

std::string Utils::removeBrackets(const std::string& str) {
    std::regex regex("^ *\\(|\\) *$");
    return std::regex_replace(str, regex, ""); 
}

// Converts formula string into list of logical atoms
std::vector<std::string> Utils::extractAtoms(const std::string& str) {
    std::regex regex("(and|imp|or|iff|neg|seq|neg|[\\(\\)\\[\\],])");
    std::string temp = std::regex_replace(str, regex, "");

    regex = std::regex("\\s+");

    temp = std::regex_replace(temp, regex, ",");

    std::vector<std::string> vec;
    std::stringstream ss(temp);

    while (ss.good()) {
        std::string atom;
        getline( ss, atom, ',' );
        vec.push_back(atom);    
    }

    std::sort( vec.begin(), vec.end() );
    vec.erase( std::unique( vec.begin(), vec.end() ), vec.end() );

    return vec;
}

// Checks whether both sides of a sequent satisfy sequent rule P1
bool Utils::checkFirstRule(std::vector<std::string>& left, std::vector<std::string>& right) {
    std::vector<std::string> intersect;
    std::sort(left.begin(), left.end());
    std::sort(right.begin(), right.end());

    std::set_intersection(left.begin(),left.end(),
                          right.begin(),right.end(),
                          back_inserter(intersect));

    return !intersect.empty();
}

// Functions to trim leading and trailing white space.
// Reference: https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/
std::string Utils::ltrim(const std::string& s)
{
	size_t start = s.find_first_not_of(" \n\r\t\f\v");
	return (start == std::string::npos) ? "" : s.substr(start);
}

std::string Utils::rtrim(const std::string& s)
{
	size_t end = s.find_last_not_of(" \n\r\t\f\v");
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string Utils::trim(const std::string& s)
{
	return rtrim(ltrim(s));
}

