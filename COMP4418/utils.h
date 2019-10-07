#pragma once
#include <string>
#include <vector>
#include <boost/regex.hpp>


namespace Utils {
    void splitIntoVector(const std::string& str, std::vector<std::string>& vec, const std::string& delim);
    void splitIntoVector(const std::string& str, std::vector<std::string>& vec, boost::regex& regex);
    std::string getFormulaRuleType(const std::string& str);
    std::string removeBrackets(const std::string& str);
}
