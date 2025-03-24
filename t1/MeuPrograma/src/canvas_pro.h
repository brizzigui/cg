/*
    Não quer codar na Canvas?
    Use a biblioteca CVPro e vire o PRO em CG

    Texto rápido, cores inteiras, imagens .bmp nativas:
    vire PRO agora!
*/

#ifndef CANVAS_PRO_H
#define CANVAS_PRO_H

#include "gl_canvas2d.h"
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
            void display_bitmap(float x, float y, float scale);
            void display_bitmap_anchored(float x, float y, float scale, char anchorX, char anchorY);
            void destroy_bitmap();
    };

    image *load_bitmap(const char *path);
}

#endif
