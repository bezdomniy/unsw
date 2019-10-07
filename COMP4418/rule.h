#pragma once
// #include <regex>
#include <boost/regex.hpp>
#include <map>
#include <algorithm>
#include <utility>
#include <optional>
#include "sequent.h"

class Rule
{
private:
    std::string type; // neg, and, or, imp, iff
    boost::regex pattern;

    enum RuleTypes { notDefined, 
                          rAnd, 
                          rOr, 
                          rNeg, 
                          rImp,
                          rIff,
                          rend };
    std::map<std::string, RuleTypes> s_mapRuleTypes;

public:
    Rule();
    ~Rule();

    bool checkMatch(const std::string);
    std::pair<Sequent, std::optional<Sequent>> tranform(Sequent&, bool, unsigned int);
};


