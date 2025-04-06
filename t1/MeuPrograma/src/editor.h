#ifndef EDITOR_H
#define EDITOR_H

#include <queue>

#include "layer_manager.h"
#include "interface.h"
#include "color.h"
#include "shared.h"

class Editor
{
    private:
        // to keep track of changes, the editor needs to know the other classes
        Layer_Manager *layer_manager;
        Interface *interface;
        
        int mouse_last_x = 0;
        int mouse_last_y = 0;

        int grab_x = 0;
        int grab_y = 0;

        bool grabbed = false;

        Color eraser_color;

        Action* get_selected_action()
        {
            for (int i = 0; i < interface->actions.size(); i++)
            {
                if (interface->actions[i].selected)
                {
                    return &interface->actions[i];
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

        void move(int state, int button, int x, int y, bool held)
        {
            if (button == 0 && state == 0)
            {
                Layer *l = &layer_manager->layers[layer_manager->active_index];

                grabbed = true;
                grab_x = x-l->anchorX;
                grab_y = y-l->anchorY;
            }

            if (grabbed)
            {
                Layer *l = &layer_manager->layers[layer_manager->active_index];
                l->anchorX = x-grab_x; 
                l->anchorY = y-grab_y;   
            }
        }

        void circle_to_bmp(Layer l, int x, int y, int radius, Color color)
        {
            for (int offset_y = -radius; offset_y < radius; offset_y++)
            {
                for (int offset_x = -radius; offset_x < radius; offset_x++)
                {
                    if (offset_x * offset_x + offset_y * offset_y < radius * radius)
                    {
                        int actual_x = x + offset_x;
                        int actual_y = y + offset_y;

                        std::pair<int, int> actual_coords = Layer_Manager::translate_rotation(l, actual_x, actual_y);
                        actual_x = actual_coords.first;
                        actual_y = actual_coords.second;

                        if (is_valid_pixel(l, actual_y, actual_x))
                        {
                            int base_index = (actual_y-l.anchorY) * l.image->width * 4 + (actual_x-l.anchorX) * 4;
                            l.image->matrix[base_index + 2] = (unsigned char)color.r;
                            l.image->matrix[base_index + 1] = (unsigned char)color.g;
                            l.image->matrix[base_index] = (unsigned char)color.b;
                            l.image->matrix[base_index + 3] = (unsigned char)color.a;
                        }
                    }
                }
            }
        }

        void square_to_bmp(Layer l, int x, int y, int size, Color color)
        {
            int radius = size/2;
            for (int offset_y = -radius; offset_y < radius; offset_y++)
            {
                for (int offset_x = -radius; offset_x < radius; offset_x++)
                {
                    int actual_x = x + offset_x;
                    int actual_y = y + offset_y;

                    std::pair<int, int> actual_coords = Layer_Manager::translate_rotation(l, actual_x, actual_y);
                    actual_x = actual_coords.first;
                    actual_y = actual_coords.second;

                    if (is_valid_pixel(l, actual_y, actual_x))
                    {
                        int base_index = (actual_y-l.anchorY) * l.image->width * 4 + (actual_x-l.anchorX) * 4;
                        l.image->matrix[base_index + 2] = (unsigned char)color.r;
                        l.image->matrix[base_index + 1] = (unsigned char)color.g;
                        l.image->matrix[base_index] = (unsigned char)color.b;
                        l.image->matrix[base_index + 3] = (unsigned char)color.a;
                    }
                }
            }
        }

        void grid_to_bmp(Layer l, int x, int y, int size, Color color)
        {
            int square_x = x/size;
            int square_y = y/size;

            for (int start_y = square_y*size; start_y < (square_y+1)*size; start_y++)
            {
                for (int start_x = square_x*size; start_x < (square_x+1)*size; start_x++)
                {
                    int actual_x = start_x;
                    int actual_y = start_y;

                    std::pair<int, int> actual_coords = Layer_Manager::translate_rotation(l, actual_x, actual_y);
                    actual_x = actual_coords.first;
                    actual_y = actual_coords.second;

                    if (is_valid_pixel(l, actual_y, actual_x))
                    {
                        int base_index = (actual_y-l.anchorY) * l.image->width * 4 + (actual_x-l.anchorX) * 4;
                        l.image->matrix[base_index + 2] = (unsigned char)color.r;
                        l.image->matrix[base_index + 1] = (unsigned char)color.g;
                        l.image->matrix[base_index] = (unsigned char)color.b;
                        l.image->matrix[base_index + 3] = (unsigned char)color.a;
                    }
                }
            }
        }

        void pencil(Action *action, int button, int x, int y, bool held)
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
                    if (((std_selectable_values *)action->values_ptr)->type == SELECTABLE_TYPE_CIRCLE)
                    {
                        circle_to_bmp(l, x - step_x*subsample, y - step_y*subsample, ((std_selectable_values *)action->values_ptr)->size, active_color);
                    }

                    else if (((std_selectable_values *)action->values_ptr)->type == SELECTABLE_TYPE_SQUARE)
                    {
                        square_to_bmp(l, x - step_x*subsample, y - step_y*subsample, ((std_selectable_values *)action->values_ptr)->size, active_color);
                    }

                    else
                    {
                        grid_to_bmp(l, x - step_x*subsample, y - step_y*subsample, ((std_selectable_values *)action->values_ptr)->size, active_color);
                    }
                    
                }           
            }
        }

