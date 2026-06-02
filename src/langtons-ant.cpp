#include "langtons-ant.h"
#include <raylib.h>
#include <cmath>

static Color getTheme(ThemeColor t)      { return ColorFromHSV(t.h, t.s, t.v); }
static Color getShade(ThemeColor t)      { return ColorFromHSV(t.h, t.s * 0.8f, t.v * 0.75f); }
static Color getComplement(ThemeColor t) { return ColorFromHSV(fmod(t.h + 150.f, 360.f), t.s, t.v); }

void turnRight(Ant& ant) {
    ant.dir = static_cast<Direction>((static_cast<int>(ant.dir) + 1) % 4);
}

void turnLeft(Ant& ant) {
    ant.dir = static_cast<Direction>((static_cast<int>(ant.dir) + 3) % 4);
}

void stepForward(Ant& ant) {
    switch (ant.dir) {
        case Direction::North: ant.y -= 1; break;
        case Direction::East:  ant.x += 1; break;
        case Direction::South: ant.y += 1; break;
        case Direction::West:  ant.x -= 1; break;
    }
}

LangtonsAnt::LangtonsAnt(int width, int height, int cellSize, ThemeColor theme)
    : width(width), height(height), cellSize(cellSize), timer(0.0f), period(0.1f  ), cycles(0), cyclesLimit(1200), theme(theme)
{
    grid.resize(width * height, 0);
    ants.push_back({width/2, height/2, Direction::East});
    ants.push_back({width/2 + 1, height/2, Direction::East});
}

void LangtonsAnt::Update(float dt)
{
    timer += dt;
    if (timer >= period)
    {
        for (Ant& ant : ants)
        {
            bool isAlive = grid[index(ant.x, ant.y)];
            ToggleCell(ant.x, ant.y);
            if (isAlive) turnRight(ant);
            else         turnLeft(ant);
            stepForward(ant);
            ant.x = (ant.x + width)  % width;
            ant.y = (ant.y + height) % height;
        }
        timer -= period;
        cycles += 1;
    }
}

void LangtonsAnt::Draw() const
{
    Color bg   = getShade(theme);
    Color fg   = getTheme(theme);
    Color comp = getComplement(theme);
    int border = 1;

    for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x)
    {
        int xPos = x * cellSize;
        int yPos = y * cellSize;
        DrawRectangle(xPos, yPos, cellSize, cellSize, bg);

        bool isAlive = grid[index(x,y)];
        Color color = isAlive ? fg : bg;
        DrawRectangleRounded(
                {(float)(xPos + border), (float)(yPos + border), (float)(cellSize - border), (float)(cellSize - border)}, 
                0.3, 4, color
        );
    }

    for (const Ant& ant : ants)
    {
        int xPos = ant.x * cellSize + cellSize/2;
        int yPos = ant.y * cellSize + cellSize/2;
        DrawCircle(xPos, yPos, (float)(cellSize/3), comp);
    }
}

void LangtonsAnt::HandleInput()
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Vector2 mouse = GetMousePosition();
        int x = mouse.x/cellSize;
        int y = mouse.y/cellSize;
        ants.push_back({x, y, Direction::North});
    }
}

bool LangtonsAnt::IsOver() const
{
    return cycles >= cyclesLimit;
}

void LangtonsAnt::Reset()
{
    timer = 0.0f;
    cycles = 0;
}

void LangtonsAnt::ToggleCell(int x, int y)
{
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    int id = index(x, y);
    grid[id] = !grid[id];
}

int LangtonsAnt::index(int x, int y) const
{
    return width * y + x;
}
