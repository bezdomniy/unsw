#pragma once
#include <string>
#include <iostream>
#include "utils.h"

class Sequent
{
private:
    std::vector<std::string> leftSide;
    std::vector<std::string> rightSide;

    

    // std::vector<std::string> atoms;

    void parse(const std::string);
    // void splitIntoVector(const std::string&, std::vector<std::string>&, const std::string&);

public:
    std::string rule = "";
    Sequent();
    Sequent(const std::string);
    Sequent(std::vector<std::string>, std::vector<std::string>, std::string);
    // const std::vector<std::string>& getAtoms();
    ~Sequent();

    void print(const std::string&);

    std::vector<std::string>& getLeft();
    std::vector<std::string>& getRight();

    void setRule(const std::string&);


};


