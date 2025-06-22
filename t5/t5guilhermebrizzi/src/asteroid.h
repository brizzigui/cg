#ifndef ASTEROID_H
#define ASTEROID_H

#include "color.h"
#include "Vector3.h"

class Asteroid
{
    private:
        Color color;

        float size;
        Vector3 dir;
        float speed = 0;
        float repeat_param = 8000.0;

        Vector3 find_real_position(Vector3 camera);

    public:
        Vector3 pos;

        Asteroid();
        ~Asteroid();

        void draw(Vector3 camera);
        void tick();
};

#endif