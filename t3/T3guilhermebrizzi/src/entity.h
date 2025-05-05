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
        float x;
        float y;

        Bounding_Box box;
        Footprint footprint;
        CVpro::image *texture;

        Entity(float x, float y);
        virtual void draw();
        virtual void tick();
        virtual void input(Event *e);
        //virtual void collide();

        std::vector<std::unique_ptr<Event>>* events_ptr;
};


#endif