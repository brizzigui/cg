#ifndef EDITOR_H
#define EDITOR_H

#include "layer_manager.h"
#include "side_menu.h"

class Editor
{
    private:
        // to keep track of changes, the editor needs to know the other classes
        Layer_Manager *layer_manager;
        Side_Menu *interface;
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
            std::cout << std::endl << j << ", " << i << std::endl;
            return (j >= l.anchorX) &&
                   (j < l.anchorX + l.image->width) &&
                   (i >= l.anchorY) &&
                   (i < l.anchorY + l.image->height);
        }

        void pencil_to_bmp(Layer l, int x, int y)
        {
            if (is_valid_pixel(l, y, x))
            {
                int base_index = (y-l.anchorY) * l.image->width * 4 + (x-l.anchorX) * 4;
                l.image->matrix[base_index + 2] = (unsigned char)255;
                l.image->matrix[base_index + 1] = (unsigned char)0;
                l.image->matrix[base_index] = (unsigned char)0;
                l.image->matrix[base_index + 3] = (unsigned char)255;
            }
        }

        void pencil(int button, int x, int y, bool held)
        {
            if (held)
            {
                Layer l = layer_manager->get_active_layer();
                pencil_to_bmp(l, x, y);                
            }
        }

        void process_update(const char *action, int button, int x, int y, bool held)
        {
            if (strcmp(action, "Pencil") == 0)
            {
                pencil(button, x, y, held);
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
        Editor(Layer_Manager *layer_manager, Side_Menu *interface)
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
            }

            mouse_last_x = x;
            mouse_last_y = y;
        }
};


#endif