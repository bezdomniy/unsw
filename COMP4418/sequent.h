#pragma once
#include <string>
#include <iostream>
#include "utils.h"

class Sequent
{
private:
    std::vector<std::string> leftSide;
    std::vector<std::string> rightSide;
    void parse(const std::string);

public:
    std::string rule = "";
    Sequent();
    Sequent(const std::string);
    Sequent(std::vector<std::string>, std::vector<std::string>, std::string);
    ~Sequent();

    void print(const std::string&);

    std::vector<std::string>& getLeft();
    std::vector<std::string>& getRight();

    void setRule(const std::string&);
};


