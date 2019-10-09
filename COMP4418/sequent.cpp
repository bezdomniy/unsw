#include "sequent.h"

Sequent::Sequent()
{
    
}

Sequent::Sequent(const std::string sequentString)
{
    parse(sequentString);
    // this->atoms = Utils::extractAtoms(sequentString);
}

Sequent::Sequent(std::vector<std::string> left, std::vector<std::string> right, std::string rule)
{
    this->leftSide = left;
    this->rightSide = right;
    this->rule = rule;
}

Sequent::~Sequent()
{
}

// const std::vector<std::string>& Sequent::getAtoms() {
//     return this->atoms;
// }

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

void Sequent::setRule(const std::string& r) {
    this->rule = r;
}

void Sequent::print(const std::string& r) {
    std::cout << "[ ";
    for (auto& s: leftSide) {
        std::cout << s << ", ";
    }
    std::cout << "] seq [ ";
    for (auto& s: rightSide) {
        std::cout << s << ", ";
    }
    std::cout<< "] - " << r << std::endl;
}

std::vector<std::string>& Sequent::getLeft()
{
    return this->leftSide;
}

std::vector<std::string>& Sequent::getRight() {
    return this->rightSide;
}