#include "game-of-life.h"
#include <raylib.h>
#include <cmath>

static Color getTheme(ThemeColor t)      { return ColorFromHSV(t.h, t.s, t.v); }
static Color getShade(ThemeColor t)      { return ColorFromHSV(t.h, t.s * 0.8f, t.v * 0.75f); }
// static Color getComplement(ThemeColor t) { return ColorFromHSV(fmod(t.h + 150.f, 360.f), t.s, t.v); }

GameOfLife::GameOfLife(int width, int height, int cellSize, ThemeColor theme)
    : width(width), height(height), cellSize(cellSize), randomDensity(0.3f), timer(0.0f), period(0.1f), cycles(0), cyclesLimit(1200), theme(theme)
{
    grid.resize(width * height, 0);
    next.resize(width * height, 0);
    Randomize();
}

void GameOfLife::Update(float dt)
{
    timer += dt;
    if (timer >= period)
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
        timer -= period;
        cycles += 1;
    }
}

void GameOfLife::Draw() const
{
    Color bg   = getShade(theme);
    Color fg   = getTheme(theme);
    
    //Color flag = getComplement();
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
}

void GameOfLife::HandleInput()
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Vector2 mouse = GetMousePosition();
        int x = mouse.x/cellSize;
        int y = mouse.y/cellSize;
        ToggleCell(x, y);
    }
}

bool GameOfLife::IsOver() const
{
    // check if stable state has been reached
    for (int i = 0; i < width * height; i++) {
        if (grid[i] != next[i]) return false;
    }
    return true;
}

void GameOfLife::Reset()
{
    timer = 0.0f;
    cycles = 0;
    Randomize();
}

void GameOfLife::Randomize()
{
    for (int i = 0; i < width * height; ++i)
    {
        grid[i] = GetRandomValue(0, 100) < randomDensity * 100;
        next[i] = 0;
    }
}

void GameOfLife::ToggleCell(int x, int y)
{
    if (x < 0 || x >= width || y < 0 || y >= height) return;
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
