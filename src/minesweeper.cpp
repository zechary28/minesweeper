#include "minesweeper.h"
#include <raylib.h>
#include <iostream>

bool debug = false;

// CLASSIC COLOR PALETTE
Color baseColor = DARKGRAY;
Color themeColor = LIGHTGRAY;
Color highlight = WHITE;
Color numberColors[9] = {
        WHITE, 
        BLUE,
        DARKGREEN, 
        RED, 
        DARKBLUE, 
        MAROON, 
        (Color){50, 150, 150, 255},
        BLACK, 
        DARKGRAY
    };

Minesweeper::Minesweeper(int width, int height, int cellSize)
    : width(width), height(height), cellSize(cellSize),
      grid(width * height, Cell(false))
{
}

void Minesweeper::RevealCell(int x, int y)
{
    Cell& cell = grid[index(x, y)];
    if (cell.visited || cell.flagged) return;

    cell.visited = true;
    
    if (cell.hasMine)
    {
        // LOSE HERE
        debug = true;
        return;
    }
    bufferQueue.push({x, y});
}

// 1 time step, empty reveal queue and swap
void Minesweeper::Update()
{
    // update loop condition
    while (!revealQueue.empty()) 
    {
        //int x = revealQueue.front().first;
        //int y = revealQueue.front().second;
        auto [x, y] = revealQueue.front();
        revealQueue.pop();
        Cell& cell = grid[index(x, y)];
        cell.revealed = true;

        // propagate to valid neighbours
        if (!cell.hasMine && cell.adjacentMines == 0) // only flood fill from empty cells
        {    
            for (auto p : getNeighbours(x, y))
            {
                Cell& neighbourCell = grid[index(p.first, p.second)];
                if (neighbourCell.reveallable()) RevealCell(p.first, p.second);
            }
        }
    }
    std::swap(revealQueue, bufferQueue);
}

void Minesweeper::DrawCell(Cell cell, int x, int y) const
{
    int border = 1;
    int bigBorder = cellSize/8;
    int xPos = x * cellSize;
    int yPos = y * cellSize;
    int textOffset = cellSize / 4;
    // base colour
    DrawRectangle(xPos, yPos, cellSize, cellSize, baseColor);
    if (!cell.revealed)
    {
        // highlight
        DrawTriangle((Vector2){xPos, yPos},
                     (Vector2){xPos, yPos + cellSize},
                     (Vector2){xPos + cellSize, yPos}, highlight);
        // center square
        DrawRectangle(xPos + bigBorder, yPos + bigBorder, cellSize - (2 * bigBorder), cellSize - (2 * bigBorder), themeColor);
        if (cell.flagged)
        {
            DrawText("f", xPos + textOffset, yPos + textOffset, cellSize/2, RED);
        }
    }
    else if (cell.revealed)
    {
        DrawRectangle(xPos + border, yPos + border, cellSize - (2 * border), cellSize - (2 * border), themeColor);
        if (cell.adjacentMines > 0) DrawText(TextFormat("%d", cell.adjacentMines), xPos + textOffset, yPos + textOffset, cellSize/2, numberColors[cell.adjacentMines]);
    }
}

void Minesweeper::Draw() const
{
    for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
    {
        DrawCell(grid[index(x,y)], x, y);
    }
}

void Minesweeper::Randomize(float probability)
{
    for (int i = 0; i < width * height; ++i)
    {
        bool isMine = GetRandomValue(0, 100) < probability * 100;
        grid[i] = Cell(isMine);
    }
    enumerateMines();
}

void Minesweeper::FlagCell(int x, int y)
{
    int id = index(x, y);
    grid[id].Flag();
}

int Minesweeper::index(int x, int y) const
{
    return width * y + x;
}

std::vector<std::pair<int, int>> Minesweeper::getNeighbours(int x, int y) const
{
    std::vector<std::pair<int,int>> neighbours;
    for (int dx = -1; dx <= 1; dx++)
    for (int dy = -1; dy <= 1; dy++)
    {
        if (dx == 0 && dy == 0) continue; // skip self
        int nx = x + dx;
        int ny = y + dy;
        if (nx >= 0 && nx < width && ny >= 0 && ny < height)
        {
            neighbours.push_back({nx, ny});
        }
    }
    return neighbours;
}

void Minesweeper::enumerateMines()
{
    for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
    {
        int count = 0;
        for (auto p : getNeighbours(x, y))
            count += grid[index(p.first, p.second)].hasMine;
        grid[index(x, y)].adjacentMines = count;
    }
}

void Minesweeper::PrintQueue()
{
    std::queue<std::pair<int,int>> copy = revealQueue;
    while (!copy.empty())
    {
        auto p = copy.front();
        copy.pop();
        std::cout << "(" << p.first << ", " << p.second << ")" << std::endl;
    }
}
