#include "gunshot.h"

#define GUNSHOT_SPEED 5

Gunshot::Gunshot(float x, float y, Vector2 direction, float speed) : Entity(x, y)
{
    texture = CVpro::load_bitmap("./T3guilhermebrizzi/assets/gunshot.bmp");
    this->direction = direction;
    this->speed = speed;
}

void Gunshot::draw()
{
    box = texture->display_bitmap(x, y, footprint);
}

void Gunshot::tick()
{
    x += direction.x * speed;
    y += direction.y * speed;
}