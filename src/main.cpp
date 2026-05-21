#include <raylib.h>
#include "minesweeper.h"

int main() 
{    
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 600;
    int cellSize = 50;
    
    InitWindow(screenWidth, screenHeight, "Minesweeper");
    SetTargetFPS(60);

    Minesweeper minesweeper(screenWidth / cellSize, screenHeight / cellSize, cellSize);
    minesweeper.Randomize(0.15f);
    
    float timer = 0.0f;
    float period = 0.02f;
    
    while (!WindowShouldClose())
    {        
        float dt = GetFrameTime(); // seconds since last frame
        timer += dt;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 mouse = GetMousePosition();
            int x = mouse.x/cellSize;
            int y = mouse.y/cellSize;
            minesweeper.RevealCell(x, y);
        }
        else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            Vector2 mouse = GetMousePosition();
            int x = mouse.x/cellSize;
            int y = mouse.y/cellSize;
            minesweeper.FlagCell(x, y);
        }

        if (timer >= period)
        {
            minesweeper.Update();
            timer -= period;   // keep leftover time
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);
        minesweeper.Draw();

        EndDrawing();
    }
    
    CloseWindow();
}