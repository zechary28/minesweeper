#include <raylib.h>
#include "game-of-life.h"
#include "rules_setup.h"

int main() 
{    
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 600;
    int cellSize = 10;
    
    InitWindow(screenWidth, screenHeight, "Game of Life");
    SetTargetFPS(10);

    GameOfLife life(screenWidth / cellSize, screenHeight / cellSize, cellSize);
    life.Randomize(0.33f);
    
    float timer = 0.0f;
    
    while (!WindowShouldClose())
    {        
        float dt = GetFrameTime(); // seconds since last frame
        timer += dt;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 mouse = GetMousePosition();
            int x = mouse.x/cellSize;
            int y = mouse.y/cellSize;       
            life.ToggleCell(x, y);
        }

        if (timer >= 0.01f)
        {
            life.Update();
            timer -= 0.01f;   // keep leftover time
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);
        life.Draw();

        EndDrawing();
    }
    
    CloseWindow();
}