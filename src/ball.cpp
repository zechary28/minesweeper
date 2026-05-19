#include "ball.h"
#include <raylib.h>

#include <cstdint>

Color randomColor()
{
    return Color{
        static_cast<uint8_t>(GetRandomValue(0, 255)),
        static_cast<uint8_t>(GetRandomValue(0, 255)),
        static_cast<uint8_t>(GetRandomValue(0, 255)),
        255
    };
}


Ball::Ball() 
    : x(100)
    , y(100)
    , speedX(5)
    , speedY(5)
    , radius(15)
    , color(WHITE)
{
}

void Ball::Update() 
{
    x += speedX;
    y += speedY;
    
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();
    
    if ((x + radius >= screenWidth) || (x - radius <= 0)) {
        speedX *= -1;
        color = randomColor();
    }
    
    if ((y + radius >= screenHeight) || (y - radius <= 0)) {
        speedY *= -1;
        color = randomColor();
    }
}

void Ball::Draw() const
{
    DrawCircle(x, y, radius, color);
}