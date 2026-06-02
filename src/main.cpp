#include "minesweeper.h"
#include "game-of-life.h"
#include "langtons-ant.h"
#include "theme.h"

#include <raylib.h>
#include <functional>
#include <vector>
#include <memory>

constexpr int screenWidth = 1800;
constexpr int screenHeight = 1000;
constexpr int cellSize = 20;
constexpr int gridW = screenWidth / cellSize;
constexpr int gridH = screenHeight / cellSize;

constexpr float HUE_STEP         = 70.0f;
constexpr float THEME_SATURATION = 0.4f;
constexpr float THEME_VALUE      = 0.85f;

struct GameSlot {
    std::function<std::unique_ptr<IGame>(ThemeColor)> make;
};

int main() 
{            
    InitWindow(screenWidth, screenHeight, "Minesweeper");
    SetTargetFPS(60);
    
    std::vector<GameSlot> slots = {
        {
            [=](ThemeColor t) -> std::unique_ptr<IGame> {
                return std::make_unique<Minesweeper>(gridW, gridH, cellSize, 0.14f, t);
            }
        },
        {
            [=](ThemeColor t) -> std::unique_ptr<IGame> {
                return std::make_unique<GameOfLife>(gridW, gridH, cellSize, t);
            }
        },
        {
            [=](ThemeColor t) -> std::unique_ptr<IGame> {
                return std::make_unique<LangtonsAnt>(gridW, gridH, cellSize, t);
            }
        }
    };

    float hue = 0.0f;
    int i = 0;

    auto makeTheme = [&]() -> ThemeColor {
        return { hue, THEME_SATURATION, THEME_VALUE };
    };
 
    auto nextGame = [&]() -> std::unique_ptr<IGame> {
        hue = fmod(hue + HUE_STEP, 360.0f);
        return slots[i].make(makeTheme());
    };

    std::unique_ptr<IGame> current = slots[i].make(makeTheme());
 
    while (!WindowShouldClose())
    {        
        if (current->IsOver()) 
        {
            i = (i + 1) % static_cast<int>(slots.size());
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