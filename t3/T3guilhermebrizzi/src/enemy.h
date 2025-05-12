#ifndef ENEMY_H
#define ENEMY_H

#include "entity.h"
#include "Vector2.h"
#include "canvas_pro.h"

class Enemy : public Entity
{
    private:
        Vector2 direction;
        float speed;
        int type;

        CVpro::image *get_texture(int type);

    public:
        Enemy(float x, float y, Vector2 direction, float speed, int type);
        virtual void draw();
        virtual void tick();
        virtual void collide(Entity *e);

};


#endif