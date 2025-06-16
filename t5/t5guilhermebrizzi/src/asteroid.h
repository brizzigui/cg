#ifndef ASTEROID_H
#define ASTEROID_H

#include "color.h"
#include "Vector3.h"

class Asteroid
{
    private:
        Color color;

        float size;
        Vector3 pos;

        Vector3 dir;
        float speed = 0;

    public:
        Asteroid();
        Asteroid(Color color, Vector3 pos, float size);
        Asteroid(Color color, Vector3 pos, float size, Vector3 dir, float speed);
        ~Asteroid();

        void draw();
};

#endif