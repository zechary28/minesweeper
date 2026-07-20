#include "solver.h"
#include "minesweeper.h"

#include <algorithm>
#include <iostream>

Solver::Solver(SelectionMode mode)
    : mode(mode), last({0, 0})
{
}

std::vector<std::pair<int,int>> Solver::getCandidates(Minesweeper& game) const
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
void Solver::Step(Minesweeper& game)
{
    auto candidates = getCandidates(game);
    bool success = false;
    for (auto p : candidates)
    {
        if (tryCell(game, p.first, p.second))
        {
            last = {p.first, p.second};
            success = true;
            return;
        }
    }
    // if (!success) // if no solvable tile, open all tiles
    // {
    //     for (auto p : candidates)
    //     {
    //         game.RevealCell(p.first, p.second);
    //     }
    // }
}

bool Solver::tryCell(Minesweeper& game, int x, int y)
{
    const Cell& cell = game.getCell(x, y);

    // valid center cell
    bool found = false;
    if (cell.revealed && cell.adjacentMines != 0) 
    {
        // check neighbours if border
        for (auto p : game.getNeighbours(x, y))
        {
            Cell neighbourCell = game.getCell(p.first, p.second);
            if (!neighbourCell.revealed && !neighbourCell.flagged) found = true;
        }
    }
    if (!found) return false;
    //std::cout << "borderCell: (" << x << ", " << y << ")\n";

    // gather unrevealed and flagged neighbours
    auto stats = getNeighbourStats(game, x, y);
    int numFlagged = stats.first;
    int numUnrevealed = stats.second;
    //std::cout << "numFlagged:    " << numFlagged << "\n";
    //std::cout << "numUnrevealed: " << numUnrevealed << "\n";

    // if flagged neighbours == adjacentMines, reveal the rest
    if (numFlagged == cell.adjacentMines) 
    {
        game.RevealCell(x, y);
        //std::cout << "revealed cell\n";
        return true;
    }
    // if unrevealed neighbours == adjacentMines - flagged, flag them all
    else if (numUnrevealed == cell.adjacentMines - numFlagged)
    {
        for (auto p : game.getNeighbours(x, y)) 
        {
            Cell neighbourCell = game.getCell(p.first, p.second);
            if (!neighbourCell.flagged && !neighbourCell.revealed) game.FlagCell(p.first, p.second);
        }
        //std::cout << "Flagged cells\n";
        return true;
    }
    return false;
}

const std::pair<int, int> Solver::getNeighbourStats(Minesweeper& game, int x, int y) const
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
