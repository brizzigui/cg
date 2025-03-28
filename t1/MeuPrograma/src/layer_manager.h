#ifndef LAYER_MANAGER_H
#define LAYER_MANAGER_H

#include "gl_canvas2d.h"
#include "canvas_pro.h"

#include <chrono>
#include <vector>

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
        std::vector<Layer> layers;
        CVpro::image *result;

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
        }

        void add_blank_layer()
        {
            Layer l;

            l.image = generate_blank_bmp();
            l.name = "Blank Layer";
            counter++;
            l.anchorX = 0;
            l.anchorY = 0;
            l.visible = true;

            layers.push_back(l);
            active_index = layers.size()-1;
        }

        void add_bmp_layer(const char *path)
        {
            Layer l;

            l.image = CVpro::load_bitmap(path);
            l.name = "Image Layer";
            counter++;
            l.anchorX = 0;
            l.anchorY = 0; 
            l.visible = true;

            layers.push_back(l);
            active_index = layers.size()-1;
        }

        void display_background()
        {
            int square_height = 30;
            int square_width = 40;

            for (int i = 0; i < square_height; i++)
            {
                for (int j = 0; j < square_width; j++)
                {
                    if (j % 2 == 0 && i % 2 == 0 || j % 2 != 0 && i % 2 != 0)
                    {
                        CVpro::color(240, 240, 240);                    
                    }

                    else
                    {
                        CVpro::color(188, 188, 188);                    
                    }

                    int baseX = anchorX + j * (board_width / square_width);
                    int baseY = anchorY + i * (board_width / square_width);
                    CV::rectFill(baseX, baseY, baseX + (board_width / square_width), baseY + (board_width / square_width));             
                }
                
            }
            
        }

        bool is_valid_pixel(Layer l, int i, int j)
        {
            return (j >= l.anchorX) &&
                   (j < l.anchorX + l.image->width) &&
                   (i >= l.anchorY) &&
                   (i < l.anchorY + l.image->height);
        }

        void flatten()
        {
            // modifies a CVpro::image (var result) as a result of color blending using alpha channel
            // allows accurate display and accurate export

            for (int i = 0; i < board_height; i++)
            {
                for (int j = 0; j < board_width; j++)
                {
                    float r_out = 0.0, g_out = 0.0, b_out = 0.0, a_out = 0.0;

                    for (int l = 0; l < layers.size(); l++)
                    {
                        if (layers[l].visible && is_valid_pixel(layers[l], i, j))
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

        void display_layers()
        {
            // using own blend from flatten();
            // is being called using async thread for increased performance
            result->display_bitmap(anchorX, anchorY, 1.0);
            return;
        }

        void display()
        {
            //display_background();
            display_layers();
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
            layers.erase(layers.begin() + layer_id);
            if (layer_id == active_index)
            {
                active_index = layers.size()-1;
            }
        }
};


#endif