        void spray_to_bmp(Layer l, int x, int y, int radius)
        {
            for (int offset_y = -radius; offset_y < radius; offset_y++)
            {
                for (int offset_x = -radius; offset_x < radius; offset_x++)
                {
                    if (rand() % 100 == 0)
                    {
                        int actual_x = x + offset_x;
                        int actual_y = y + offset_y;

                        std::pair<int, int> actual_coords = Layer_Manager::translate_rotation(l, actual_x, actual_y);

                        actual_x = actual_coords.first;
                        actual_y = actual_coords.second;

                        if (is_valid_pixel(l, actual_y, actual_x))
                        {
                            int base_index = (actual_y-l.anchorY) * l.image->width * 4 + (actual_x-l.anchorX) * 4;
                            l.image->matrix[base_index + 2] = (unsigned char)active_color.r;
                            l.image->matrix[base_index + 1] = (unsigned char)active_color.g;
                            l.image->matrix[base_index] = (unsigned char)active_color.b;
                            l.image->matrix[base_index + 3] = (unsigned char)active_color.a;
                        }
                    }
                }
            }
        }

        void spray(Action *action, int button, int x, int y, bool held)
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
                    spray_to_bmp(l, x - step_x*subsample, y - step_y*subsample, ((std_selectable_values *)action->values_ptr)->size);
                }           
            }
        }


        void eraser(Action *action, int button, int x, int y, bool held)
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
                    if (((std_selectable_values *)action->values_ptr)->type == SELECTABLE_TYPE_CIRCLE)
                    {
                        circle_to_bmp(l, x - step_x*subsample, y - step_y*subsample, ((std_selectable_values *)action->values_ptr)->size, eraser_color);  
                    }

                    else if (((std_selectable_values *)action->values_ptr)->type == SELECTABLE_TYPE_SQUARE)
                    {
                        square_to_bmp(l, x - step_x*subsample, y - step_y*subsample, ((std_selectable_values *)action->values_ptr)->size, eraser_color);  
                    }
                    
                    else
                    {
                        grid_to_bmp(l, x - step_x*subsample, y - step_y*subsample, ((std_selectable_values *)action->values_ptr)->size, eraser_color);  
                    }
                }           
            }
        }

        void marker_to_bmp(Layer l, int x, int y, int size)
        {
            for (int i = 0; i < size; i++)
            {
                int actual_x = x;
                int actual_y = y + i - size/2.0;

                std::pair<int, int> actual_coords = Layer_Manager::translate_rotation(l, actual_x, actual_y);

                actual_x = actual_coords.first;
                actual_y = actual_coords.second;

                if (is_valid_pixel(l, actual_y, actual_x))
                {
                    int base_index = (actual_y-l.anchorY) * l.image->width * 4 + (actual_x-l.anchorX) * 4;
                    l.image->matrix[base_index + 2] = (unsigned char)active_color.r;
                    l.image->matrix[base_index + 1] = (unsigned char)active_color.g;
                    l.image->matrix[base_index] = (unsigned char)active_color.b;
                    l.image->matrix[base_index + 3] = (unsigned char)active_color.a;
                }
            }
        }

        void marker(Action *action, int button, int x, int y, bool held)
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
                    marker_to_bmp(l, x - step_x*subsample, y - step_y*subsample, ((std_selectable_values *)action->values_ptr)->size);
                }           
            }
        }

        bool fill_check_color(Layer l, int i, int j, pixel color)
        {
            int base_index = (i-l.anchorY) * l.image->width * 4 + (j-l.anchorX) * 4;
            return l.image->matrix[base_index + 2] == color.r &&
                    l.image->matrix[base_index + 1] == color.g &&
                    l.image->matrix[base_index] == color.b &&
                    l.image->matrix[base_index + 3] == color.a;
        }

        pixel fill_get_pixel_color(Layer l, int x, int y)
        {
            pixel start_color;
            int base_index = (y-l.anchorY) * l.image->width * 4 + (x-l.anchorX) * 4;
            start_color.r = l.image->matrix[base_index + 2];
            start_color.g = l.image->matrix[base_index + 1];
            start_color.b = l.image->matrix[base_index];
            start_color.a = l.image->matrix[base_index + 3];
            return start_color;
        }

        void fill(int state, int button, int x, int y, bool held)
        {
            Layer l = layer_manager->get_active_layer();
            // must have clicked and must be starting at a valid layer pixel
            if (!(state == 0 && button == 0)) return;

            std::pair<int, int> actual_coords = Layer_Manager::translate_rotation(l, x, y);
            x = actual_coords.first;
            y = actual_coords.second;

            if (!is_valid_pixel(l, y, x)) return;
            
            // can't start setting in color already equal to expected
            // removing this will cause bugs (infinite loop)!
            pixel start_color = fill_get_pixel_color(l, x, y);
            if (
                start_color.r == active_color.r &&
                start_color.g == active_color.g &&
                start_color.b == active_color.b &&
                start_color.a == active_color.a
            )
            {
                return;
            }
            
            
            std::queue<std::pair<int, int>> queue;
            queue.push(std::make_pair(y, x));

            while (queue.size() > 0)
            {
                std::pair<int, int> cur = queue.front();
                queue.pop();

                int i = cur.first;
                int j = cur.second;
                
                if (is_valid_pixel(l, i, j) && fill_check_color(l, i, j, start_color))
                {
                    int base_index = (i-l.anchorY) * l.image->width * 4 + (j-l.anchorX) * 4;
                    l.image->matrix[base_index + 2] = (unsigned char)active_color.r;
                    l.image->matrix[base_index + 1] = (unsigned char)active_color.g;
                    l.image->matrix[base_index] = (unsigned char)active_color.b;
                    l.image->matrix[base_index + 3] = (unsigned char)active_color.a;

                    queue.push(std::make_pair(i+1, j));
                    queue.push(std::make_pair(i+1, j+1));   
                    queue.push(std::make_pair(i-1, j));   
                    queue.push(std::make_pair(i-1, j-1));

                }
            }
            
        }

        void pick(int state, int button, int x, int y)
        {
            Layer l = layer_manager->get_active_layer();
            std::pair<int, int> actual_coords = Layer_Manager::translate_rotation(l, x, y);

            x = actual_coords.first;
            y = actual_coords.second;

            if (state == 0 && button == 0 && is_valid_pixel(l, y, x))
            {
                int base_index = (y-l.anchorY) * l.image->width * 4 + (x-l.anchorX) * 4;
                active_color.set_from_rgb(l.image->matrix[base_index + 2], l.image->matrix[base_index + 1], l.image->matrix[base_index], l.image->matrix[base_index + 3]);
            }   
        }

        void set_rotation(Layer* l, int rotation)
        {
            l->rotation++;
            l->sin = sin(l->rotation * PI / 180.0);
            l->cos = cos(l->rotation * PI / 180.0);
        }

        void resize_and_rotate(int state, int button, int x, int y, bool held)
        {
            if (button == 0)
            {
                Layer *l = layer_manager->get_active_layer_ptr();
                set_rotation(l, l->rotation++);
            }
        }

        void process_update(Action *action, int state, int button, int x, int y, bool held)
        {
            if (strcmp(action->label, "Move") == 0)
            {
                move(state, button, x, y, held);
            }

            else if (strcmp(action->label, "Pencil") == 0)
            {
                pencil(action, button, x, y, held);
            }

            else if (strcmp(action->label, "Spray") == 0)
            {
                spray(action, button, x, y, held);
            }

            else if (strcmp(action->label, "Eraser") == 0)
            {
                eraser(action, button, x, y, held);
            }

            else if (strcmp(action->label, "Marker") == 0)
            {
                marker(action, button, x, y, held);
            }

            else if (strcmp(action->label, "Fill") == 0)
            {
                fill(state, button, x, y, held);
            }

            else if (strcmp(action->label, "Color Picker") == 0)
            {
                pick(state, button, x, y);
            }
            
            else if (strcmp(action->label, "Resize & Rotate") == 0)
            {
                resize_and_rotate(state, button, x, y, held);
            }
            
        }

        bool is_inside_area(int x, int y)
        {
            return (x >= layer_manager->anchorX) &&   
                    (x < layer_manager->anchorX + layer_manager->board_width) &&
                    (y >= layer_manager->anchorY) &&   
                    (y < layer_manager->anchorY + layer_manager->board_height);
        }

        void update_grabbing(bool held)
        {
            grabbed &= held;
        }
        
    public:
        Color active_color;

        Editor(Layer_Manager *layer_manager, Interface *interface)
        {
            this->layer_manager = layer_manager;
            this->interface = interface;
            this->active_color.set_from_rgb(255, 0, 0, 255);
            this->eraser_color.set_from_rgb(0, 0, 0, 0);
        }        

        void update_state(int state, int button, int x, int y, bool held)
        {
            Action *action = get_selected_action();

            if (action != NULL && is_inside_area(x, y) && layer_manager->is_valid())
            {
                process_update(action, state, button, x-layer_manager->anchorX, y-layer_manager->anchorY, held);
                mouse_last_x = x-layer_manager->anchorX;
                mouse_last_y = y-layer_manager->anchorY;
            }

            update_grabbing(held);
        }
};


#endif