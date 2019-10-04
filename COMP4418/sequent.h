#pragma once
#include <string>
#include <vector>

class Sequent
{
private:
    std::vector<std::string> leftSide;
    std::vector<std::string> rightSide;

    void parse(const std::string);
    void splitIntoVector(const std::string&, std::vector<std::string>&, const std::string&);

public:
    Sequent(const std::string);
    ~Sequent();


};


