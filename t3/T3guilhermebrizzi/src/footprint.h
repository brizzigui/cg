#ifndef FOOTPRINT_H
#define FOOTPRINT_H

class Footprint
{
    public:
        Footprint();
        void mark_pixel(int x, int y);
        void clear();

        bool *matrix;
        static int width;
        static int height;
};


#endif