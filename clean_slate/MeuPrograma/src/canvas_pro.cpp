#include <string.h>
#include <stdarg.h>
#include <iostream>

#include "canvas_pro.h"

CVpro::image::image(int width, int height, subpixel *matrix)
{
    this->width = width;
    this->height = height;
    this->matrix = matrix;
}

void CVpro::image::display_bitmap(int x, int y, int scale)
{
    // still needs work!!!! very wrong!!!!!!!!!!
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            CVpro::color(matrix[i * width * 3 + j * 3 + 1], matrix[i * width * 3 + j * 3], matrix[i * width * 3 + j * 3 + 2]);
            CV::point(x+j, y-i+height);
        }
    }
    
}

void CVpro::image::destroy_bitmap()
{
    // todo
}


/*
Recebe coordenadas x e y e texto para formatação no padrão C.
Não ultrapassar buffer de 256 chars.
*/
void CVpro::text(float x, float y, const char *text, ...)
{
    char buffer[256];

    va_list args;
    va_start(args, text);
    vsnprintf(buffer, 256, text, args);

    CV::text(x, y, buffer);
}

/*
Recebe cor em rgb de inteiros e passa para o Canvas
*/
void CVpro::color(int r, int g, int b)
{
    CV::color(r/255.0, g/255.0, b/255.0);
}

/*
Recebe caminho de arquivo e retorna um ponteiro para CVpro::image
*/
CVpro::image *CVpro::load_bitmap(const char *path)
{
    FILE *descriptor = fopen(path, "rb");
    
    if (descriptor == NULL)
    {
        printf("Fatal: could not find image file. Check path.");
        exit(404);
    }

    // pega dimensões no header
    int width, height;
    fseek(descriptor, 18, SEEK_CUR);
    fread(&width, 4, 1, descriptor);
    fread(&height, 4, 1, descriptor);

    subpixel *matrix = (subpixel *)malloc(sizeof(subpixel) * width * height * 3);
    fseek(descriptor, 64, SEEK_SET);                    // pula o restante dos headers
    fread(matrix, 1, width * height * 3, descriptor);   // le a matriz de pixels em si

    return new CVpro::image(width, height, matrix);
}