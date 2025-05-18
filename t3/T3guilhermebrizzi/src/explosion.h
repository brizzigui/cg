#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "entity.h"

#include <vector>
#include "canvas_pro.h"

class Explosion : public Entity
{
    private:
        std::vector<CVpro::image *> bmps;
        int tick_lenght;
        int tick_counter;
        
    public:
        Explosion(float x, float y, int tick_lenght);
        ~Explosion();

        virtual void draw();
        virtual void tick();
};

#endif