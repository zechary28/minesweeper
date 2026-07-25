#include "minesweeper.h"
#include "game-of-life.h"
#include "langtons-ant.h"
#include "theme.h"

#include <raylib.h>
#include <iostream>
#include <functional>
#include <vector>
#include <memory>

constexpr int PARTIAL_SCREEN_WIDTH = 1200;
constexpr int PARTIAL_SCREEN_HEIGHT = 800;

constexpr int CELL_SIZE = 20;

constexpr float HUE_STEP         = 70.0f;
constexpr float THEME_SATURATION = 0.4f;
constexpr float THEME_VALUE      = 0.85f;

struct GameSlot {
    std::function<std::unique_ptr<IGame>(ThemeColor)> make;
};

int main() 
{            
    InitWindow(100, 100, "Minesweeper");  // temporary small window to init raylib

    int currentMonitor = 0;
    int screenWidth  = PARTIAL_SCREEN_WIDTH;
    int screenHeight = PARTIAL_SCREEN_HEIGHT;

    // round down window dimensions
    screenWidth  = (screenWidth  / CELL_SIZE) * CELL_SIZE;
    screenHeight = (screenHeight / CELL_SIZE) * CELL_SIZE;
    SetWindowSize(screenWidth, screenHeight);
    //ToggleBorderlessWindowed();
    //ToggleFullscreen();

    int gridW = screenWidth / CELL_SIZE;
    int gridH = screenHeight / CELL_SIZE;

    SetTargetFPS(60);
    
    std::vector<GameSlot> slots = {
        {
            [=](ThemeColor t) -> std::unique_ptr<IGame> {
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
 
    while (!WindowShouldClose())
    {        

        if (IsKeyPressed(KEY_M))  // cycle through monitors
        {
            // int monitorCount = GetMonitorCount();
            // int nextMonitor = (GetCurrentMonitor() + 1) % monitorCount;
            // std::cout << "monitorCount:   " + std::to_string(monitorCount) + "\n";
            // std::cout << "currentMonitor: " + std::to_string(GetCurrentMonitor()) + "\n";
            // std::cout << "nextMonitor:    " + std::to_string(nextMonitor) + "\n";

            // std::cout << "currentMonitorPos: " + std::to_string(GetMonitorPosition(currentMonitor).x) + ", " + std::to_string(GetMonitorPosition(currentMonitor).y) + "\n";
            // std::cout << "nextMonitorPos:    " + std::to_string(GetMonitorPosition(nextMonitor).x) + ", " + std::to_string(GetMonitorPosition(nextMonitor).y) + "\n";

            // std::cout << "currentMonitorDim: " + std::to_string(GetMonitorWidth(currentMonitor)) + ", " + std::to_string(GetMonitorHeight(currentMonitor)) + "\n";
            // std::cout << "nextMonitorDim:    " + std::to_string(GetMonitorWidth(nextMonitor)) + ", " + std::to_string(GetMonitorHeight(nextMonitor)) + "\n";

            // // get target monitor's position and dimensions
            // Vector2 pos = GetMonitorPosition(nextMonitor);
            // int w = GetMonitorWidth(nextMonitor);
            // int h = GetMonitorHeight(nextMonitor);

            // ToggleBorderlessWindowed();   // back to normal window
            // SetWindowPosition((int)pos.x, (int)pos.y);
            // SetWindowSize(w, h);
            // ToggleBorderlessWindowed();   // borderless on new monitor

            // currentMonitor = nextMonitor;
            // screenWidth  = w;
            // screenHeight = h;
            // gridW = screenWidth / CELL_SIZE;
            // gridH = screenHeight / CELL_SIZE;

            // gameIndex = 0;
            // current = slots[gameIndex].make(makeTheme());

            ToggleFullscreen();

            int w = PARTIAL_SCREEN_WIDTH;
            int h = PARTIAL_SCREEN_HEIGHT;
            screenWidth  = w;
            screenHeight = h;
            gridW = screenWidth / CELL_SIZE;
            gridH = screenHeight / CELL_SIZE;

            gameIndex = 0;
            current = slots[gameIndex].make(makeTheme());

        }

        // check for new window dimensions, update grid accordingly
        int newW = GetMonitorWidth(currentMonitor);
        int newH = GetMonitorHeight(currentMonitor);
        if (newW != screenWidth || newH != screenHeight)
        {
            screenWidth  = newW;
            screenHeight = newH;
            gridW = screenWidth / CELL_SIZE;
            gridH = screenHeight / CELL_SIZE;

            gameIndex = 0;
            current = slots[i].make(makeTheme());
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