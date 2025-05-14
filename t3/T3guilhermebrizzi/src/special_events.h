#include "entity.h"

class Event_Create_Entity : public Event
{
    public:
        Entity *entity;
        Event_Create_Entity(Entity *e);
};

class Event_Suicide : public Event
{
    public:
        int id;
        Event_Suicide(int id);
};

class Event_Point : public Event
{
    public:
        int points;
        Event_Point(int points);
};

class Event_Game_Over : public Event
{
    public:
        Event_Game_Over();
};