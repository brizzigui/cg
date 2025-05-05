#include "bounding_box.h"
#include <limits>

Bounding_Box::Bounding_Box()
{
    this->min_x = std::numeric_limits<float>::infinity();
    this->min_y = std::numeric_limits<float>::infinity();
    this->max_x = -std::numeric_limits<float>::infinity();
    this->max_y = -std::numeric_limits<float>::infinity();
}

Bounding_Box::Bounding_Box(float min_x, float min_y, float max_x, float max_y)
{
    this->min_x = min_x;
    this->min_y = min_y;
    this->max_x = max_x;
    this->max_y = max_y;
}

void Bounding_Box::update(float x, float y)
{
    min_x = (x < min_x) ? x : min_x;
    min_y = (y < min_y) ? y : min_y;
    max_x = (x > max_x) ? x : max_x;
    max_y = (y > max_y) ? y : max_y;
}