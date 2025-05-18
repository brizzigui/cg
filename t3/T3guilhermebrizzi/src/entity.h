#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <memory>
#include "event.h"
#include "footprint.h"
#include "bounding_box.h"
#include "canvas_pro.h"

class Entity
{
    public:
        static int id_counter;

        int id;
        int god_id = -1;

        float x;
        float y;

        Bounding_Box box;
        Footprint footprint;
        CVpro::image *texture = NULL;
        bool change = false;
        bool collidable = true;

        Entity(float x, float y);
        ~Entity();
        virtual void draw();
        virtual void tick();
        virtual void input(Event *e);
        virtual void collide(Entity *e);

        std::vector<std::unique_ptr<Event>>* events_ptr;
};


#endif