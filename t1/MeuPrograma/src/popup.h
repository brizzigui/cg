#ifndef POPUP_H
#define POPUP_H

#include "gl_canvas2d.h"
#include "canvas_pro.h"

#include "layer_manager.h"
#include "keyboard.h"
#include "shared.h"

#define POPUP_ROUTINE_NEW_LAYER 0
#define POPUP_ROUTINE_EFFECTS 1
#define POPUP_ROUTINE_ADJUSTMENTS 2
#define POPUP_ROUTINE_SAVE 3
#define POPUP_ROUTINE_EXPORT 4
#define POPUP_ROUTINE_PENCIL_SETTINGS 5
#define POPUP_ROUTINE_SPRAY_SETTINGS 6
#define POPUP_ROUTINE_MARKER_SETTINGS 7
#define POPUP_ROUTINE_ERASER_SETTINGS 8

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
        bool simple_held = false;

        Layer_Manager *layer_manager;

        CVpro::image *new_layer_icon;
        CVpro::image *blank_to_layer_icon;
        CVpro::image *file_to_layer_icon;
        CVpro::image *layer_to_file_icon;
        CVpro::image *layer_to_img_icon;
        CVpro::image *x_icon;

        void *var = NULL;

        // for effects
        std::vector<CVpro::image *> previews;

    public:
        bool is_open = false;

        Popup(int screen_width, int screen_height)
        {
            this->screen_width = screen_width;
            this->screen_height = screen_height;
            anchorX = screen_width/2.0-width/2.0;
            anchorY = screen_height/2.0-height/2.0;
            strcpy(keyboard_buffer, "");
        }

        void give_root_access(Layer_Manager *layer_manager)
        {
            this->layer_manager = layer_manager;

            this->new_layer_icon = CVpro::load_bitmap("./MeuPrograma/images/new_layer.bmp");
            this->blank_to_layer_icon = CVpro::load_bitmap("./MeuPrograma/images/blank_to_layer.bmp");
            this->file_to_layer_icon = CVpro::load_bitmap("./MeuPrograma/images/file_to_layer.bmp");
            this->layer_to_file_icon = CVpro::load_bitmap("./MeuPrograma/images/layer_to_file.bmp");
            this->layer_to_img_icon = CVpro::load_bitmap("./MeuPrograma/images/layer_to_img.bmp");
            this->x_icon = CVpro::load_bitmap("./MeuPrograma/images/x.bmp");
        }

        void open(int routine)
        {
            this->routine = routine;
            is_open = true;

            // resets keyboard buffer
            strcpy(keyboard_buffer, "");

            if (routine == POPUP_ROUTINE_EFFECTS)
            {   
                previews = layer_manager->create_previews();
            }
        }

        void open(int routine, void *var)
        {
            this->routine = routine;
            is_open = true;

            // resets keyboard buffer
            strcpy(keyboard_buffer, "");

            // passes var pointer;
            this->var = var;
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

            // close button
            x_icon->display_bitmap_anchored(anchorX+width, anchorY, 1.0, 'r', 't');
        }

        void display_new_layer_routine()
        {
            // header
            CV::color(1, 1, 1);
            CVpro::text_align(anchorX + width/2.0, anchorY + 40, 'c', "New Layer");
            CV::line(anchorX + 30, anchorY + 50, anchorX + width - 30, anchorY + 50);
            CVpro::autotext(anchorX + width/2.0, anchorY + 75, 'c', 15, "You can add a Blank Layer\nor import a BMP to add as an Image Layer.");


            // blank layer dialog
            blank_to_layer_icon->display_bitmap_anchored(anchorX + width/5.0, anchorY + 125, 1.0, 'c', 't');

            CV::color(1, 1, 1);
            CVpro::autotext(anchorX + width/5.0, anchorY + 200, 'c', 15, "Add Blank Layer.");

            new_layer_icon->display_bitmap_anchored(anchorX + width/5.0, anchorY + 225, 1.0, 'c', 't');


            // file to layer dialog
            file_to_layer_icon->display_bitmap_anchored(anchorX + 3.5*width/5.0, anchorY + 125, 1.0, 'c', 't');

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

        void display_effects_routine()
        {
            CV::color(1, 1, 1);
            CVpro::text_align(anchorX + width/2.0, anchorY + 40, 'c', "Effects");
            CV::line(anchorX + 30, anchorY + 50, anchorX + width - 30, anchorY + 50);

            if (layer_manager->is_valid())
            {
                CVpro::autotext(anchorX + width/2.0, anchorY + 75, 'c', 15, "Select an effect and then apply it.");

                const char* effect_names[4] = {"Grayscale", "Sepia", "Gaussian Blur", "Vivid"};
                for (int k = 0; k < 4; k++)
                {
                    CVpro::color(70, 70, 70);
                    CV::rectFill(anchorX + 30 + k*(width-90)/4.0 + k*10, anchorY + 100, anchorX + 30 + (k+1)*(width-90)/4.0 + k*10, anchorY + 300);
        
                    CV::color(1, 1, 1);
                    CV::rect(anchorX + 30 + k*(width-90)/4.0 + k*10, anchorY + 100, anchorX + 30 + (k+1)*(width-90)/4.0 + k*10, anchorY + 300);
                    CVpro::text_align(anchorX + 30 + (k+0.5)*(width-90)/4.0 + k*10, anchorY + 280, 'c', effect_names[k]);

                    previews[k]->display_bitmap_anchored(anchorX + 30 + (int)((k+0.5)*(width-90)/4.0 + k*10), anchorY + 110, ((width-90)/4.0 - 20)/layer_manager->board_width, 'c', 't');
                }

                CVpro::color(70, 70, 70);
                CV::rectFill(anchorX + width/2.0 - 75, anchorY + height - 70, anchorX + width/2.0 + 75, anchorY + height - 30);
    
                CV::color(1, 1, 1);
                CV::rect(anchorX + width/2.0 - 75, anchorY + height - 70, anchorX + width/2.0 + 75, anchorY + height - 30);
                CVpro::text_align(anchorX + width/2.0, anchorY + height - 45, 'c', "Accept");
            }

            else
            {
                CVpro::autotext(anchorX + width/2.0, anchorY + height/2.0, 'c', 15, "No active layer.\nSelect a layer to apply an effect.");
            }
            
        }

        void display_adjustments_routine()
        {
            CV::color(1, 1, 1);
            CVpro::text_align(anchorX + width/2.0, anchorY + 40, 'c', "Adjustments");
            CV::line(anchorX + 30, anchorY + 50, anchorX + width - 30, anchorY + 50);
            CVpro::autotext(anchorX + width/2.0, anchorY + 75, 'c', 15, "Use the sliders to adjust properties and then apply them.");
        }

        void display_save_routine()
        {
            CV::color(1, 1, 1);
            CVpro::text_align(anchorX + width/2.0, anchorY + 40, 'c', "Save project");
            CV::line(anchorX + 30, anchorY + 50, anchorX + width - 30, anchorY + 50);
            CVpro::autotext(anchorX + width/2.0, anchorY + 75, 'c', 15, "Save the current state as a '.bimp' project.");

            layer_to_file_icon->display_bitmap_anchored(anchorX + width/2.0, anchorY + 110, 1.0, 'c', 't');

            CV::color(1, 1, 1);
            CVpro::autotext(anchorX + width/2.0, anchorY + 180, 'c', 15, "Type a file name:\n(root dir: ./MeuPrograma/projects/)");
            
            CV::color(0, 0, 0);
            CV::rectFill(anchorX + width/2.0 - 150, anchorY + 220, anchorX + width/2.0 + 150, anchorY + 250);

            CVpro::color(23, 196, 0);
            CVpro::autotext(anchorX + width/2.0, anchorY + 240, 'c', 15, "%s.bimp", keyboard_buffer);

            CVpro::color(70, 70, 70);
            CV::rectFill(anchorX + width/2.0 - 75, anchorY + 270, anchorX + width/2.0 + 75, anchorY + 310);

            CV::color(1, 1, 1);
            CV::rect(anchorX + width/2.0 - 75, anchorY + 270, anchorX + width/2.0 + 75, anchorY + 310);
            CVpro::text_align(anchorX + width/2.0, anchorY + 295, 'c', "Accept");
        }

        void display_export_routine()
        {
            CV::color(1, 1, 1);
            CVpro::text_align(anchorX + width/2.0, anchorY + 40, 'c', "Export project");
            CV::line(anchorX + 30, anchorY + 50, anchorX + width - 30, anchorY + 50);
            CVpro::autotext(anchorX + width/2.0, anchorY + 75, 'c', 15, "Export the edited image as a '.bmp'.");

            layer_to_img_icon->display_bitmap_anchored(anchorX + width/2.0, anchorY + 110, 1.0, 'c', 't');

            CV::color(1, 1, 1);
            CVpro::autotext(anchorX + width/2.0, anchorY + 180, 'c', 15, "Type a file name:\n(root dir: ./MeuPrograma/exports/)");
            
            CV::color(0, 0, 0);
            CV::rectFill(anchorX + width/2.0 - 150, anchorY + 220, anchorX + width/2.0 + 150, anchorY + 250);

            CVpro::color(23, 196, 0);
            CVpro::autotext(anchorX + width/2.0, anchorY + 240, 'c', 15, "%s.bmp", keyboard_buffer);

            CVpro::color(70, 70, 70);
            CV::rectFill(anchorX + width/2.0 - 75, anchorY + 270, anchorX + width/2.0 + 75, anchorY + 310);

            CV::color(1, 1, 1);
            CV::rect(anchorX + width/2.0 - 75, anchorY + 270, anchorX + width/2.0 + 75, anchorY + 310);
            CVpro::text_align(anchorX + width/2.0, anchorY + 295, 'c', "Accept");
        }

        void display_special_click_configs(const char *title)
        {
            int size = ((std_selectable_values *)var)->size;

            CV::color(1, 1, 1);
            CVpro::text_align(anchorX + width/2.0, anchorY + 40, 'c', "%s settings", title);
            CV::line(anchorX + 30, anchorY + 50, anchorX + width - 30, anchorY + 50);
            CVpro::autotext(anchorX + width/2.0, anchorY + 75, 'c', 15, "Change size and type of the %s.", title);

            // size slider
            CVpro::text_align(anchorX + 30, anchorY + 125, 'l', "Size");

            CVpro::color(90, 90, 90);
            CV::rectFill(anchorX + 30, anchorY + 150, anchorX + 280, anchorY + 160);

            CV::color(1, 1, 1);
            CV::rect(anchorX + 30, anchorY + 150, anchorX + 280, anchorY + 160);
            CV::circleFill(anchorX + 30 + size*(250)/100.0-1, anchorY + 155, 10, 20);

            CV::color(0, 0, 0);
            CV::rectFill(anchorX + 300, anchorY + 140, anchorX + 350, anchorY + 170);

            CV::color(1, 1, 1);
            CVpro::text_align(anchorX + 325, anchorY + 160, 'c', "%02d", size);
            CV::rect(anchorX + 300, anchorY + 140, anchorX + 350, anchorY + 170);
        }

        void display_popup()
        {
            switch (routine)
            {
                case POPUP_ROUTINE_NEW_LAYER:
                    display_new_layer_routine();
                    break;

                case POPUP_ROUTINE_EFFECTS:
                    display_effects_routine();
                    break;

                case POPUP_ROUTINE_ADJUSTMENTS:
                    display_adjustments_routine();
                    break;

                case POPUP_ROUTINE_SAVE:
                    display_save_routine();
                    break;

                case POPUP_ROUTINE_EXPORT:
                    display_export_routine();
                    break;

                case POPUP_ROUTINE_PENCIL_SETTINGS:
                    display_special_click_configs("Pencil");
                    break;

                case POPUP_ROUTINE_SPRAY_SETTINGS:
                    display_special_click_configs("Spray");
                    break;

                case POPUP_ROUTINE_MARKER_SETTINGS:
                    display_special_click_configs("Marker");
                    break;

                case POPUP_ROUTINE_ERASER_SETTINGS:
                    display_special_click_configs("Eraser");
                    break;
                
                default:
                    break;
            }
        }

        void listen(int key)
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

                FILE *ptr = fopen(tmp, "r");
                if (ptr == NULL)
                {
                    invalid_file_add_layer_routine = true;
                }
                
                else
                {
                    fclose(ptr);
                    layer_manager->add_bmp_layer(tmp);
                    close();
                }
            }
        }

        bool check_common_accept_button(int state, int button, int x, int y)
        {
            return state == 0 && button == 0 &&
            x > anchorX + width/2.0 - 75 &&
            y > anchorY + 270 &&
            x < anchorX + width/2.0 + 75 &&
            y < anchorY + 310;
        }

        void update_save_routine(int state, int button, int x, int y)
        {
            if (check_common_accept_button(state, button, x, y))
            {
                layer_manager->save_project(keyboard_buffer);
            }
        }

        void update_export_routine(int state, int button, int x, int y)
        {
            if (check_common_accept_button(state, button, x, y))
            {
                layer_manager->export_image(keyboard_buffer);
            }
        }

        bool check_common_slider(int state, int button, int x, int y)
        {
            bool tap =
                    state == 0 && button == 0 &&
                    x > anchorX + 30 &&
                    y > anchorY + 150 &&
                    x < anchorX + 280 &&
                    y < anchorY + 160;

                simple_held = simple_held || tap;

            return tap || simple_held;
        }

        void translate_common_slider(int x, int y)
        {
            x = (x-anchorX-30);
            x = (x < 0) ? 0 : x;
            x = (x > 250) ? 250 : x;

            ((std_selectable_values *)(var))->size = (x/250.0)*100+1;
        }

        void update_special_click_configs(int state, int button, int x, int y, bool held)
        {
            if(check_common_slider(state, button, x, y))
            {
                translate_common_slider(x, y);
            }
        }

        bool check_close_button(int button, int x, int y)
        {
            return (button == 0) &&
                    x > anchorX+width-20 &&
                    y > anchorY &&
                    x < anchorX+width &&
                    y < anchorY+20;
        }

        void check_common_ui(int button, int x, int y)
        {
            if (check_close_button(button, x, y))
            {
                close();
            }
        }

        void update(int state, int button, int x, int y, bool held)
        {
            simple_held &= held;
            check_common_ui(button, x, y);

            switch (routine)
            {
                case POPUP_ROUTINE_NEW_LAYER:
                    update_new_layer_routine(button, x, y);
                    break;

                case POPUP_ROUTINE_SAVE:
                    update_save_routine(state, button, x, y);
                    break;

                case POPUP_ROUTINE_EXPORT:
                    update_export_routine(state, button, x, y);
                    break;

                case POPUP_ROUTINE_PENCIL_SETTINGS:
                case POPUP_ROUTINE_SPRAY_SETTINGS:
                case POPUP_ROUTINE_MARKER_SETTINGS:
                case POPUP_ROUTINE_ERASER_SETTINGS:
                    update_special_click_configs(state, button, x, y, held);
                    break;
                
                default:
                    break;
            }
        }
};

#endif