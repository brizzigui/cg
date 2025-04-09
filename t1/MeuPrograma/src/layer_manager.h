#ifndef LAYER_MANAGER_H
#define LAYER_MANAGER_H

#include "gl_canvas2d.h"
#include "canvas_pro.h"
#include "color.h"
#include "clamp.h"

#include <chrono>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <cmath>

typedef struct bmp_header
{
    char signature[2];
    uint32_t file_size;
    uint32_t reserved;
    uint32_t offset;

    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t img_size;
    uint32_t x_px_m;
    uint32_t y_px_m;
    uint32_t colors_used;
    uint32_t colors_important;

} bmp_header;

struct Layer
{
    CVpro::image *image;
    int anchorX;
    int anchorY;
    const char *name;
    bool visible;
    float scale;
    int rotation;
    float sin;
    float cos;
};

class Layer_Manager
{
    public:
        std::mutex thread_mutex;
        std::vector<Layer> layers;
        CVpro::image *result;
        CVpro::image *background;

        // global layer counter for naming
        int counter = 1;

        // arbitrary board size
        // uses 4:3 aspect ratio
        // may change latter... or make dynamic
        int board_width = 720;
        int board_height = 540;

        int anchorX = 130, anchorY = 100;

        int active_index = 0;

        
        CVpro::image *generate_blank_bmp()
        {
            int bytes = 4; // (rgb + alpha = 4)
            subpixel *matrix = (subpixel *)calloc(1, sizeof(subpixel) * board_width * board_height * bytes);
            CVpro::image *img = new CVpro::image(board_width, board_height, matrix);

            return img;
        }

        Layer_Manager()
        {
            this->result = generate_blank_bmp();
            this->background = generate_background();
        }

        void add_blank_layer()
        {
            thread_mutex.lock();

            Layer l;

            l.image = generate_blank_bmp();
            l.name = "Blank Layer";
            counter++;
            l.anchorX = 0;
            l.anchorY = 0;
            l.visible = true;
            l.scale = 1.0;
            l.rotation = 0;
            l.sin = sin(l.rotation * PI / 180.0);
            l.cos = cos(l.rotation * PI / 180.0);

            layers.push_back(l);
            set_active_layer(layers.size()-1);

            thread_mutex.unlock();
        }

        void add_bmp_layer(const char *path)
        {
            thread_mutex.lock();

            Layer l;

            l.image = CVpro::load_bitmap(path);
            l.name = "Image Layer";
            counter++;
            l.anchorX = 0;
            l.anchorY = 0; 
            l.visible = true;
            l.scale = 1.0;
            l.rotation = 0;
            l.sin = sin(l.rotation * PI / 180.0);
            l.cos = cos(l.rotation * PI / 180.0);

            layers.push_back(l);
            set_active_layer(layers.size()-1);

            thread_mutex.unlock();
        }

        CVpro::image *generate_background()
        {
            int bytes = 4; // (rgb + alpha = 4)
            subpixel *matrix = (subpixel *)calloc(1, sizeof(subpixel) * board_width * board_height * bytes);
            CVpro::image *img = new CVpro::image(board_width, board_height, matrix);
            
            int square_size = 16;

            for (int i = 0; i < board_height; i++)
            {
                for (int j = 0; j < board_width; j++)
                {
                    int base_index = i * board_width * 4 + j * 4;
                    int v;
                    
                    if ((i / square_size) % 2 == 0)
                    {
                        if ((j / square_size) % 2 == 0)
                        {
                            v = 192;
                        }
                        
                        else
                        {
                            v = 128;
                        }
                    }

                    else
                    {
                        if ((j / square_size) % 2 != 0)
                        {
                            v = 192;
                        }
                        
                        else
                        {
                            v = 128;
                        }
                    }   
                    
                    matrix[base_index + 2] = (unsigned char)v;
                    matrix[base_index + 1] = (unsigned char)v;
                    matrix[base_index    ] = (unsigned char)v;
                    matrix[base_index + 3] = (unsigned char)255;
                }
            }

            return img;
        }

