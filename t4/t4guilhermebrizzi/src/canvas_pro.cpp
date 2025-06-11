#include <string.h>
#include <stdarg.h>
#include <iostream>
#include <vector>
#include <cstdint>

#include "canvas_pro.h"

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
Recebe coordenadas x e y, tipo de alinhamento e texto para formatação no padrão C.

Tipo de alinhamento usa literais 'L', 'R' e 'C' para esquerda, direita e centro.

Ancora posição conforme alinhamento em x e y.
Não ultrapassar buffer de 256 chars.
*/
void CVpro::text_align(float x, float y, char align, const char *text, ...)
{
    char buffer[256];
    
    va_list args;
    va_start(args, text);
    vsnprintf(buffer, 256, text, args);
    
    int offset;
    if (align == 'R' || align == 'r')
    {
        offset = strlen(buffer) * 10;       // 10 é a largura padrão de texto na Canvas2D
    }

    else if(align == 'C' || align == 'c')
    {
        offset = strlen(buffer) * 10/2.0;   // 10 é a largura padrão de texto na Canvas2D
    }

    else
    {
        offset = 0;
    }

    CV::text(x-offset, y, buffer);
}

/*
Função auxiliar ao autotext usada apenas internamente. Imprime cada linha.
*/
void autotext_process_line(char *text, float x, float y, char align)
{
    int offset;
    if (align == 'R' || align == 'r')
    {
        offset = strlen(text) * 10;       // 10 é a largura padrão de texto na Canvas2D
    }

    else if(align == 'C' || align == 'c')
    {
        offset = strlen(text) * 10/2.0;   // 10 é a largura padrão de texto na Canvas2D
    }

    else
    {
        offset = 0;
    }

    CV::text(x-offset, y, text);
}

/*
Recebe múltiplas linhas de texto separado por \n e posiciona automaticamente.

Recebe coordenadas iniciais x e y, tipo de alinhamento e texto para formatação no padrão C.

Tipo de alinhamento usa literais 'L', 'R' e 'C' para esquerda, direita e centro.

Ancora posição conforme alinhamento em x e y.
Não ultrapassar buffer de 1024 chars.
*/
void CVpro::autotext(float x, float y, char align, float spacing, const char *text, ...)
{
    char buffer[1024];
    char tmp[1024];

    va_list args;
    va_start(args, text);
    vsnprintf(buffer, 1024, text, args);

    int k = 0;
    for (int i = 0; i < (int)strlen(buffer) + 1; i++)
    {
        if (buffer[i] == '\n' || buffer[i] == '\0')
        {            
            tmp[k] = '\0';
            autotext_process_line(tmp, x, y, align);
            y += spacing;
            k = 0;
        }

        else
        {
            tmp[k] = buffer[i];
            k++;
        }
    }
}

/*
Recebe cor em rgb de inteiros e passa para o Canvas
Todos valores entre [0, 255]
*/
void CVpro::color(int r, int g, int b)
{
    CV::color(r/255.0, g/255.0, b/255.0);
}

/*
Recebe cor em rgba de inteiros e passa para o Canvas
Todos valores entre [0, 255]
*/
void CVpro::color(int r, int g, int b, int a)
{
    CV::color(r/255.0, g/255.0, b/255.0, a/255.0);
}

CVpro::image::image(int width, int height)
{
    this->width = width;
    this->height = height;
    matrix = (subpixel *)calloc(width * height * bytes * sizeof(subpixel), 1);
}

CVpro::image::~image()
{
    free(matrix);
}

void CVpro::image::display(int x, int y)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int index = i * width * bytes + j * bytes;
            CVpro::color(matrix[index + 2], matrix[index + 1], matrix[index], 255);
            CV::rectFill(x+j, y+i, x+j+1, y+i+1);
        }
    }
}

void CVpro::image::display()
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int index = i * width * bytes + j * bytes;
            CVpro::color(matrix[index + 2], matrix[index + 1], matrix[index], 255);
            CV::rectFill(j, i, j+1, i+1);
        }
    }
}

void CVpro::image::clear()
{
    memset(matrix, 0, width * height * bytes * sizeof(subpixel));
}

int get_stride(int width, int bytes)
{
    int bytes_per_pixel = bytes;  // number of bytes for 8 bit RGB pixels
    int alignment = 4;  // required byte alignment for BMP image rows

    // number of bytes in a row (round _up_ by alignment)
    int stride = (width * bytes_per_pixel) + (alignment - 1);
    stride /= alignment;
    stride *= alignment;

    return stride;
}

CVpro::image *CVpro::load_bitmap(const char *path)
{
    FILE *descriptor = fopen(path, "rb");
    
    if (descriptor == NULL)
    {
        printf("Fatal: could not find image file at '%s'. Check path.", path);
        exit(404);
    }

    // pega dimensões no header
    int width, height, bytes;
    uint16_t bits;

    fseek(descriptor, 18, SEEK_CUR);
    fread(&width, 4, 1, descriptor);
    fread(&height, 4, 1, descriptor);
    fseek(descriptor, 2, SEEK_CUR);
    int read = fread(&bits, 2, 1, descriptor);
    bytes = bits/8;

    auto image = new CVpro::image(width, height);

    // número de bytes em uma linha (data + 4-indexing alignment)
    int stride = get_stride(width, bytes);
    int padding = stride - width * bytes;

    subpixel *matrix = image->matrix;
    fseek(descriptor, 54, SEEK_SET); // pula o restante dos headers

    for (int line = 0; line < height; line++)
    {
        for (int pixel = 0; pixel < width; pixel++)
        {
            if (bytes == 3)
            {
                subpixel *address = matrix + (height - line - 1) * width * (bytes + 1) + pixel * (bytes + 1);
                fread(address, 1, bytes, descriptor);  // le a matriz de pixels em si, pulando padding
                *(address + bytes) = (unsigned char)255;
            }

            else
            {
                subpixel *address = matrix + (height - line - 1) * width * bytes + pixel * bytes;
                fread(address, 1, bytes, descriptor);  // le a matriz de pixels em si, pulando padding
            }
        }
        
        fseek(descriptor, padding, SEEK_CUR);
    }

    return image;
}