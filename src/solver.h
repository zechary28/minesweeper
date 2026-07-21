#pragma once

#include <utility>
#include <vector>
#include <random>

class Minesweeper;

enum class SelectionMode { RowMajor, Random, NearestToLast };

class Solver
{
public:
    Solver(SelectionMode mode = SelectionMode::RowMajor);
    void Step(Minesweeper& game);

private:
    SelectionMode mode;
    std::pair<int,int> last = {0, 0}; 
    mutable std::mt19937 rng{std::random_device{}()};

    std::vector<std::pair<int,int>> getCandidates() const;
    bool tryCell(int x, int y);
    bool isBorderCell(int x, int y) const;
    bool isSolvableCell(int x, int y) const;
    bool passRule1(int x, int y, int numFlagged, int numUnrevealed) const;
    bool passRule2(int x, int y, int numFlagged, int numUnrevealed) const;
    const std::pair<int, int> getNeighbourStats(int x, int y) const;
};