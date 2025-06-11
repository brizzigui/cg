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
#include <stdarg.h>

typedef unsigned char subpixel;

namespace CVpro {
    class image
    {
        public:
            subpixel *matrix;
            int width, height, bytes = 4;
            image(int width, int height);
            ~image();

            void display();
            void display(int x, int y);
            void clear();
    };
    
    void text(float x, float y, const char *text, ...);
    void text_align(float x, float y, char align, const char *text, ...);
    void autotext(float x, float y, char align, float spacing, const char *text, ...);
    void color(int r, int g, int b);
    void color(int r, int g, int b, int a);
    CVpro::image *load_bitmap(const char *path);

}

#endif
