#include "entity.h"
#include "canvas_pro.h"

#include "event.h"
#include "footprint.h"

int Entity::id_counter = 0;

Entity::Entity(float x, float y)
{
    this->id = id_counter;
    id_counter++;

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

void Entity::collide(Entity *e)
{
    //empty for base class
}