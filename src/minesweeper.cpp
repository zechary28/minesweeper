#include "minesweeper.h"

#include <raylib.h>
#include <iostream>

bool debug = false;

// CLASSIC COLOR PALETTE
Color baseColor = (Color){0xd8, 0xc8, 0xa8, 0xff};
//Color themeColor = (Color){0xde, 0x73, 0x56, 0xff};
Color themeColor = (Color){0xe8, 0xd5, 0xc4, 0xff};
Color highlight = WHITE;
Color numberColorsClassic[9] = {
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

Color numberColorsPastel[9] = {
        WHITE, 
        (Color){0xd4, 0xe8, 0xf0, 0xff},
        (Color){0xb8, 0xd4, 0xe3, 0xff}, 
        (Color){0x95, 0xb8, 0xd1, 0xff},
        DARKBLUE, 
        MAROON, 
        (Color){50, 150, 150, 255},
        BLACK, 
        DARKGRAY
    };
Color revealedColor = (Color){0xc4, 0xb5, 0xa5, 0xff};
Color flaggedColor = (Color){0xf4, 0xa5, 0xa5, 0xff};

static Color getTheme(ThemeColor t)      { return ColorFromHSV(t.h, t.s, t.v); }
static Color getShade(ThemeColor t)      { return ColorFromHSV(t.h, t.s * 0.8f, t.v * 0.75f); }
static Color getComplement(ThemeColor t) { return ColorFromHSV(fmod(t.h + 150.f, 360.f), t.s, t.v); }

Minesweeper::Minesweeper(int width, int height, int cellSize, float mineProbability, ThemeColor theme)
    : width(width), height(height), cellSize(cellSize), mineProbability(mineProbability), won(false), lost(false),
      timer(0.0f), period(0.05f), reset(false), generated(false),
      grid(width * height, Cell(false)),
      solver(SelectionMode::NearestToLast),
      theme(theme)
{
}


// IGame Interface

void Minesweeper::Update(float dt)
{
    timer += dt;
    if (timer >= period)
    {
        if (IsIdle()) solver.Step(*this);
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
        timer -= period;   // keep leftover time
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

void Minesweeper::HandleInput()
{
    // first click
    if (!generated)
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 mouse = GetMousePosition();
            int x = mouse.x/cellSize;
            int y = mouse.y/cellSize;
            GenerateMines(mineProbability, x, y);
            generated = true;
            RevealCell(x, y);
        }
    }

    // game ongoing
    else if (!IsWon() && !IsLost())
    {
        // receive user input
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 mouse = GetMousePosition();
            int x = mouse.x/cellSize;
            int y = mouse.y/cellSize;
            //solver.Step();
            RevealCell(x, y);
        }
        else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            Vector2 mouse = GetMousePosition();
            int x = mouse.x/cellSize;
            int y = mouse.y/cellSize;
            FlagCell(x, y);
        }
    }

    // game won/lost
    else if (IsWon() || IsLost())
    {
        reset = false;
        // restart game
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (!reset) {
                Reset();
                reset = true;
                generated = false;
            }
            else
            {
                Vector2 mouse = GetMousePosition();
                int x = mouse.x/cellSize;
                int y = mouse.y/cellSize;
                GenerateMines(mineProbability, x, y);
                generated = true;
                RevealCell(x, y);
            }
        }
    }
}

bool Minesweeper::IsOver() const
{
    return won || lost;
}

void Minesweeper::Reset()
{
    GenerateMines(mineProbability, width/2, height/2); // or a random start cell
    won = false;
    lost = false;
    generated = true;
    timer = 0.0f;
    bufferQueue = {};
    revealQueue = {};
    RevealCell(width/2, height/2); // kick off the first reveal
}


// Helpers

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
    Color bg   = getShade(theme);
    Color fg   = getTheme(theme);
    Color flag = getComplement(theme);

    int border = 1;
    // int bigBorder = cellSize/8;
    int xPos = x * cellSize;
    int yPos = y * cellSize;
    // int textOffset = cellSize / 4;
    // float scale = 1.0f;
    // float animPadding = (cellSize / 2.0f) * (1.0f - scale);
    // Color color = cell.visited && debug ? SKYBLUE : themeColor;

    // base colour
    DrawRectangle(xPos, yPos, cellSize, cellSize, bg);
    if (!cell.revealed)
    {
        // highlight
        // float fx = (float)xPos;
        // float fy = (float)yPos;
        // float fs = (float)cellSize;
        // DrawTriangle((Vector2){fx, fy},
        //              (Vector2){fx, fy + fs},
        //              (Vector2){fx + fs, fy}, highlight);
        // center square
        // classic style
        // DrawRectangle(xPos + bigBorder, yPos + bigBorder, cellSize - (2 * bigBorder), cellSize - (2 * bigBorder), color);
        // modern
        DrawRectangleRounded(
                {(float)(xPos + border), (float)(yPos + border), (float)(cellSize - border), (float)(cellSize - border)}, 
                0.3, 4, fg
        );
        if (cell.flagged)
        {
            // classic
            // DrawText("f", xPos + textOffset, yPos + textOffset, cellSize/2, RED);
            DrawRectangleRounded(
                    {(float)(xPos + border), (float)(yPos + border), (float)(cellSize - border), (float)(cellSize - border)}, 
                    0.3, 4, flag
            );
        }
    }
    else if (cell.revealed || lost)
    {
        // classic
        // DrawRectangle(xPos + border, yPos + border, cellSize - border, cellSize - border, color);
        // modern
        DrawRectangleRounded(
                {(float)(xPos + border), (float)(yPos + border), (float)(cellSize - border), (float)(cellSize - border)},                 
                0.3, 4, bg
        );
        //if (cell.adjacentMines > 0) DrawText(TextFormat("%d", cell.adjacentMines), xPos + textOffset, yPos + textOffset, cellSize/2, numberColorsPastel[cell.adjacentMines]);
    }
    if (cell.hasMine && lost)
    {
        DrawRectangle(xPos, yPos, cellSize, cellSize, RED);
    }
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


