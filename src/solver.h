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

    std::vector<std::pair<int,int>> getCandidates(Minesweeper& game) const;
    bool tryCell(Minesweeper& game, int x, int y);
    const std::pair<int, int> getNeighbourStats(Minesweeper& game, int x, int y) const;
};