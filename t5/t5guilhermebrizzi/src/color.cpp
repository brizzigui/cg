#include "color.h"

Color::Color(int r, int g, int b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

Color::Color()
{
    r = g = b = 0;
}

Color::~Color()
{
}