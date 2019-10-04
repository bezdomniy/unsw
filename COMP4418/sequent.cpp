#include "sequent.h"

#include <iostream>

Sequent::Sequent(const std::string sequentString)
{
    parse(sequentString);
}

Sequent::~Sequent()
{
}

void Sequent::parse(const std::string sequentString)
{
    std::string seq = "seq";

    std::size_t current, previous = 0;
    std::size_t seqPos = sequentString.find(seq);

    std::string leftString = sequentString.substr(1, seqPos - 1);
    std::string rightString = sequentString.substr(seqPos + 4, sequentString.length() - seqPos - 4 - 1);

    // std::cout << leftString << std::endl << rightString << std::endl;

    splitIntoVector(leftString, this->leftSide, ",");
    splitIntoVector(rightString, this->rightSide, ",");

    // for (auto& s: leftSide) {
    //     std::cout << s << std::endl;
    // }
    // std::cout << std::endl;



}

void Sequent::splitIntoVector(const std::string& str, std::vector<std::string>& vec, const std::string& delim)
{
    std::size_t current, previous = 0;
    current = str.find(delim);

    while (current != std::string::npos) {
        vec.push_back(str.substr(previous, current - previous));
        previous = current + 1;
        current = str.find(delim, previous);
    }
    vec.push_back(str.substr(previous, current - previous));
}