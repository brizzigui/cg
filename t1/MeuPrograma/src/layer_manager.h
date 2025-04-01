#ifndef LAYER_MANAGER_H
#define LAYER_MANAGER_H

#include "gl_canvas2d.h"
#include "canvas_pro.h"

#include <chrono>
#include <vector>
#include <thread>
#include <mutex>

struct Layer
{
    CVpro::image *image;
    int anchorX;
    int anchorY;
    const char *name;
    bool visible;
};

class Layer_Manager
{
    public:
        std::mutex thread_mutex;
        std::vector<Layer> layers;
        CVpro::image *result;
        CVpro::image *background;

        CVpro::image *bottom;
        CVpro::image *mid;
        CVpro::image *top;

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

        void update_display_buffer()
        {
            
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
                        else if (layers_cpy[l].visible && is_valid_pixel(layers_cpy[l], i, j))
                        {
                            float r, g, b, a;
                            int base_index = (i-layers_cpy[l].anchorY) * layers_cpy[l].image->width * 4 + (j-layers_cpy[l].anchorX) * 4;

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
            if (active_index >= layers.size())
            {
                return false;
            }

            return true;
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
            update_display_buffer();
        }

        void switch_order(int l_origin, int l_target)
        {
            if (l_target >= layers.size() || l_target < 0)
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
};


#endif