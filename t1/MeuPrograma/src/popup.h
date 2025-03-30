#ifndef POPUP_H
#define POPUP_H

#include "gl_canvas2d.h"
#include "canvas_pro.h"

#include "layer_manager.h"
#include "interface.h"
#include "editor.h"
#include "keyboard.h"

#define POPUP_ROUTINE_NEW_LAYER 0

class Popup
{
    private:
        int routine = -1;
        int screen_width;
        int screen_height;

        int width = 720;
        int height = 405;

        int anchorX;
        int anchorY;

        char keyboard_buffer[256];

        bool invalid_file_add_layer_routine = false;

        Layer_Manager *layer_manager;
        Interface *interface;
        Editor *editor;

        CVpro::image *new_layer_icon;
        CVpro::image *blank_to_layer;
        CVpro::image *file_to_layer;

    public:
        bool is_open = false;

        Popup(int screen_width, int screen_height)
        {
            this->screen_width = screen_width;
            this->screen_height = screen_height;
            anchorX = screen_width/2.0-width/2.0;
            anchorY = screen_height/2.0-height/2.0;
        }

        void give_root_access(Layer_Manager *layer_manager, Interface *interface, Editor *editor)
        {
            this->layer_manager = layer_manager;
            this->interface = interface;
            this->editor = editor;
            this->new_layer_icon = CVpro::load_bitmap("./MeuPrograma/images/new_layer.bmp");
            this->blank_to_layer = CVpro::load_bitmap("./MeuPrograma/images/blank_to_layer.bmp");
            this->file_to_layer = CVpro::load_bitmap("./MeuPrograma/images/file_to_layer.bmp");
        }

        void open(int routine)
        {
            this->routine = routine;
            is_open = true;

            interface->deselect_all_actions();

            // resets keyboard buffer
            strcpy(keyboard_buffer, "");
        }

        void close()
        {
            this->routine = -1;
            is_open = false;
            invalid_file_add_layer_routine = false;
        }

        void display()
        {
            if (is_open)
            {
                display_background();
                display_popup();
            }
        }

        void display_background()
        {
            // content
            CVpro::color(50, 50, 50);
            CV::rectFill(anchorX, anchorY, anchorX + width, anchorY + height);

            // border
            CVpro::color(40, 40, 40);
            CV::rectFill(anchorX, anchorY, anchorX + 10, anchorY + height);
            CV::rectFill(anchorX, anchorY, anchorX + width, anchorY + 10);
            CV::rectFill(anchorX + width - 10, anchorY, anchorX + width, anchorY + height);
            CV::rectFill(anchorX, anchorY + height - 10, anchorX + width, anchorY + height);
        }

        void display_new_layer_routine()
        {
            // header
            CV::color(1, 1, 1);
            CVpro::text_align(anchorX + width/2.0, anchorY + 40, 'c', "New Layer");
            CV::line(anchorX + 30, anchorY + 50, anchorX + width - 30, anchorY + 50);
            CVpro::autotext(anchorX + width/2.0, anchorY + 75, 'c', 15, "You can add a Blank Layer\nor import a BMP to add as an Image Layer.");


            // blank layer dialog
            blank_to_layer->display_bitmap_anchored(anchorX + width/5.0, anchorY + 125, 1.0, 'c', 't');

            CV::color(1, 1, 1);
            CVpro::autotext(anchorX + width/5.0, anchorY + 200, 'c', 15, "Add Blank Layer.");

            new_layer_icon->display_bitmap_anchored(anchorX + width/5.0, anchorY + 225, 1.0, 'c', 't');


            // file to layer dialog
            file_to_layer->display_bitmap_anchored(anchorX + 3.5*width/5.0, anchorY + 125, 1.0, 'c', 't');

            CV::color(1, 1, 1);
            CVpro::autotext(anchorX + 3.5*width/5.0, anchorY + 200, 'c', 15, "Import from file.\nType file name:\n(root dir: ./MeuPrograma/images/)");

            if (invalid_file_add_layer_routine)
            {
                CVpro::color(200, 20, 20);
                CVpro::autotext(anchorX + 3.5*width/5.0, anchorY + 250, 'c', 15, "File not found, try again.");
            }
            
            CV::color(0, 0, 0);
            CV::rectFill(anchorX + 2.5*width/5.0, anchorY + 255, anchorX + 4.5*width/5.0, anchorY + 285);

            CVpro::color(23, 196, 0);
            CVpro::autotext(anchorX + 3.5*width/5.0, anchorY + 275, 'c', 15, keyboard_buffer);

            new_layer_icon->display_bitmap_anchored(anchorX + 3.5*width/5.0, anchorY + 300, 1.0, 'c', 't');
        }

        void display_popup()
        {
            switch (routine)
            {
                case POPUP_ROUTINE_NEW_LAYER:
                    display_new_layer_routine();
                    break;
                
                default:
                    break;
            }
        }

        void listen_new_layer_routine(int key)
        {
            if (is_open)
            {
                int cur = strlen(keyboard_buffer);
                if (cur < 255)
                {
                    CVkeyboard::decode(keyboard_buffer, key);
                }
            }
        }

        void listen(int key)
        {
            switch (routine)
            {
                case POPUP_ROUTINE_NEW_LAYER:
                    listen_new_layer_routine(key);
                    break;
                
                default:
                    break;
            }
        }
        
        void update_new_layer_routine(int button, int x, int y)
        {
            if (button == 0 &&
                x > anchorX + width/5.0 - new_layer_icon->width/2.0 &&
                y > anchorY + 225 &&
                x < anchorX + width/5.0 + new_layer_icon->width/2.0 &&
                y < anchorY + 225 + new_layer_icon->height
            )
            {
                layer_manager->add_blank_layer();
                close();
            }

            else if (button == 0 &&
                x > anchorX + 3.5*width/5.0 - new_layer_icon->width/2.0 &&
                y > anchorY + 300 &&
                x < anchorX + 3.5*width/5.0 + new_layer_icon->width/2.0 &&
                y < anchorY + 300 + new_layer_icon->height
            )
            {
                char tmp[256] = "./MeuPrograma/images/";
                strcat(tmp, keyboard_buffer);
                if (fopen(tmp, "r") == NULL)
                {
                    invalid_file_add_layer_routine = true;
                }
                
                else
                {
                    layer_manager->add_bmp_layer(tmp);
                    close();
                }
            }
        }

        void update(int button, int x, int y, bool held)
        {
            switch (routine)
            {
                case POPUP_ROUTINE_NEW_LAYER:
                    update_new_layer_routine(button, x, y);
                    break;
                
                default:
                    break;
            }
        }
};

#endif