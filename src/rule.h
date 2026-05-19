#pragma once

#include <functional>
#include "tree_symbol.h"

class Tree;

struct Rule
{
    Symbol predecessor;
    float weight;   // relative probability
    std::function<void(Tree&)> action;
};
