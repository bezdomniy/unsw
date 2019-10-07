#pragma once
// #include <regex>
#include <boost/regex.hpp>
#include <map>
#include <algorithm>
#include <utility>
#include <optional>
#include "sequent.h"

namespace Rule
{
    bool checkMatch(const std::string);
    std::pair<Sequent, std::optional<Sequent>> tranform(Sequent&, bool, unsigned int);
};