        bool is_valid_pixel(Layer l, int i, int j)
        {
            return (j >= l.anchorX) &&
                   (j < l.anchorX + l.image->width) &&
                   (i >= l.anchorY) &&
                   (i < l.anchorY + l.image->height);
        }

        static std::pair<int, int> translate_scale_rotation(Layer l, int display_x, int display_y)
        {
            if (l.rotation == 0)
            {
                return std::make_pair(round((display_x-l.anchorX)/l.scale+l.anchorX), round((display_y-l.anchorY)/l.scale+l.anchorY));
            }
                        
            int center_x = l.anchorX + (l.image->width*l.scale)/2.0;
            int center_y = l.anchorY + (l.image->height*l.scale)/2.0;

            float offset_x = display_x - center_x;
            float offset_y = display_y - center_y;

            int sourceX =  offset_x * l.cos + offset_y * l.sin + center_x;
            int sourceY = -offset_x * l.sin + offset_y * l.cos + center_y;

            return std::make_pair(round((sourceX-l.anchorX)/l.scale+l.anchorX), round((sourceY-l.anchorY)/l.scale+l.anchorY));
        }

        void flatten()
        {
            // modifies a CVpro::image (var result) as a result of color blending using alpha channel
            // allows accurate display and accurate export
            thread_mutex.lock();
            auto layers_cpy = layers;
            thread_mutex.unlock();

            for (int i = 0; i < board_height; i++)
            {
                for (int j = 0; j < board_width; j++)
                {
                    float r_out = 0.0, g_out = 0.0, b_out = 0.0, a_out = 0.0;

                    // why the fuck -1? good question!
                    // -1 represents the background, so its always there!

                    for (int l = -1; l < (int)layers_cpy.size(); l++)
                    {
                        // pseudolayer background
                        if (l == -1)
                        {
                            float r, g, b, a;
                            int base_index = i * background->width * 4 + j * 4;

                            r = background->matrix[base_index + 2]/255.0;
                            g = background->matrix[base_index + 1]/255.0;
                            b = background->matrix[base_index]/255.0;
                            a = background->matrix[base_index + 3]/255.0;

                            r_out = r * a + r_out * (1 - a);
                            g_out = g * a + g_out * (1 - a);
                            b_out = b * a + b_out * (1 - a);
                            a_out = a + a_out * (1 - a);

                        }

                        // actual layers
                        else if (layers_cpy[l].visible)
                        {
                            std::pair<int, int> actual_coords = translate_scale_rotation(layers_cpy[l], j, i);
                            int actual_i = actual_coords.second;
                            int actual_j = actual_coords.first;
                            
                            if (is_valid_pixel(layers_cpy[l], actual_i, actual_j))
                            {
                                int base_index = (actual_i-layers_cpy[l].anchorY) * layers_cpy[l].image->width * 4 + (actual_j-layers_cpy[l].anchorX) * 4;

                                float r, g, b, a;

                                r = layers_cpy[l].image->matrix[base_index + 2]/255.0;
                                g = layers_cpy[l].image->matrix[base_index + 1]/255.0;
                                b = layers_cpy[l].image->matrix[base_index]/255.0;
                                a = layers_cpy[l].image->matrix[base_index + 3]/255.0;
    
                                r_out = r * a + r_out * (1 - a);
                                g_out = g * a + g_out * (1 - a);
                                b_out = b * a + b_out * (1 - a);
                                a_out = a + a_out * (1 - a);
                            }
                        }
                    }
                    
                    int base_index = i * board_width * 4 + j * 4;
                    result->matrix[base_index + 2] = (unsigned char)(r_out*255);
                    result->matrix[base_index + 1] = (unsigned char)(g_out*255);
                    result->matrix[base_index] = (unsigned char)(b_out*255);
                    result->matrix[base_index + 3] = (unsigned char)(a_out*255);
                }
            }
        }

        void flatten_worker()
        {
            // async worker
            // calls flatten() in separate thread

            while(true)
            {
                flatten();
                Sleep(0);
            }
        }

        void display()
        {
            // using own blend from flatten();
            // is being called using async thread for increased performance
            result->display_bitmap(anchorX, anchorY, 1.0);
        }

