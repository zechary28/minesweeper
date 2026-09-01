#include "minesweeper.h"
#include "game-of-life.h"
#include "langtons-ant.h"
#include "theme.h"

#include <raylib.h>
#include <iostream>
#include <functional>
#include <vector>
#include <memory>

constexpr int WINDOW_WIDTH = 1200;
constexpr int WINDOW_HEIGHT = 800;

constexpr int CELL_SIZE = 20;

constexpr float HUE_STEP         = 70.0f;
constexpr float THEME_SATURATION = 0.4f;
constexpr float THEME_VALUE      = 0.85f;

struct GameSlot {
    std::function<std::unique_ptr<IGame>(ThemeColor)> make;
};

int main()
{   
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_ALWAYS_RUN);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Minesweeper");

    bool isFullscreen = false;
    int screenWidth  = WINDOW_WIDTH;
    int screenHeight = WINDOW_HEIGHT;
    int monitor = 0;

    int gridW = screenWidth / CELL_SIZE;
    int gridH = screenHeight / CELL_SIZE;

    SetTargetFPS(60);
    
    std::vector<GameSlot> slots = {
        {
            [&](ThemeColor t) -> std::unique_ptr<IGame> {
                return std::make_unique<Minesweeper>(gridW, gridH, CELL_SIZE, 0.13f, t);
            }
        }
        // {
        //     [=](ThemeColor t) -> std::unique_ptr<IGame> {
        //         return std::make_unique<GameOfLife>(gridW, gridH, CELL_SIZE, t);
        //     }
        // },
        // {
        //     [=](ThemeColor t) -> std::unique_ptr<IGame> {
        //         return std::make_unique<LangtonsAnt>(gridW, gridH, CELL_SIZE, t);
        //     }
        // }
    };

    float hue = 0.0f;
    int gameIndex = 0;

    auto makeTheme = [&]() -> ThemeColor {
        return { hue, THEME_SATURATION, THEME_VALUE };
    };
 
    auto nextGame = [&]() -> std::unique_ptr<IGame> {
        hue = fmod(hue + HUE_STEP, 360.0f);
        return slots[gameIndex].make(makeTheme());
    };

    std::unique_ptr<IGame> current = slots[gameIndex].make(makeTheme());

    SetWindowState(FLAG_WINDOW_ALWAYS_RUN);
 
    while (!WindowShouldClose())
    {        

        if (IsKeyPressed(KEY_F))  // cycle through monitors
        {
            if (!isFullscreen)
            {
                // go fullscreen on whichever monitor the window is currently on
                screenWidth  = GetMonitorWidth(monitor);
                screenHeight = GetMonitorHeight(monitor);
                SetWindowSize(screenWidth, screenHeight);
                ToggleFullscreen();
                isFullscreen = true;
            }
            else
            {
                // back to fixed window
                ToggleFullscreen();
                SetWindowState(FLAG_WINDOW_UNDECORATED);
                screenWidth  = WINDOW_WIDTH;
                screenHeight = WINDOW_HEIGHT;
                SetWindowSize(screenWidth, screenHeight);
                isFullscreen = false;
            }

            // recreate game for new dimensions
            gridW = screenWidth / CELL_SIZE;
            gridH = screenHeight / CELL_SIZE;
            gameIndex = (gameIndex + 1) % static_cast<int>(slots.size());
            current = nextGame();

        }

        // check if monitor has changed, DO NOTHING FOR NOW
        int currentMonitor = GetCurrentMonitor();
        if (currentMonitor != monitor)
        {
            monitor = currentMonitor;
            std::cout << "changing monitor to " << std::to_string(monitor) << "\n";
        }

        if (current->IsOver()) 
        {
            gameIndex = (gameIndex + 1) % static_cast<int>(slots.size());
            current = nextGame();
        }

        float dt = GetFrameTime();
        current->HandleInput();
        current->Update(dt);

        BeginDrawing();
        ClearBackground(DARKGRAY);
        current->Draw();
        EndDrawing();
    }
    
    CloseWindow();
}