#ifndef PREVIEW_H
#define PREVIEW_H

#include <vector>
#include "Vector2.h"



class Preview
{
    private:
        std::vector<Vector2> *R2_points;


        float *width, *height;
        float padding;
        
    public:
        Preview(std::vector<Vector2> *points, float screen_height, float screen_width);
        ~Preview();

        void draw();
        void update();
        void recreate();
};

#endif