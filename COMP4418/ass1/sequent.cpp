#include "sequent.h"

Sequent::Sequent()
{
    
}

Sequent::Sequent(const std::string sequentString)
{
    parse(sequentString);
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

void Sequent::parse(const std::string sequentString)
{
    std::string seq = "seq";

    std::size_t current, previous = 0;
    std::size_t seqPos = sequentString.find(seq);

    std::string leftString = sequentString.substr(1, seqPos - 3);
    std::string rightString = sequentString.substr(seqPos + 5, sequentString.length() - seqPos - 4 - 2);

    Utils::splitIntoVector(leftString, this->leftSide, ",");
    Utils::splitIntoVector(rightString, this->rightSide, ",");
}

void Sequent::setRule(const std::string& r) {
    this->rule = r;
}

int Sequent::print(const std::string& r,std::optional<Sequent>& other, int counter, bool fromDoubleRule, bool secondary) {

    counter++;
    bool atomic = true;

    // Check if sequent is comprised entirely of atomic formulas (no neg, or, and, iff, imp)
    for (auto& formula: this->leftSide) {
        if (Utils::getFormulaRuleType(formula) != "atom") {
            atomic = false;
            break;
        }
    }
    if (atomic) {
        for (auto& formula: this->rightSide) {
            if (Utils::getFormulaRuleType(formula) != "atom") {
                atomic = false;
                break;
            }
        }
    }

    std::cout << counter << ". [ ";

    std::sort( leftSide.begin(), leftSide.end() );
    leftSide.erase( std::unique( leftSide.begin(), leftSide.end() ), leftSide.end() );

    std::sort( rightSide.begin(), rightSide.end() );
    rightSide.erase( std::unique( rightSide.begin(), rightSide.end() ), rightSide.end() );


    for (int i = 0; i < leftSide.size(); i++) {
        std::cout << leftSide.at(i);
        if (i < leftSide.size() - 1) std::cout << ", ";
    }
    std::cout << " ] seq [ ";
    for (int i = 0; i < rightSide.size(); i++) {
        std::cout << rightSide.at(i);
        if (i < rightSide.size() - 1) std::cout << ", ";
    }

    if (atomic) {
        std::cout<< " ] - Rule P1";
    } else {
        std::cout<< " ] - Rule " << r;
    }
    

    if (r == "P1" || atomic) {
        std::cout << std::endl;
    }
    else if (fromDoubleRule) {
        if (secondary) {
            std::cout << " - from: " << counter - 3 << ". and " << counter - 2 << ". " << std::endl;
        } else {
            std::cout << " - from: " << counter - 2 << ". and " << counter - 1 << ". " << std::endl;
        }
    }
    else {
        if (secondary) {
            std::cout << " - from: " << counter - 2 << ". " << std::endl;
        }
        else {
            std::cout << " - from: " << counter - 1 << ". " << std::endl;
        }
        
    }

    if (other.has_value()) {
        std::optional<Sequent> empty;
        counter = other.value().print(r, empty, counter, fromDoubleRule, true);
        
    }

    return counter;
    
}

std::vector<std::string>& Sequent::getLeft()
{
    return this->leftSide;
}

std::vector<std::string>& Sequent::getRight() {
    return this->rightSide;
}