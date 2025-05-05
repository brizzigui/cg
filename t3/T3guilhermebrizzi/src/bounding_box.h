#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

class Bounding_Box
{
    public:
        float min_x, min_y, max_x, max_y;
        Bounding_Box();
        Bounding_Box(float min_x, float min_y, float max_x, float max_y);
        void update(float x, float y);
};

#endif