#ifndef SHARED_DEF_H
#define SHARED_DEF_H

#define SELECTABLE_TYPE_CIRCLE 0
#define SELECTABLE_TYPE_SQUARE 1
#define SELECTABLE_TYPE_PIXEL_ART 2

struct std_selectable_values
{
    int size;
    int type;

    std_selectable_values(int size, int type)
    {
        this->size = size;
        this->type = type;
    }
};

typedef struct std_selectable_values std_selectable_values;

#endif