        bool is_valid()
        {
            if (active_index >= (int)layers.size() || (int)layers.size() <= 0)
            {
                return false;
            }

            return true;
        }

        Layer *get_active_layer_ptr()
        {
            if (!is_valid())
            {
                std::cout << "\n\nIndex not found exception:\n\tCould not resolve active_index layer at 'Layer get_active_layer()'.\n\tContinuing would result in segmentation fault. Aborting.\n" << std::endl;
                exit(1);
            }
            
            return &layers[active_index];
        }

        Layer get_active_layer()
        {
            if (!is_valid())
            {
                std::cout << "\n\nIndex not found exception:\n\tCould not resolve active_index layer at 'Layer get_active_layer()'.\n\tContinuing would result in segmentation fault. Aborting.\n" << std::endl;
                exit(1);
            }
            
            return layers[active_index];
        }

        void set_active_layer(int active_layer)
        {
            active_index = active_layer;
        }

        void switch_order(int l_origin, int l_target)
        {
            if (l_target >= (int)layers.size() || l_target < 0)
            {
                return;
            }

            else
            {
                Layer tmp = layers[l_origin];
                layers[l_origin] = layers[l_target];
                layers[l_target] = tmp;
            }
        }

        void safe_delete_layer(int layer_id)
        {
            // thread sensitive operation locks all other threads
            thread_mutex.lock();

            layers.erase(layers.begin() + layer_id);
            if (layer_id == active_index)
            {
                active_index = layers.size()-1;
            }

            thread_mutex.unlock();

            // theres a memory leak here, but solving it directly can endanger the thread
            // so i wont - for now.
        }

        void flip_active_horizontal()
        {
            if (is_valid())
            {
                Layer l = get_active_layer();
                l.image->flip_bitmap('h');
            }
        }

        void flip_active_vertical()
        {
            if (is_valid())
            {
                Layer l = get_active_layer();
                l.image->flip_bitmap('v');
            }
        }

        bool restore_project(char *path)
        {
            FILE *input = fopen(path, "rb");
            if (input == NULL)
            {
                return false;
            }
            
            uint32_t n_layers;
            fread(&n_layers, sizeof(uint32_t), 1, input);
            
            for (int i = 0; i < (int)n_layers; i++)
            {
                Layer l;

                int width, height;
                fread(&height, sizeof(int), 1, input);
                fread(&width, sizeof(int), 1, input);

                subpixel *mat = (subpixel *)malloc(sizeof(subpixel) * width * height * 4);
                fread(mat, sizeof(subpixel), 4 * height * width, input);
                CVpro::image *img = new CVpro::image(width, height, mat);

                l.image = img;
                
                fread(&l.anchorX, sizeof(int), 1, input);
                fread(&l.anchorY, sizeof(int), 1, input);
                fread(&l.visible, sizeof(bool), 1, input);
                fread(&l.scale, sizeof(int), 1, input);
                fread(&l.rotation, sizeof(int), 1, input);
                l.sin = sin(l.rotation * PI / 180.0);
                l.cos = cos(l.rotation * PI / 180.0);

                uint32_t name_len;
                fread(&name_len, sizeof(uint32_t), 1, input);
                
                char tmp[256];
                fread(tmp, sizeof(char), name_len, input);
                tmp[name_len] = '\0';
                const char *name = (strcmp(tmp, "Blank Layer") == 0) ? "Blank Layer" : "Image Layer";
                l.name = name;

                layers.push_back(l);
            }

            fclose(input);

            return true;
        }

