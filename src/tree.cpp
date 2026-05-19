#include "tree.h"
#include "ruleset.h"
#include <raylib.h>
#include <raymath.h>

#include <cstdint>
#include <cmath>
#include <algorithm>

Tree::Tree(Vector2 root, 
           float length, 
           float rotation, 
           int rank, 
           Symbol symbol, 
           Tree* parent) 
    : root(root),
      length(length), 
      rotation(rotation), 
      rank(rank), 
      symbol(symbol), 
      state(GrowthState::Active), 
      parent(parent)
{
}

Tree& Tree::AddChild(Vector2 root,
                     float length,
                     float rotation,
                     Symbol symbol)
{
    branches.push_back(
        std::make_unique<Tree>(
            root,
            length,
            rotation,
            rank + 1,
            symbol,
            this
        )
    );
    return *branches.back();
}

static Vector2 GetEnd(Vector2 start, float length, float rotation)
{
    float rad = rotation * DEG2RAD;
    return {start.x + length * std::cos(rad), start.y + length * std::sin(rad)}; 
}

void Tree::collectGrowable(std::vector<Tree*>& out)
{
    if (canGrow())
        out.push_back(this);

    for (auto& b : branches)
        b->collectGrowable(out);
}

void Tree::Grow(RuleSet& rules)
{
    if (!canGrow())
        return;

    // Try to apply a rule to THIS node
    if (rules.Apply(*this))
        return;

    // Otherwise, recurse into children (one path only)
    if (!branches.empty())
    {
        int i = GetRandomValue(0, branches.size() - 1);
        branches[i]->Grow(rules);
    }
}

Tree* Tree::FindNearest(Vector2 mouse, float& bestDist)
{
    Tree* best = nullptr;

    Vector2 end = GetEnd(root, length, rotation);
    float d = Vector2Distance(mouse, end);

    if (d < bestDist)
    {
        bestDist = d;
        best = this;
    }

    for (const std::unique_ptr<Tree>& b : branches)
    {
        if (Tree* candidate = b->FindNearest(mouse, bestDist))
            best = candidate;
    }

    return best;
}

void Tree::Prune()
{
    branches.clear();
    symbol = Symbol::Leaf;
}

void Tree::PruneSelf()
{
    if (!parent) return;

    auto& siblings = parent->branches;

    siblings.erase(
        std::remove_if(
            siblings.begin(),
            siblings.end(),
            [&](const std::unique_ptr<Tree>& t)
            {
                return t.get() == this;
            }
        ),
        siblings.end()
    );
}


bool Tree::isLeaf() const
{
    return branches.empty();
}

bool Tree::canGrow() const
{
    if (state != GrowthState::Active)
        return false;

    if (branches.size() >= 2)
        return false;

    return true;
}

void Tree::Draw() const
{
    Vector2 end = GetEnd(root, length, rotation);
        DrawLineEx(root, end, length/4, BROWN);
    if (isLeaf())
    {
        DrawCircleV(end, 5, {20, 200, 133, 255});
    }
    for (const std::unique_ptr<Tree>& branch : branches)
    {
        branch->Draw();
    }
}