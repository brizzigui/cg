#ifndef LAYER_MANAGER_H
#define LAYER_MANAGER_H

#include "gl_canvas2d.h"
#include "canvas_pro.h"

#include <vector>

struct Layer
{
    CVpro::image *image;
};

class Layer_Manager
{
    private:
        std::vector<Layer> layers;
    public:
        Layer_Manager();
};


#endif