        bool save_project(char *path)
        {
            FILE *output = fopen(path, "wb");
            if (output == NULL)
            {
                return false;
            }
            
            uint32_t n_layers = layers.size();
            fwrite(&n_layers, sizeof(uint32_t), 1, output);

            for (int i = 0; i < (int)n_layers; i++)
            {
                fwrite(&layers[i].image->height, sizeof(int), 1, output);
                fwrite(&layers[i].image->width, sizeof(int), 1, output);
                fwrite(layers[i].image->matrix, sizeof(subpixel), 4 * layers[i].image->height * layers[i].image->width, output);
                fwrite(&layers[i].anchorX, sizeof(int), 1, output);
                fwrite(&layers[i].anchorY, sizeof(int), 1, output);
                fwrite(&layers[i].visible, sizeof(bool), 1, output);
                fwrite(&layers[i].scale, sizeof(int), 1, output);
                fwrite(&layers[i].rotation, sizeof(int), 1, output);

                uint32_t name_len = strlen(layers[i].name);
                fwrite(&name_len, sizeof(uint32_t), 1, output);
                fwrite(layers[i].name, sizeof(char), name_len, output);
            }

            return true;
        }

        bmp_header create_header()
        {
            bmp_header h; 

            h.signature[0] = 'B';
            h.signature[1] = 'M'; 
            h.file_size = (54 + result->width*result->height*4);
            h.reserved = 0;
            h.offset = 54;

            h.size = 40;
            h.width = result->width;
            h.height = result->height;
            h.planes = 1;
            h.bit_count = 32;
            h.compression = 0;
            h.img_size = 0;
            h.x_px_m = 0;
            h.y_px_m = 0;

            h.colors_used = 256;
            h.colors_important = 0;

            return h;
        }

        CVpro::image *create_export_input()
        {
            CVpro::image *export_input = generate_blank_bmp();

            for (int i = 0; i < board_height; i++)
            {
                for (int j = 0; j < board_width; j++)
                {
                    float r_out = 0.0, g_out = 0.0, b_out = 0.0, a_out = 0.0;

                    for (int l = 0; l < (int)layers.size(); l++)
                    {
                        // actual layers
                        if(layers[l].visible && is_valid_pixel(layers[l], i, j))
                        {
                            float r, g, b, a;
                            int base_index = (i-layers[l].anchorY) * layers[l].image->width * 4 + (j-layers[l].anchorX) * 4;

                            r = layers[l].image->matrix[base_index + 2]/255.0;
                            g = layers[l].image->matrix[base_index + 1]/255.0;
                            b = layers[l].image->matrix[base_index]/255.0;
                            a = layers[l].image->matrix[base_index + 3]/255.0;

                            r_out = r * a + r_out * (1 - a);
                            g_out = g * a + g_out * (1 - a);
                            b_out = b * a + b_out * (1 - a);
                            a_out = a + a_out * (1 - a);
                        }

                    }
                    
                    int base_index = i * board_width * 4 + j * 4;
                    export_input->matrix[base_index + 2] = (unsigned char)(r_out*255);
                    export_input->matrix[base_index + 1] = (unsigned char)(g_out*255);
                    export_input->matrix[base_index] = (unsigned char)(b_out*255);
                    export_input->matrix[base_index + 3] = (unsigned char)(a_out*255);
                }
            }

            return export_input;
        }

        bool export_image(char *path)
        {
            bmp_header h = create_header(); 
            CVpro::image *input = create_export_input();

            FILE *output = fopen(path, "wb");
            if (output == NULL)
            {
                return false;
            }
            
            fwrite(h.signature, sizeof(char), 2, output);
            fwrite(&h.file_size, sizeof(uint32_t), 1, output);
            fwrite(&h.reserved, sizeof(uint32_t), 1, output);
            fwrite(&h.offset, sizeof(uint32_t), 1, output);

            fwrite(&h.size, sizeof(uint32_t), 1, output);
            fwrite(&h.width, sizeof(uint32_t), 1, output);
            fwrite(&h.height, sizeof(uint32_t), 1, output);
            fwrite(&h.planes, sizeof(uint16_t), 1, output);
            fwrite(&h.bit_count, sizeof(uint16_t), 1, output);
            fwrite(&h.compression, sizeof(uint32_t), 1, output);
            fwrite(&h.img_size, sizeof(uint32_t), 1, output);
            fwrite(&h.x_px_m, sizeof(uint32_t), 1, output);
            fwrite(&h.y_px_m, sizeof(uint32_t), 1, output);           

            fwrite(&h.colors_used, sizeof(uint32_t), 1, output);           
            fwrite(&h.colors_important, sizeof(uint32_t), 1, output); 
            
            for (int i = 0; i < input->height; i++)
            {
                for (int j = 0; j < input->width; j++)
                {
                    fwrite(input->matrix + (input->height-i-1) * input->width * 4 + j * 4, sizeof(subpixel) * 4, 1, output);
                }
            }

            fclose(output);
            free(input);

            return true;
        }

