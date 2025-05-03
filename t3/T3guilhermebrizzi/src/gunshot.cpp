#include "gunshot.h"

#define GUNSHOT_SPEED 5

Gunshot::Gunshot(float x, float y, Vector2 direction, float speed) : Entity(x, y)
{
    this->direction = direction;
    this->speed = speed;
}

void Gunshot::draw()
{
    CVpro::color(255, 255, 255);
    CV::circleFill(x, y, 5, 15);
}

void Gunshot::tick()
{
    x += direction.x * speed;
    y += direction.y * speed;
}