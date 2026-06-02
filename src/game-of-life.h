#pragma once
#include "IGame.h"
#include "theme.h"

#include <vector>
#include <cstdint>

class GameOfLife : public IGame
{
public:
    GameOfLife(int width, int height, int cellSize, ThemeColor theme);

    void Update(float dt) override;
    void Draw() const override;
    void HandleInput() override;
    bool IsOver() const override;
    void Reset() override;

    void Randomize();
    void ToggleCell(int x, int y);

private:
    int width;
    int height;
    int cellSize;
    float randomDensity;

    float timer;
    float period;
    int cycles;
    int cyclesLimit;

    std::vector<uint8_t> grid;
    std::vector<uint8_t> next;

    ThemeColor theme;

    int index(int x, int y) const;
    int aliveNeighbours(int x, int y) const;
};