        // does a shallow copy of a CVpro::image
        CVpro::image *copy_image(CVpro::image *old_img)
        {
            subpixel *new_mat = (subpixel *)malloc(sizeof(subpixel) * old_img->width * old_img->height * 4);
            memcpy(new_mat, old_img->matrix, sizeof(subpixel) * old_img->width * old_img->height * 4);

            CVpro::image *new_img = new CVpro::image(old_img->width, old_img->height, new_mat);
            return new_img;
        }

        CVpro::image *create_grayscale()
        {
            CVpro::image *copy = copy_image(layers[active_index].image);
            for (int i = 0; i < copy->height; i++)
            {
                for (int j = 0; j < copy->width; j++)
                {
                    int base_index = i * copy->width * 4 + j * 4;
                    int luminance = (
                        copy->matrix[base_index] + copy->matrix[base_index + 1] + copy->matrix[base_index + 2]
                    )/3;

                    copy->matrix[base_index] = (unsigned char)luminance;
                    copy->matrix[base_index + 1] = (unsigned char)luminance;
                    copy->matrix[base_index + 2] = (unsigned char)luminance;
                }
            }

            return copy;
        }

        CVpro::image *create_sepia()
        {
            CVpro::image *copy = copy_image(layers[active_index].image);
            for (int i = 0; i < copy->height; i++)
            {
                for (int j = 0; j < copy->width; j++)
                {
                    int base_index = i * copy->width * 4 + j * 4;
                    int red_out = (copy->matrix[base_index + 2] * 0.393) + (copy->matrix[base_index + 1] * 0.769) + (copy->matrix[base_index] * 0.189);
                    int green_out = (copy->matrix[base_index + 2] * 0.349) + (copy->matrix[base_index + 1] * 0.686) + (copy->matrix[base_index] * 0.168);
                    int blue_out = (copy->matrix[base_index + 2] * 0.272) + (copy->matrix[base_index + 1] * 0.534) + (copy->matrix[base_index] * 0.131);

                    red_out = std::min(255, std::max(0, red_out));
                    green_out = std::min(255, std::max(0, green_out));
                    blue_out = std::min(255, std::max(0, blue_out));                    

                    copy->matrix[base_index + 2] = (unsigned char)red_out;
                    copy->matrix[base_index + 1] = (unsigned char)green_out;
                    copy->matrix[base_index] = (unsigned char)blue_out;
                }
            }

            return copy;
        }

        // returns a gaussian kernel
        // sigma is the sd
        std::vector<std::vector<double>> generate_gaussian_kernel(int size, double sigma)
        {
            int radius = size/2;
            double sum = 0.0;

            std::vector<std::vector<double>> kernel;

            for (int i = -radius; i <= radius; i++)
            {
                kernel.push_back(std::vector<double>());
                for (int j = -radius; j <= radius; j++)
                {
                    double v = (1/(2*PI*sigma*sigma))*exp(-(j*j + i*i)/(2*sigma*sigma));
                    kernel[i + radius].push_back(v);
                    sum += v;
                }
            }

            for (int i = 0; i < size; i++) 
            {
                for (int j = 0; j < size; j++) 
                {
                    kernel[i][j] /= sum;
                }
            }
            
            return kernel;
        }

