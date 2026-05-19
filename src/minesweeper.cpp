enum CellType
{
    Empty,
    Mine
};
\
#include "game-of-life.h"
#include <raylib.h>

GameOfLife::GameOfLife(int width, int height, int cellSize)
    : width(width), height(height), cellSize(cellSize)
{
    std::vector<CellType> grid(width * height, Empty);
}

void GameOfLife::Update()
{
    for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
    {
        int alive = aliveNeighbours(x, y);
        bool isAlive = grid[index(x,y)];

        next[index(x,y)] =
            (isAlive && (alive == 2 || alive == 3)) ||
            (!isAlive && alive == 3);
    }
    grid.swap(next);
}

void GameOfLife::Draw() const
{
    for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
    {
        bool isAlive = grid[index(x,y)];
        Color color = isAlive ? GREEN : DARKGRAY;
        DrawRectangle(x * cellSize, y * cellSize, cellSize, cellSize, color);
    }
}

void GameOfLife::Randomize(float probability)
{
    for (int i = 0; i < width * height; ++i)
    {
        grid[i] = GetRandomValue(0, 100) < probability * 100;
        next[i] = 0;
    }
}

void GameOfLife::ToggleCell(int x, int y)
{
    int id = index(x, y);
    grid[id] = !grid[id];
}

int GameOfLife::index(int x, int y) const
{
    return width * y + x;
}

int GameOfLife::aliveNeighbours(int x, int y) const
{
    int count = 0;
    for (int dx = -1; dx <= 1; dx++)
    for (int dy = -1; dy <= 1; dy++)
    {
        if (dx == 0 && dy == 0) continue; // skip self
        int nx = x + dx;
        int ny = y + dy;
        if (nx >= 0 && nx < width && ny >= 0 && ny < height)
        {
            count += grid[index(nx, ny)];
        }
    }
    return count;
}
