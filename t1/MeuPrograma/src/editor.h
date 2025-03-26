#ifndef EDITOR_H
#define EDITOR_H

#include "layer_manager.h"
#include "interface.h"

class Editor
{
    private:
        // to keep track of changes, the editor needs to know the other classes
        Layer_Manager *layer_manager;
        Interface *interface;
        int mouse_last_x = 0;
        int mouse_last_y = 0;

        const char* get_selected_action()
        {
            for (int i = 0; i < interface->actions.size(); i++)
            {
                if (interface->actions[i].selected)
                {
                    return interface->actions[i].label;
                }
            }

            return NULL;
        }

        float dist(int x0, int y0, int x1, int y1)
        {
            return sqrt((double)((x0-x1)*(x0-x1) + (y0-y1)*(y0-y1)));
        }

        bool is_valid_pixel(Layer l, int i, int j)
        {
            return (j >= l.anchorX) &&
                   (j < l.anchorX + l.image->width) &&
                   (i >= l.anchorY) &&
                   (i < l.anchorY + l.image->height);
        }

        void pencil_to_bmp(Layer l, int x, int y)
        {
            int radius = 5;
            for (int offset_y = -radius; offset_y < radius; offset_y++)
            {
                for (int offset_x = -radius; offset_x < radius; offset_x++)
                {
                    if (offset_x * offset_x + offset_y * offset_y < radius * radius)
                    {
                        int actual_x = x + offset_x;
                        int actual_y = y + offset_y;
                        if (is_valid_pixel(l, actual_y, actual_x))
                        {
                            int base_index = (actual_y-l.anchorY) * l.image->width * 4 + (actual_x-l.anchorX) * 4;
                            l.image->matrix[base_index + 2] = (unsigned char)255;
                            l.image->matrix[base_index + 1] = (unsigned char)0;
                            l.image->matrix[base_index] = (unsigned char)0;
                            l.image->matrix[base_index + 3] = (unsigned char)255;
                        }
                    }
                }
            }
        }

        void pencil(int button, int x, int y, bool held)
        {            
            if (held)
            {
                Layer l = layer_manager->get_active_layer();

                int diff_x = x - mouse_last_x;
                int diff_y = y - mouse_last_y;

                int samples = std::max(std::abs(diff_x), std::abs(diff_y)) + 1;

                float step_x = (float)diff_x / samples;
                float step_y = (float)diff_y / samples;

                for (int subsample = 0; subsample < samples; subsample++)
                {
                    pencil_to_bmp(l, x - step_x*subsample, y - step_y*subsample);
                }           
            }
        }

        void eraser_to_bmp(Layer l, int x, int y)
        {
            int radius = 10;
            for (int offset_y = -radius; offset_y < radius; offset_y++)
            {
                for (int offset_x = -radius; offset_x < radius; offset_x++)
                {
                    if (offset_x * offset_x + offset_y * offset_y < radius * radius)
                    {
                        int actual_x = x + offset_x;
                        int actual_y = y + offset_y;
                        if (is_valid_pixel(l, actual_y, actual_x))
                        {
                            int base_index = (actual_y-l.anchorY) * l.image->width * 4 + (actual_x-l.anchorX) * 4;
                            l.image->matrix[base_index + 2] = (unsigned char)0;
                            l.image->matrix[base_index + 1] = (unsigned char)0;
                            l.image->matrix[base_index] = (unsigned char)0;
                            l.image->matrix[base_index + 3] = (unsigned char)0;
                        }
                    }
                }
            }
        }

        void eraser(int button, int x, int y, bool held)
        {
            if (held)
            {
                Layer l = layer_manager->get_active_layer();

                int diff_x = x - mouse_last_x;
                int diff_y = y - mouse_last_y;

                int samples = std::max(std::abs(diff_x), std::abs(diff_y));

                float step_x = (float)diff_x / samples;
                float step_y = (float)diff_y / samples;

                for (int subsample = 0; subsample < samples+1; subsample++)
                {
                    eraser_to_bmp(l, x - step_x*subsample, y - step_y*subsample);
                }           
            }
        }

        void brush_to_bmp(Layer l, int x, int y)
        {
            // to do
        }

        void brush(int button, int x, int y, bool held)
        {
            if (held)
            {
                Layer l = layer_manager->get_active_layer();

                int diff_x = x - mouse_last_x;
                int diff_y = y - mouse_last_y;

                int samples = std::max(std::abs(diff_x), std::abs(diff_y));

                float step_x = (float)diff_x / samples;
                float step_y = (float)diff_y / samples;

                for (int subsample = 0; subsample < samples+1; subsample++)
                {
                    brush_to_bmp(l, x - step_x*subsample, y - step_y*subsample);
                }           
            }
        }

        void process_update(const char *action, int button, int x, int y, bool held)
        {
            if (strcmp(action, "Pencil") == 0)
            {
                pencil(button, x, y, held);
            }

            else if (strcmp(action, "Brush") == 0)
            {
                brush(button, x, y, held);
            }

            else if (strcmp(action, "Eraser") == 0)
            {
                eraser(button, x, y, held);
            }
            
        }

        bool is_inside_area(int x, int y)
        {
            return (x >= layer_manager->anchorX) &&   
                    (x < layer_manager->anchorX + layer_manager->board_width) &&
                    (y >= layer_manager->anchorY) &&   
                    (y < layer_manager->anchorY + layer_manager->board_height);
        }
        
    public:
        Editor(Layer_Manager *layer_manager, Interface *interface)
        {
            this->layer_manager = layer_manager;
            this->interface = interface;
        }        

        void update_state(int button, int x, int y, bool held)
        {
            const char *action = get_selected_action();

            if (action != NULL && is_inside_area(x, y))
            {
                process_update(action, button, x-layer_manager->anchorX, y-layer_manager->anchorY, held);
                mouse_last_x = x-layer_manager->anchorX;
                mouse_last_y = y-layer_manager->anchorY;
            }
        }
};


#endif