        CVpro::image *create_gauss()
        {
            CVpro::image *copy = copy_image(layers[active_index].image);

            int sigma = 3;
            std::vector<std::vector<double>> kernel = generate_gaussian_kernel(sigma * 3, sigma);   // size = 3*sigma is what i read is best
            
            int radius = (sigma*3)/2;

            for (int i = 0; i < copy->height; i++)
            {
                for (int j = 0; j < copy->width; j++)
                {
                    double out_r, out_g, out_b, out_a;
                    out_r = out_g = out_b = out_a = 0;

                    for (int ki = -radius; ki <= radius; ki++)
                    {
                        for (int kj = -radius; kj <= radius; kj++)
                        {
                            int ni = clamp(i + ki, 0, copy->height - 1);
                            int nj = clamp(j + kj, 0, copy->width - 1);
                            int base_index = ni * copy->width * 4 + nj * 4;
                            double weight = kernel[ki + radius][kj + radius];
                            
                            out_r += layers[active_index].image->matrix[base_index + 2] * weight;
                            out_g += layers[active_index].image->matrix[base_index + 1] * weight;
                            out_b += layers[active_index].image->matrix[base_index + 0] * weight;
                            out_a += layers[active_index].image->matrix[base_index + 3] * weight;
                        }
                    }
                    
                    int base_index = i * copy->width * 4 + j * 4;
                    copy->matrix[base_index + 2] = (unsigned char)clamp((int)out_r, 0, 255);
                    copy->matrix[base_index + 1] = (unsigned char)clamp((int)out_g, 0, 255);
                    copy->matrix[base_index + 0] = (unsigned char)clamp((int)out_b, 0, 255);
                    copy->matrix[base_index + 3] = (unsigned char)clamp((int)out_a, 0, 255);                    
                }
                
            }
            
            return copy;
        }

        CVpro::image *create_vivid()
        {
            CVpro::image *copy = copy_image(layers[active_index].image);
            Color c;
            for (int i = 0; i < copy->height; i++)
            {
                for (int j = 0; j < copy->width; j++)
                {
                    int base_index = i * copy->width * 4 + j * 4;
                    c.set_from_rgb(copy->matrix[base_index + 2], copy->matrix[base_index + 1], copy->matrix[base_index], copy->matrix[base_index + 3]);
                    c.set_from_hsv(c.h, (((c.s * 1.3) > 1.0) ? 1.0 : c.s*1.3), c.v, c.a);

                    int red_out = c.r;
                    int green_out = c.g;
                    int blue_out = c.b;                

                    copy->matrix[base_index + 2] = (unsigned char)red_out;
                    copy->matrix[base_index + 1] = (unsigned char)green_out;
                    copy->matrix[base_index] = (unsigned char)blue_out;
                }
            }

            return copy;
        }

        std::vector<CVpro::image *> create_previews()
        {
            if (!is_valid())
            {
                return std::vector<CVpro::image *>();
            }
            
            std::vector<CVpro::image *> previews;
            previews.push_back(create_grayscale());
            previews.push_back(create_sepia());
            previews.push_back(create_gauss());
            previews.push_back(create_vivid());  
            
            return previews;
        }

        CVpro::image *create_adjustments_preview()
        {
            CVpro::image *copy = copy_image(get_active_layer().image);            
            return copy;
        }

        void adjust_brightness(CVpro::image *img, int brightness)
        {
            Color tmp;
            for (int i = 0; i < img->height; i++)
            {
                for (int j = 0; j < img->width; j++)
                {
                    int base_index = i * img->width * 4 + j * 4;
                    tmp.set_from_rgb(img->matrix[base_index + 2], img->matrix[base_index + 1], img->matrix[base_index], img->matrix[base_index + 3]);
                    tmp.set_from_hsv(tmp.h, tmp.s, clamp(tmp.v + brightness/200.0, 0.0, 1.0), tmp.a);
                    // dividing by 200 makes our scale a little more usable

                    img->matrix[base_index + 2] = (unsigned char)tmp.r;
                    img->matrix[base_index + 1] = (unsigned char)tmp.g;
                    img->matrix[base_index    ] = (unsigned char)tmp.b;
                }
            }
        }

