#include "solver.h"
#include "minesweeper.h"
#include <algorithm>
#include <iostream>

Solver::Solver(Minesweeper& game, SelectionMode mode)
    : game(game), mode(mode), last({0, 0})
{
}

std::vector<std::pair<int,int>> Solver::getCandidates() const
{
    int width = game.getWidth();
    int height = game.getHeight();
    std::vector<std::pair<int,int>> candidates;

    // min-heap: (distance, {x, y})
    using Entry = std::pair<int, std::pair<int,int>>;
    std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> minHeap;

    switch (mode)
    {
        case SelectionMode::RowMajor:
            // already in order, nothing to do
            // populate in row-major order
            for (int y = 0; y < height; y++)
            for (int x = 0; x < width; x++)
                candidates.emplace_back(x, y);
            break;

        case SelectionMode::Random:
            for (int y = 0; y < height; y++)
            for (int x = 0; x < width; x++)
                candidates.emplace_back(x, y);
            std::shuffle(candidates.begin(), candidates.end(), rng);
            break;

        case SelectionMode::NearestToLast:
            for (int y = 0; y < height; y++)
            for (int x = 0; x < width; x++)
            {
                int dx = x - last.first;
                int dy = y - last.second;
                int dist = dx*dx + dy*dy;
                minHeap.push({dist, {x, y}});
            }

            // drain heap into ordered vector
            candidates.reserve(width * height);
            while (!minHeap.empty())
            {
                candidates.push_back(minHeap.top().second);
                minHeap.pop();
            }
    }

    return candidates;
}

// Called once per solver tick from main.cpp
// Make one logical move: flag a mine, or reveal a safe cell
void Solver::Step()
{
    auto candidates = getCandidates();
    bool success = false;
    for (auto p : candidates)
    {
        if (tryCell(p.first, p.second))
        {
            last = {p.first, p.second};
            success = true;
            return;
        }
    }
    //if (!success) game.ResetMap();
}

// does the necessary checks and actions
bool Solver::tryCell(int x, int y)
{
    if (!isBorderCell(x, y)) return false;

    // gather unrevealed and flagged neighbours
    auto stats = getNeighbourStats(x, y);
    int numFlagged = stats.first;
    int numUnrevealed = stats.second;

    // rule 1: if flagged neighbours == adjacentMines, reveal the rest
    if (passRule1(x, y, numFlagged, numUnrevealed)) 
    {
        game.RevealCell(x, y); // game will chord cell
        return true;
    }

    // rule 2: if unrevealed neighbours == adjacentMines - flagged, flag all
    if (passRule2(x, y, numFlagged, numUnrevealed))
    {
        for (auto p : game.getNeighbours(x, y)) 
        {
            Cell neighbourCell = game.getCell(p.first, p.second);
            if (!neighbourCell.flagged && !neighbourCell.revealed) game.FlagCell(p.first, p.second);
        }
        return true;
    }

    return false;
}

// checks that the cell is along the border of the solved area
// is called by Solver::tryCell()
bool Solver::isBorderCell(int x, int y) const
{
    const Cell& cell = game.getCell(x, y);

    // valid center cell
    if (cell.revealed && cell.adjacentMines != 0) 
    {
        // check neighbours if border
        for (auto p : game.getNeighbours(x, y))
        {
            Cell neighbourCell = game.getCell(p.first, p.second);
            if (!neighbourCell.revealed && !neighbourCell.flagged) return true;
        }
    }
    return false;
}

// checks that the cell is solvable, ie passes the two deterministic rules
// 1. if flagged neighbours == adjacentMines              => reveal the rest
// 2. if unrevealed neighbours == adjacentMines - flagged => flag them all
bool Solver::isSolvableCell(int x, int y) const
{
    // gather unrevealed and flagged neighbours
    auto stats = getNeighbourStats(x, y);
    int numFlagged = stats.first;
    int numUnrevealed = stats.second;

    return (passRule1(x, y, numFlagged, numUnrevealed) || 
            passRule2(x, y, numFlagged, numUnrevealed));
}

// rule 1: if flagged neighbours == adjacentMines, reveal the rest
bool Solver::passRule1(int x, int y, int numFlagged, int numUnrevealed) const
{
    const Cell& cell = game.getCell(x, y);
    return numFlagged == cell.adjacentMines;
}

// rule 2: if unrevealed neighbours == adjacentMines - flagged, flag them all
bool Solver::passRule2(int x, int y, int numFlagged, int numUnrevealed) const
{
    const Cell& cell = game.getCell(x, y);
    return numUnrevealed == cell.adjacentMines - numFlagged;
}

const std::pair<int, int> Solver::getNeighbourStats(int x, int y) const
{
    int numFlagged = 0;
    int numUnrevealed = 0;
    for (auto p : game.getNeighbours(x, y))
    {
        Cell neighbourCell = game.getCell(p.first, p.second);
        numFlagged += neighbourCell.flagged;
        numUnrevealed += !neighbourCell.revealed && !neighbourCell.flagged;
    }
    return {numFlagged, numUnrevealed};
}
