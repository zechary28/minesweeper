#include <raylib.h>
#include "minesweeper.h"
#include "solver.h"

int main() 
{    
    constexpr int screenWidth = 1800;
    constexpr int screenHeight = 1000;
    int cellSize = 20;
    float mineProbability = 0.14f;
    
    InitWindow(screenWidth, screenHeight, "Minesweeper");
    SetTargetFPS(60);

    Minesweeper minesweeper(screenWidth / cellSize, screenHeight / cellSize, cellSize);
    Solver solver(minesweeper, SelectionMode::NearestToLast);

    bool generated = false;
    bool reset = false;
    float timer = 0.0f;
    float period = 0.02f;
    
    while (!WindowShouldClose())
    {        
        float dt = GetFrameTime(); // seconds since last frame
        timer += dt;

        // first click
        if (!generated)
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                Vector2 mouse = GetMousePosition();
                int x = mouse.x/cellSize;
                int y = mouse.y/cellSize;
                minesweeper.GenerateMines(mineProbability, x, y);
                generated = true;
                minesweeper.RevealCell(x, y);
            }
        }

        // game ongoing
        else if (!minesweeper.IsWon() && !minesweeper.IsLost())
        {
            // receive user input
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                Vector2 mouse = GetMousePosition();
                int x = mouse.x/cellSize;
                int y = mouse.y/cellSize;
                //solver.Step();
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
                // if (minesweeper.IsIdle()) solver.Step();
                minesweeper.Update();
                timer -= period;   // keep leftover time
            }
        }

        // game won/lost
        else if (minesweeper.IsWon() || minesweeper.IsLost())
        {
            reset = false;
            // restart game
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if (!reset) {
                    minesweeper.ResetMap();
                    reset = true;
                    generated = false;
                }
                else
                {
                    Vector2 mouse = GetMousePosition();
                    int x = mouse.x/cellSize;
                    int y = mouse.y/cellSize;
                    minesweeper.GenerateMines(mineProbability, x, y);
                    generated = true;
                    minesweeper.RevealCell(x, y);
                }
            }
        }

        BeginDrawing();
        ClearBackground(DARKGRAY);
        minesweeper.Draw();

        EndDrawing();
    }
    
    CloseWindow();
}