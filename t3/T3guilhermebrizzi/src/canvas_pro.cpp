#include <string.h>
#include <stdarg.h>
#include <iostream>
#include <vector>
#include <cstdint>

#include "canvas_pro.h"
#include "bounding_box.h"

CVpro::image::image(int width, int height, subpixel *matrix)
{
    this->width = width;
    this->height = height;
    this->matrix = matrix;
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


/*
Mostra na tela um CVpro::image, a partir das coordenadas x, y.
Para manter tamanho original, passe 'scale' como 1.
Senão, passe um float com a escala desejada.
*/
void CVpro::image::display_bitmap(float x, float y, float scale)
{
    for (int i = 0; i < (int)(height*scale); i++)
    {
        for (int j = 0; j < (int)(width*scale); j++)
        {
            int base_index = (int)(i/scale) * width * 4 + (int)(j/scale) * 4;
            CVpro::color(matrix[base_index + 2], matrix[base_index + 1], matrix[base_index], matrix[base_index + 3]);
            CV::point(x+j, y+i);
        }
    }
}

/*
Mostra na tela um CVpro::image, a partir das coordenadas x, y.
Para manter tamanho original, passe 'scale' como 1.
Senão, passe um float com a escala desejada.
*/
Bounding_Box CVpro::image::display_bitmap(float x, float y, float scale, Footprint fp)
{
    Bounding_Box box;
    for (int i = 0; i < (int)(height*scale); i++)
    {
        for (int j = 0; j < (int)(width*scale); j++)
        {
            int base_index = (int)(i/scale) * width * 4 + (int)(j/scale) * 4;
            CVpro::color(matrix[base_index + 2], matrix[base_index + 1], matrix[base_index], matrix[base_index + 3]);
            CV::point(x+j, y+i);
            if(matrix[base_index + 3] > 0)
            {
                fp.mark_pixel(x+j, y+i);
                box.update(x+j, y+i);
            }
        }
    }
    return box;
}


void CVpro::image::display_bitmap(float x, float y, float scale, float angle)
{
    int max_dim = height + width;
    
    for (int y_dst = - max_dim * scale; y_dst < max_dim * scale; y_dst++) 
    {
        for (int x_dst = - max_dim * scale; x_dst < max_dim * scale; x_dst++) 
        {
            float x_src = (cos(-angle) * x_dst - sin(-angle) * y_dst) / scale;
            float y_src = (sin(-angle) * x_dst + cos(-angle) * y_dst) / scale;
    
            int i = (int)y_src + height/2.0;
            int j = (int)x_src + width/2.0;
    
            if (i >= 0 && i < height && j >= 0 && j < width) 
            {
                int base_index = i * width * 4 + j * 4;
                CVpro::color(matrix[base_index + 2], matrix[base_index + 1], matrix[base_index], matrix[base_index + 3]);
                CV::point(x + x_dst, y + y_dst);
            }
        }
    }
}

Bounding_Box CVpro::image::display_bitmap(float x, float y, float scale, float angle, Footprint fp)
{
    Bounding_Box box;

    int max_dim = height + width;
    
    for (int y_dst = - max_dim * scale; y_dst < max_dim * scale; y_dst++) 
    {
        for (int x_dst = - max_dim * scale; x_dst < max_dim * scale; x_dst++) 
        {
            float x_src = (cos(-angle) * x_dst - sin(-angle) * y_dst) / scale;
            float y_src = (sin(-angle) * x_dst + cos(-angle) * y_dst) / scale;
    
            int i = (int)y_src + height/2.0;
            int j = (int)x_src + width/2.0;
    
            if (i >= 0 && i < height && j >= 0 && j < width) 
            {
                int base_index = i * width * 4 + j * 4;
                CVpro::color(matrix[base_index + 2], matrix[base_index + 1], matrix[base_index], matrix[base_index + 3]);
                CV::point(x + x_dst, y + y_dst);
                if(matrix[base_index + 3] > 0)
                {
                    fp.mark_pixel(x + x_dst, y + y_dst);
                    box.update(x+j, y+i);
                }
            }
        }
    }
    return box;
}


/*
Libera memória da imagem.
Aviso: usar a imagem após liberação causará segmentation fault.
*/
void CVpro::image::destroy_bitmap()
{
    free(matrix);
}


void flip_vertical(CVpro::image *img)
{
    subpixel *tmp = (subpixel *)malloc(img->width * 4);
    for (int line = 0; line < img->height/2; line++)
    {
        memcpy(tmp, img->matrix + img->width * line * 4, img->width * 4);
        memcpy(img->matrix + img->width * line * 4, img->matrix + img->width * (img->height-line-1) * 4, img->width * 4);
        memcpy(img->matrix + img->width * (img->height-line-1) * 4, tmp, img->width * 4);
    }
    free(tmp);
}

void flip_horizontal(CVpro::image *img)
{
    subpixel tmp[4];
    for (int line = 0; line < img->height; line++)
    {
        for (int pixel = 0; pixel < img->width/2; pixel++)
        {
            memcpy(tmp, img->matrix + line * img->width * 4 + pixel * 4, 4);
            memcpy(img->matrix + line * img->width * 4 + pixel * 4, img->matrix + line * img->width * 4 + (img->width - pixel - 1) * 4, 4);
            memcpy(img->matrix + line * img->width * 4 + (img->width - pixel - 1) * 4, tmp, 4);
        }
    }
}

/*
Inverte a imagem.
Recebe um CVpro::image e espelha conforme o eixo pedido.
Eixos aceitos:
    - 'V' = vertical
    - 'H' = horizontal (default)
*/
void CVpro::image::flip_bitmap(char axis)
{
    if (axis == 'V' || axis == 'v')
    {
        flip_vertical(this);
    }

    else
    {
        flip_horizontal(this);
    }
    
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
    int width, height, bytes;
    uint16_t bits;

    fseek(descriptor, 18, SEEK_CUR);
    fread(&width, 4, 1, descriptor);
    fread(&height, 4, 1, descriptor);
    fseek(descriptor, 2, SEEK_CUR);
    fread(&bits, 2, 1, descriptor);
    bytes = bits/8;

    // número de bytes em uma linha (data + 4-indexing alignment)
    int stride = get_stride(width, bytes);
    int padding = stride - width * bytes;

    subpixel *matrix = (subpixel *)malloc(sizeof(subpixel) * width * height * (bytes+1));
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

    return new CVpro::image(width, height, matrix);
}