#ifndef COLOR_H
#define COLOR_H

#include <iostream>

class Color
{
    public:
        int r, g, b;
        int a;

        Color()
        {
            r = g = b = a = 0;
        }

        Color(int r, int g, int b, int a)
        {
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = a;
        }
        
};

#endif