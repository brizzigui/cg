#include "special_events.h"

Event_Create_Entity::Event_Create_Entity(Entity *entity)
{
    this->type = EVENT_CREATE_ENTITY;
    this->entity = entity;
}

Event_Suicide::Event_Suicide(int id)
{
    this->type = EVENT_SUICIDE;
    this->id = id;
}

Event_Point::Event_Point(int points)
{
    this->type = EVENT_POINT;
    this->points = points;
}

Event_Game_Over::Event_Game_Over()
{
    this->type = EVENT_GAME_OVER;
}