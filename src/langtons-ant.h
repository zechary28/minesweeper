#pragma once
#include "IGame.h"
#include "theme.h"

#include <vector>
#include <cstdint>

enum class Direction { North, East, South, West };

struct Ant {
    int x;
    int y;
    Direction dir;
};

class LangtonsAnt : public IGame
{
public:
    LangtonsAnt(int width, int height, int cellSize, ThemeColor theme);

    void Update(float dt) override;
    void Draw() const override;
    void HandleInput() override;
    bool IsOver() const override;
    void Reset() override;

    void ToggleCell(int x, int y);

private:
    int width;
    int height;
    int cellSize;

    float timer;
    float period;
    int cycles;
    int cyclesLimit;

    std::vector<Ant> ants;
    std::vector<uint8_t> grid;

    ThemeColor theme;

    int index(int x, int y) const;
};
