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