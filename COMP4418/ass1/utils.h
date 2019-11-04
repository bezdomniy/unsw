#pragma once
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <algorithm>


namespace Utils {
    void splitIntoVector(const std::string& str, std::vector<std::string>& vec, const std::string& delim);
    void splitIntoVector(const std::string& str, std::vector<std::string>& vec, std::regex& regex);
    std::string getFormulaRuleType(const std::string& str);
    std::string removeBrackets(const std::string& str);
    std::vector<std::string> extractAtoms(const std::string& str);
    bool checkFirstRule(std::vector<std::string>& left, std::vector<std::string>& right);

    std::string ltrim(const std::string& s);

    std::string rtrim(const std::string& s);

    std::string trim(const std::string& s);
}
