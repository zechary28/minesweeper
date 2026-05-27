#include <vector>
#include <queue>
#include <cstdint>
#include "cell.h"
//#include "solver.h"
//#include "IGame.h"

class Minesweeper
{
public:
    Minesweeper(int width, int height, int cellSize);

    // void Update(float dt) override;
    void Draw() const;
    // void HandleInput() override;
    // bool IsOver() const override;
    // void Reset() override;

    void RevealCell(int x, int y);   // called on left click
    void SingleReveal(int x, int y);
    void FlagCell(int x, int y);     // called on right click
    const Cell& getCell(int x, int y) const;
    NeighbourList getNeighbours(int x, int y) const;
    int getWidth() const;
    int getHeight() const;
    void Update();
    void CheckIsWon();
    bool IsWon() const;
    bool IsLost() const;
    bool IsIdle() const;
    void ResetMap();
    void DrawCell(Cell cell, int x, int y) const;
    void GenerateMines(float probability, int x, int y);
    void PrintQueue();

private:
    int width;
    int height;
    int cellSize;
    bool won;
    bool lost;

    float timer;
    float period;
    bool generated;
    //Solver solver;

    std::vector<Cell> grid;
    std::queue<std::pair<int, int>> revealQueue;
    std::queue<std::pair<int, int>> bufferQueue;

    int index(int x, int y) const;
    int enumerateMines(int x, int y);
    void enumerateAllMines();
};
