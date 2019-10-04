#pragma once
#include <regex>
#include "sequent.h"

class Rule
{
private:
    std::string type; // neg, and, or, imp, iff
    std::regex pattern;

public:
    Rule(const std::string);
    ~Rule(); 

    bool checkMatch(const std::string);
    std::string tranform(Sequent&, bool);
};


