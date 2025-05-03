#ifndef GUNSHOT_H
#define GUNSHOT_H

#define GUNSHOT_DEFAULT_SPEED 5

#include "canvas_pro.h"
#include "entity.h"

class Gunshot : public Entity
{
    private:
        Vector2 direction;
        float speed;
    public:
        Gunshot(float x, float y, Vector2 direction, float speed);
        virtual void draw();
        virtual void tick();

};

#endif