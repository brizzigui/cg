/*
    Não quer codar na Canvas?
    Use a biblioteca CVPro e vire o PRO em CG

    Texto rápido, cores inteiras, imagens .bmp nativas:
    vire PRO agora!
*/

// CVpro GAMING version 2.0 - t3

#ifndef CANVAS_PRO_H
#define CANVAS_PRO_H

#include "gl_canvas2d.h"
#include "footprint.h"
#include "bounding_box.h"
#include <stdarg.h>

typedef unsigned char subpixel;

namespace CVpro {
    void text(float x, float y, const char *text, ...);
    void text_align(float x, float y, char align, const char *text, ...);
    void autotext(float x, float y, char align, float spacing, const char *text, ...);
    void color(int r, int g, int b);
    void color(int r, int g, int b, int a);
    typedef unsigned char subpixel;
    class image
    {
        public:
            int width;
            int height;
            subpixel *matrix;

            image(int width, int height, subpixel *matrix);
            Bounding_Box display_bitmap(float x, float y, Footprint fp);
            void display_bitmap(float x, float y, float scale);
            // Bounding_Box display_bitmap(float x, float y, float scale, Footprint fp);
            void display_bitmap(float x, float y, float scale, float angle);
            Bounding_Box display_bitmap(float x, float y, float angle, Footprint fp);
            void destroy_bitmap();
            void flip_bitmap(char axis);
    };

    image *load_bitmap(const char *path);
}

#endif
