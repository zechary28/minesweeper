#pragma once

class IGame {
public:
    virtual void Update(float dt) = 0;
    virtual void Draw() const = 0;
    virtual void HandleInput() = 0;
    virtual bool IsOver() const = 0;
    virtual void Reset() = 0;
    virtual ~IGame() = default;
};
