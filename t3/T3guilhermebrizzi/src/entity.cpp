#include "entity.h"
#include "canvas_pro.h"

#include "event.h"

Entity::Entity(float x, float y)
{
    this->x = x;
    this->y = y;
}

void Entity::draw()
{
    CVpro::color(255, 0, 0);
    CV::circleFill(x, y, 15, 15);
}

void Entity::tick()
{
    //empty for base class
}

void Entity::input(Event *e)
{
    //empty for base class
}