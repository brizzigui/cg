#ifndef SHARED_DEF_H
#define SHARED_DEF_H

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