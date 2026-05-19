#pragma once

#include "ruleset.h"
#include "tree.h"
#include <cmath>
#include <raylib.h>

inline RuleSet CreateTreeRules()
{
    RuleSet rules;

    // Leaf → split into two leaves
    rules.AddRule({
        Symbol::Leaf,
        0.5f,
        [](Tree& n)
        {
            Vector2 end{
                n.root.x + n.length * cosf(n.rotation * DEG2RAD),
                n.root.y + n.length * sinf(n.rotation * DEG2RAD)
            };

            float offset = GetRandomValue(20, 30);
            float newLength = n.length * 0.85f;

            n.AddChild(end, newLength, n.rotation + offset, Symbol::Leaf);
            n.AddChild(end, newLength, n.rotation - offset, Symbol::Leaf);
            n.symbol = Symbol::Branch;
        }
    });

    // Leaf → extend
    rules.AddRule({
        Symbol::Leaf,
        0.5f,
        [](Tree& n)
        {
            Vector2 end{
                n.root.x + n.length * cosf(n.rotation * DEG2RAD),
                n.root.y + n.length * sinf(n.rotation * DEG2RAD)
            };
            float offset = GetRandomValue(20, 30);
            float sign   = GetRandomValue(0, 1) ? 1.0f : -1.0f;
            float newRotation = n.rotation + sign * offset;
            n.AddChild(end,
                       n.length * 0.9f,
                       newRotation,
                       Symbol::Leaf);
            n.symbol = Symbol::Branch;
        }
    });

    // Branch → add branch
    rules.AddRule({
        Symbol::Branch,
        1.0f,
        [](Tree& n)
        {
            float oldRotation = n.rotation;
            if (!n.isLeaf())
            {
                oldRotation = n.branches[0]->rotation;
            }
            Vector2 end{
                n.root.x + n.length * cosf(n.rotation * DEG2RAD),
                n.root.y + n.length * sinf(n.rotation * DEG2RAD)
            };
            float offset = 2 * GetRandomValue(20, 30);
            float sign   = GetRandomValue(0, 1) ? 1.0f : -1.0f;
            float newRotation = oldRotation + sign * offset;
            n.AddChild(end,
                       n.length * 0.9f,
                       newRotation,
                       Symbol::Leaf);
        }
    });

    return rules;
}
