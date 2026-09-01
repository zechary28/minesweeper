#pragma once
#include <memory>
#include "IGame.h"

class GameManager {
public:
    GameManager(std::unique_ptr<IGame> game)
        : current(std::move(game))
    {}

    void Update(float dt)
    {
        if (!current->IsOver())
            current->Update(dt);
    }

    void Draw() const
    {
        current->Draw();
    }

    void HandleInput()
    {
        current->HandleInput();
    }

    bool IsOver() const
    {
        return current->IsOver();
    }

private:
    std::unique_ptr<IGame> current;
};