        void adjust_contrast(CVpro::image *img, int contrast)
        {
            pixel tmp;
            for (int i = 0; i < img->height; i++)
            {
                for (int j = 0; j < img->width; j++)
                {
                    int base_index = i * img->width * 4 + j * 4;

                    tmp.r = clamp((img->matrix[base_index + 2]-128) * (1 + contrast/200.0) + 128, 0.0, 255.0);
                    tmp.g = clamp((img->matrix[base_index + 1]-128) * (1 + contrast/200.0) + 128, 0.0, 255.0);
                    tmp.b = clamp((img->matrix[base_index + 0]-128) * (1 + contrast/200.0) + 128, 0.0, 255.0);
                    // dividing by 200 makes our scale a little more usable

                    img->matrix[base_index + 2] = (unsigned char)tmp.r;
                    img->matrix[base_index + 1] = (unsigned char)tmp.g;
                    img->matrix[base_index    ] = (unsigned char)tmp.b;
                }
            }
        }

        int get_sign(int x)
        {
            return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
        }

        void adjust_saturation(CVpro::image *img, int saturation)
        {
            Color tmp;
            for (int i = 0; i < img->height; i++)
            {
                for (int j = 0; j < img->width; j++)
                {
                    int base_index = i * img->width * 4 + j * 4;
                    tmp.set_from_rgb(img->matrix[base_index + 2], img->matrix[base_index + 1], img->matrix[base_index], img->matrix[base_index + 3]);
                    tmp.set_from_hsv(tmp.h, clamp(tmp.s + pow(saturation/100.0, 2) * get_sign(saturation), 0.0, 1.0), tmp.v, tmp.a);
                    // squaring makes it more usable

                    img->matrix[base_index + 2] = (unsigned char)tmp.r;
                    img->matrix[base_index + 1] = (unsigned char)tmp.g;
                    img->matrix[base_index    ] = (unsigned char)tmp.b;
                }
            }
        }

        void adjust_temperature(CVpro::image *img, int temperature)
        {
            pixel tmp;
            for (int i = 0; i < img->height; i++)
            {
                for (int j = 0; j < img->width; j++)
                {
                    int base_index = i * img->width * 4 + j * 4;

                    tmp.r = clamp(img->matrix[base_index + 2] * (1 + temperature/200.0), 0.0, 255.0);
                    tmp.b = clamp(img->matrix[base_index + 0] * (1 - temperature/200.0), 0.0, 255.0);

                    img->matrix[base_index + 2] = (unsigned char)tmp.r;
                    img->matrix[base_index    ] = (unsigned char)tmp.b;
                }
            }
        }

        void adjust_gamma(CVpro::image *img, double gamma)
        {
            pixel tmp;
            for (int i = 0; i < img->height; i++)
            {
                for (int j = 0; j < img->width; j++)
                {
                    int base_index = i * img->width * 4 + j * 4;

                    tmp.r = clamp(pow((double)img->matrix[base_index + 2]/255.0, 1/gamma)*255, 0.0, 255.0);
                    tmp.g = clamp(pow((double)img->matrix[base_index + 1]/255.0, 1/gamma)*255, 0.0, 255.0);
                    tmp.b = clamp(pow((double)img->matrix[base_index + 0]/255.0, 1/gamma)*255, 0.0, 255.0);

                    img->matrix[base_index + 2] = (unsigned char)tmp.r;
                    img->matrix[base_index + 1] = (unsigned char)tmp.g;
                    img->matrix[base_index    ] = (unsigned char)tmp.b;
                }
            }
        }

        void clean_non_malloc_copy(CVpro::image *old_img, CVpro::image *new_img)
        {
            memcpy(new_img->matrix, old_img->matrix, sizeof(subpixel) * old_img->width * old_img->height * 4);
        }

        CVpro::image *create_adjustments_preview(CVpro::image *img, int brightness, int contrast, int saturation, int temperature, double gamma_correction)
        {   
            clean_non_malloc_copy(get_active_layer().image, img);
            if (brightness != 0)
            {
                adjust_brightness(img, brightness);
            }
            if (contrast != 0)
            {
                adjust_contrast(img, contrast);
            }
            if (saturation != 0)
            {
                adjust_saturation(img, saturation);
            }
            if (temperature != 0)
            {
                adjust_temperature(img, temperature);
            }
            if (fabs(gamma_correction - 1.0) > 0.01)
            {
                adjust_gamma(img, gamma_correction);
            }
            
            return img;
        }
};


#endif