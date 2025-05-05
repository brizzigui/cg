#include "footprint.h"
#include "canvas_pro.h"

#include <stdlib.h>
#include <iostream>

int Footprint::width = 0;
int Footprint::height = 0;

Footprint::Footprint()
{
    std::cout << Footprint::width << std::endl;
    matrix = (bool *)calloc(1, sizeof(bool) * Footprint::width * Footprint::height);
}

void Footprint::mark_pixel(int x, int y)
{
    if (x >= 0 && x < Footprint::width && y >= 0 && y < Footprint::height)
    {
        int base_index = y * Footprint::width + x;
        matrix[base_index] = true;
    }
}

