#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <memory>
#include "event.h"

class Entity
{
    protected:
        float x;
        float y;

    public:
        Entity(float x, float y);
        virtual void draw();
        virtual void tick();
        virtual void input(Event *e);

        std::vector<std::unique_ptr<Event>>* events_ptr;
};


#endif