#include <string.h>
#include <stdarg.h>
#include <iostream>
#include <vector>

#include "canvas_pro.h"

CVpro::image::image(int width, int height, subpixel *matrix)
{
    this->width = width;
    this->height = height;
    this->matrix = matrix;
}

int get_stride(int width)
{
    int bytes_per_pixel = 3;  // number of bytes for 8 bit RGB pixels
    int alignment = 4;  // required byte alignment for BMP image rows

    // number of bytes in a row (round _up_ by alignment)
    int stride = (width * bytes_per_pixel) + (alignment - 1);
    stride /= alignment;
    stride *= alignment;

    return stride;
}

/*
Mostra na tela um CVpro::image, a partir das coordenadas x, y.
Para manter tamanho original, passe 'scale' como 1.
Senão, passe um float com a escala desejada.
*/
void CVpro::image::display_bitmap(float x, float y, float scale)
{
    int padding = 0;
    for (int i = 0; i < (int)(height*scale); i++)
    {
        for (int j = 0; j < (int)(width*scale); j++)
        {
            int base_index = (int)(i/scale) * width * 3 + (int)(j/scale) * 3;
            CVpro::color(matrix[base_index + 2], matrix[base_index + 1], matrix[base_index]);
            CV::point(x+j, y-i+height*scale);
        }
    }
}

/*
Mostra na tela um CVpro::image, a partir das coordenadas x, y.
Para manter tamanho original, passe 'scale' como 1.
Senão, passe um float com a escala desejada.

Tipo de âncora:
    - anchorX usa literais 'L', 'R' e 'C' para esquerda, direita e centro.
    - anchorY usa literais 'T', 'B' e 'C' para cima, baixo e centro.

Ancora posição conforme alinhamento em x e y.
*/
void CVpro::image::display_bitmap_anchored(float x, float y, float scale, char anchorX, char anchorY)
{
    if (anchorX == 'R' || anchorX == 'r')
    {
        x = x-width*scale;
    }

    else if (anchorX == 'C' || anchorX == 'c')
    {
        x = x-(width*scale)/2;
    }

    if (anchorY == 'B' || anchorY == 'b')
    {
        y = y-height*scale;
    }

    else if (anchorY == 'C' || anchorY == 'c')
    {
        y = y-(height*scale)/2;
    }
    
    display_bitmap(x, y, scale);
}

/*
Libera memória da imagem.
Aviso: usar a imagem após liberação causará segmentation fault.
*/
void CVpro::image::destroy_bitmap()
{
    free(matrix);
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
    for (int i = 0; i < strlen(buffer) + 1; i++)
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

/*
Recebe caminho de arquivo e retorna um ponteiro para CVpro::image
*/
CVpro::image *CVpro::load_bitmap(const char *path)
{
    FILE *descriptor = fopen(path, "rb");
    
    if (descriptor == NULL)
    {
        printf("Fatal: could not find image file at '%s'. Check path.", path);
        exit(404);
    }

    // pega dimensões no header
    int width, height, bytes = 3;
    fseek(descriptor, 18, SEEK_CUR);
    fread(&width, 4, 1, descriptor);
    fread(&height, 4, 1, descriptor);

    // número de bytes em uma linha (data + 4-indexing alignment)
    int stride = get_stride(width);
    int padding = stride - width * bytes;

    subpixel *matrix = (subpixel *)malloc(sizeof(subpixel) * width * height * bytes);
    fseek(descriptor, 54, SEEK_SET); // pula o restante dos headers

    for (int line = 0; line < height; line++)
    {
        fread(matrix + line * width * bytes, 1, width * bytes, descriptor);  // le a matriz de pixels em si, pulando padding
        fseek(descriptor, padding, SEEK_CUR);
    }

    return new CVpro::image(width, height, matrix);
}