#include "sequent.h"

Sequent::Sequent()
{
    
}

Sequent::Sequent(const std::string sequentString)
{
    parse(sequentString);
}

Sequent::Sequent(std::vector<std::string> left, std::vector<std::string> right)
{
    leftSide = left;
    rightSide = right;
}

Sequent::~Sequent()
{
}

void Sequent::parse(const std::string sequentString)
{
    std::string seq = "seq";

    std::size_t current, previous = 0;
    std::size_t seqPos = sequentString.find(seq);

    std::string leftString = sequentString.substr(1, seqPos - 3);
    std::string rightString = sequentString.substr(seqPos + 5, sequentString.length() - seqPos - 4 - 2);

    // std::cout << leftString << std::endl << rightString << std::endl;

    Utils::splitIntoVector(leftString, this->leftSide, ",");
    Utils::splitIntoVector(rightString, this->rightSide, ",");

    // for (auto& s: leftSide) {
    //     std::cout << s << std::endl;
    // }
    // std::cout << std::endl;



}

// void Sequent::splitIntoVector(const std::string& str, std::vector<std::string>& vec, const std::string& delim)
// {
//     std::size_t current, previous = 0;
//     current = str.find(delim);

//     while (current != std::string::npos) {
//         vec.push_back(str.substr(previous, current - previous));
//         previous = current + 1;
//         current = str.find(delim, previous);
//     }
//     vec.push_back(str.substr(previous, current - previous));
// }

void Sequent::print() {
    std::cout << "[ ";
    for (auto& s: leftSide) {
        std::cout << s << ", ";
    }
    std::cout << "] seq [ ";
    for (auto& s: rightSide) {
        std::cout << s << ", ";
    }
    std::cout<< "]" << std::endl;
}

std::vector<std::string>& Sequent::getLeft()
{
    return this->leftSide;
}

std::vector<std::string>& Sequent::getRight() {
    return this->rightSide;
}