#include "entity.h"
#include "canvas_pro.h"

#include "event.h"
#include "footprint.h"

Entity::Entity(float x, float y)
{
    this->x = x;
    this->y = y;
    footprint = Footprint();
}

void Entity::draw()
{
    // empty for base class
}

void Entity::tick()
{
    //empty for base class
}

void Entity::input(Event *e)
{
    //empty for base class
}