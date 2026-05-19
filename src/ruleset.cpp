#include "ruleset.h"
#include "tree.h"
#include <raylib.h>

void RuleSet::AddRule(const Rule& rule)
{
    rules.push_back(rule);
}

bool RuleSet::Apply(Tree& node)
{
    std::vector<const Rule*> candidates;
    float totalWeight = 0.0f;

    for (const auto& r : rules)
    {
        if (r.predecessor == node.symbol)
        {
            candidates.push_back(&r);
            totalWeight += r.weight;
        }
    }

    if (candidates.empty())
        return false;

    float pick = GetRandomValue(0, 10000) / 10000.0f * totalWeight;
    float accum = 0.0f;

    for (const Rule* r : candidates)
    {
        accum += r->weight;
        if (pick <= accum)
        {
            r->action(node);
            return true;
        }
    }

    return false;
}
