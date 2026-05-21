#include <vector>
#include <queue>
#include <cstdint>
#include "cell.h"

class Minesweeper
{
public:
    Minesweeper(int width, int height, int cellSize);

    void RevealCell(int x, int y);   // called on left click
    void FlagCell(int x, int y);     // called on right click
    void Update();
    bool IsWon() const;
    bool IsLost() const;
    void Draw() const;
    void DrawCell(Cell cell, int x, int y) const;
    void Randomize(float probability);
    void PrintQueue();

private:
    int width;
    int height;
    int cellSize;

    std::vector<Cell> grid;
    std::queue<std::pair<int, int>> revealQueue;
    std::queue<std::pair<int, int>> bufferQueue;

    int index(int x, int y) const;
    std::vector<std::pair<int, int>> getNeighbours(int x, int y) const;
    int countNeighbours(int x, int y) const;
    void enumerateMines();
};
