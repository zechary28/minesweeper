#pragma once

#include <raylib.h>
#include <vector>
#include <cstdint>
#include <memory>

#include "tree_symbol.h"

enum class GrowthState : uint8_t
{
    Active,
    Dormant
};

class RuleSet;

class Tree
{
public:
    Tree(Vector2 root, 
         float length, 
         float rotation, 
         int rank, 
         Symbol symbol, 
         Tree* parent);
    Tree& AddChild(Vector2 root, 
                   float length, 
                   float rotation, 
                   Symbol symbol);
    void collectGrowable(std::vector<Tree*>& out);
    void Grow(RuleSet& rules);
    Tree* FindNearest(Vector2 mouse, float& best);
    void Prune();
    void PruneSelf();
    void Draw() const;

    bool isLeaf() const;
    bool canGrow() const;

    Vector2 root;
    float length;
    float rotation;
    int rank;
    Symbol symbol;
    GrowthState state;
    Tree* parent;
    std::vector<std::unique_ptr<Tree>> branches;
};
