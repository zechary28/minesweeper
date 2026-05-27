#include "minesweeper.h"
#include <raylib.h>
#include <iostream>

bool debug = false;

// CLASSIC COLOR PALETTE
Color baseColor = DARKGRAY;
//Color themeColor = (Color){0xde, 0x73, 0x56, 0xff};
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
    : width(width), height(height), cellSize(cellSize), won(false), lost(false),
      //timer(0.0f), period(0.02f), generated(false), Solver(SelectionMode::NearestToLast),
      grid(width * height, Cell(false))
{
}

// void Minesweeper::Update(float dt)
// {

// }

// void Minesweeper::HandleInput()
// {
//     // move input handling from main.cpp here
// }

// bool Minesweeper::IsOver() const
// {
//     return won || lost;
// }

// void Minesweeper::Reset() const
// {

// }

const Cell& Minesweeper::getCell(int x, int y) const
{
    return grid[index(x, y)];
}

int Minesweeper::getWidth() const
{
    return width;
}

int Minesweeper::getHeight() const
{
    return height;
}

bool Minesweeper::IsIdle() const
{
    return revealQueue.empty() && bufferQueue.empty();
}

void Minesweeper::RevealCell(int x, int y)
{
    Cell& cell = grid[index(x, y)];

    if (cell.flagged) return;

    else if (cell.revealed)
    { // chord reveal

        // count adjacent flags
        int adjacentFlags = 0;
        for (auto p : getNeighbours(x, y))
            adjacentFlags += grid[index(p.first, p.second)].flagged;
        
        // reveal all neighbours if count is correct
        if (adjacentFlags != cell.adjacentMines) return;

        for (auto p : getNeighbours(x, y))
            SingleReveal(p.first, p.second);
    } 
    else 
    { // regular reveal
        SingleReveal(x, y);
    }
}

void Minesweeper::SingleReveal(int x, int y)
{

    Cell& cell = grid[index(x, y)];
    if (cell.visited || cell.flagged) return;

    cell.visited = true;
        
    if (cell.hasMine)
    {
        // LOSE HERE
        //debug = true;
        //themeColor = RED;
        lost = true;
        return;
    }
    bufferQueue.push({x, y});
}

// void Minesweeper::Update(float dt)
// {
//     if (dt >= period)
//     {
//         if (IsIdle()) solver.Step();
//         minesweeper.Update();
//         timer -= period;   // keep leftover time
//     }
// }

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
                if (neighbourCell.floodable()) RevealCell(p.first, p.second);
            }
        }
    }
    CheckIsWon();
    std::swap(revealQueue, bufferQueue);
}

void Minesweeper::CheckIsWon() {
    for (int i = 0; i < width * height; i++)
    {
        if (!grid[i].hasMine && !grid[i].revealed)
        {    
            won = false;
            return;
        }
    }
    won = true;
}

bool Minesweeper::IsWon() const {
    return won;
}

bool Minesweeper::IsLost() const {
    return lost;
}

void Minesweeper::DrawCell(Cell cell, int x, int y) const
{
    int border = 1;
    int bigBorder = cellSize/8;
    int xPos = x * cellSize;
    int yPos = y * cellSize;
    int textOffset = cellSize / 4;
    Color color = cell.visited && debug ? SKYBLUE : themeColor;
    // base colour
    DrawRectangle(xPos, yPos, cellSize, cellSize, baseColor);
    if (!cell.revealed)
    {
        // highlight
        float fx = (float)xPos;
        float fy = (float)yPos;
        float fs = (float)cellSize;
        DrawTriangle((Vector2){fx, fy},
                     (Vector2){fx, fy + fs},
                     (Vector2){fx + fs, fy}, highlight);
        // center square
        DrawRectangle(xPos + bigBorder, yPos + bigBorder, cellSize - (2 * bigBorder), cellSize - (2 * bigBorder), color);
        if (cell.flagged)
        {
            DrawText("f", xPos + textOffset, yPos + textOffset, cellSize/2, RED);
        }
    }
    else if (cell.revealed || lost)
    {
        DrawRectangle(xPos + border, yPos + border, cellSize - border, cellSize - border, color);
        if (cell.adjacentMines > 0) DrawText(TextFormat("%d", cell.adjacentMines), xPos + textOffset, yPos + textOffset, cellSize/2, numberColors[cell.adjacentMines]);
    }
    if (cell.hasMine && lost)
    {
        DrawRectangle(xPos, yPos, cellSize, cellSize, RED);
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

void Minesweeper::ResetMap()
{
    for (int i = 0; i < width * height; ++i)
        grid[i] = Cell(false);
    won = false;
    lost = false;
    bufferQueue = {};
    revealQueue = {};
}

void Minesweeper::GenerateMines(float probability, int x, int y)
{
    for (int i = 0; i < width * height; ++i)
    {
        bool isMine = GetRandomValue(0, 100) < probability * 100;
        grid[i] = Cell(isMine);
    }

    // ensure first click is 0
    grid[index(x, y)] = Cell(false);
    for (auto p : getNeighbours(x, y))
        grid[index(p.first, p.second)] = Cell(false);

    enumerateAllMines();
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

NeighbourList Minesweeper::getNeighbours(int x, int y) const
{
    NeighbourList neighbours;
    for (int dx = -1; dx <= 1; dx++)
    for (int dy = -1; dy <= 1; dy++)
    {
        if (dx == 0 && dy == 0) continue; // skip self
        int nx = x + dx;
        int ny = y + dy;
        if (nx >= 0 && nx < width && ny >= 0 && ny < height)
            neighbours.cells[neighbours.count++] = {nx, ny};
    }
    return neighbours;
}

int Minesweeper::enumerateMines(int x, int y)
{
    int count = 0;
    for (auto p : getNeighbours(x, y))
        count += grid[index(p.first, p.second)].hasMine;
    return count;
}

void Minesweeper::enumerateAllMines()
{
    for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
        grid[index(x, y)].adjacentMines = enumerateMines(x, y);
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
