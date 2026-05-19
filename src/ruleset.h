#pragma once

#include <vector>
#include "rule.h"

class Tree;

class RuleSet
{
public:
    void AddRule(const Rule& rule);

    // Returns true if a rule was applied
    bool Apply(Tree& node);

private:
    std::vector<Rule> rules;
};
