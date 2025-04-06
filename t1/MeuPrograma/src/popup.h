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

typedef struct Adjustment
{
    double value;
    double range;
    const char* label;
    bool held;

} Adjustment;


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
        bool export_failure = false;
        int load_previews = -1;

        int effect_selected = -1;

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

        // for adjustments
        std::vector<Adjustment> adjustment_tracker;
        CVpro::image *adjustments_preview;

    public:
        bool is_open = false;

        Popup(int screen_width, int screen_height)
        {
            this->screen_width = screen_width;
            this->screen_height = screen_height;
            anchorX = screen_width/2.0-width/2.0;
            anchorY = screen_height/2.0-height/2.0;
            strcpy(keyboard_buffer, "");

            this->new_layer_icon = CVpro::load_bitmap("./MeuPrograma/images/new_layer.bmp");
            this->blank_to_layer_icon = CVpro::load_bitmap("./MeuPrograma/images/blank_to_layer.bmp");
            this->file_to_layer_icon = CVpro::load_bitmap("./MeuPrograma/images/file_to_layer.bmp");
            this->layer_to_file_icon = CVpro::load_bitmap("./MeuPrograma/images/layer_to_file.bmp");
            this->layer_to_img_icon = CVpro::load_bitmap("./MeuPrograma/images/layer_to_img.bmp");
            this->x_icon = CVpro::load_bitmap("./MeuPrograma/images/x.bmp");
        }

        void give_root_access(Layer_Manager *layer_manager)
        {
            this->layer_manager = layer_manager;
        }

        void open(int routine)
        {
            this->routine = routine;
            is_open = true;

            // resets keyboard buffer
            strcpy(keyboard_buffer, "");

            if (routine == POPUP_ROUTINE_EFFECTS)
            {   
                load_previews = 1;
                effect_selected = -1;
            }

            else if (routine == POPUP_ROUTINE_ADJUSTMENTS && layer_manager->is_valid())
            {
                create_adjustments_tracker();
                adjustments_preview = layer_manager->create_adjustments_preview();
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
            load_previews = -1;
            export_failure = false;
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
                if (load_previews == 1)
                {
                    CVpro::autotext(anchorX + width/2.0, anchorY + height/2.0, 'c', 15, "Loading effects...\nPlease wait.");
                    load_previews--;
                }
                
                else if (load_previews == 0)
                {
                    previews = layer_manager->create_previews();
                    load_previews--;
                }

                else
                {
                    CVpro::autotext(anchorX + width/2.0, anchorY + 75, 'c', 15, "Select an effect and then apply it.");

                    const char* effect_names[4] = {"Grayscale", "Sepia", "Gaussian Blur", "Vivid"};
                    for (int k = 0; k < 4; k++)
                    {
                        CVpro::color(70, 70, 70);
                        CV::rectFill(anchorX + 30 + k*(width-90)/4.0 + k*10, anchorY + 100, anchorX + 30 + (k+1)*(width-90)/4.0 + k*10, anchorY + 300);
                    
                        CV::color(1, 1, 1);
    
                        if (effect_selected == k)
                        {
                            CV::rect(anchorX + 30 + k*(width-90)/4.0 + k*10, anchorY + 100, anchorX + 30 + (k+1)*(width-90)/4.0 + k*10, anchorY + 300);
                        }
                        
                        CVpro::text_align(anchorX + 30 + (k+0.5)*(width-90)/4.0 + k*10, anchorY + 280, 'c', effect_names[k]);
                        previews[k]->display_bitmap_anchored(anchorX + 30 + (int)((k+0.5)*(width-90)/4.0 + k*10), anchorY + 110, ((width-90)/4.0 - 20)/layer_manager->board_width, 'c', 't');
                    }
    
                    CVpro::color(70, 70, 70);
                    CV::rectFill(anchorX + width/2.0 - 75, anchorY + height - 70, anchorX + width/2.0 + 75, anchorY + height - 30);
        
                    CV::color(1, 1, 1);
                    CV::rect(anchorX + width/2.0 - 75, anchorY + height - 70, anchorX + width/2.0 + 75, anchorY + height - 30);
                    CVpro::text_align(anchorX + width/2.0, anchorY + height - 45, 'c', "Accept");
                }
                
            }

            else
            {
                CVpro::autotext(anchorX + width/2.0, anchorY + height/2.0, 'c', 15, "No active layer.\nSelect a layer to apply an effect.");
            }
            
        }

        void create_adjustments_tracker()
        {
            adjustment_tracker.erase(adjustment_tracker.begin(), adjustment_tracker.end());
            const char *slider_labels[] = {"Brightness", "Contrast", "Saturation", "Temperature", "Gamma Correction"};
            int range[] = {200, 200, 200, 200, 2};

            for (int i = 0; i < 5; i++)
            {
                Adjustment a;
                a.label = slider_labels[i];
                a.value = 0;
                a.held = false;
                a.range = range[i];

                adjustment_tracker.push_back(a);
            }

            // sets gamma correction start value manually
            adjustment_tracker[4].value = 1;
        }

        void draw_adjustment_slider(int i)
        {
            int actual_anchorY = anchorY + 110;

            // size slider
            CVpro::text_align(anchorX + 30, actual_anchorY - 5 + 60*i, 'l', adjustment_tracker[i].label);

            CVpro::color(90, 90, 90);
            CV::rectFill(anchorX + 30, actual_anchorY + 10 + 60*i, anchorX + 280, actual_anchorY + 20 + 60*i);

            CV::color(1, 1, 1);
            CV::rect(anchorX + 30, actual_anchorY + 10 + 60*i, anchorX + 280, actual_anchorY + 20 + 60*i);

            int posX;
            if (i < 4)
            {
                posX = ((adjustment_tracker[i].value+adjustment_tracker[i].range/2.0) / adjustment_tracker[i].range) * 250;
            }
            else
            {
                posX = ((adjustment_tracker[i].value - 1) / adjustment_tracker[i].range) * 250;
            }
            
            CV::circleFill(anchorX + 30 + posX, actual_anchorY + 10 + 5 + 60*i, 10, 20);

            CV::color(0, 0, 0);
            CV::rectFill(anchorX + 300, actual_anchorY + 60*i, anchorX + 350, actual_anchorY + 30 + 60*i);

            CV::color(1, 1, 1);
            CVpro::text_align(anchorX + 325, actual_anchorY + 20 + 60*i, 'c', "%.3g", adjustment_tracker[i].value);
            CV::rect(anchorX + 300, actual_anchorY + 60*i, anchorX + 350, actual_anchorY + 30 + 60*i);
        }

        void display_adjustments_routine()
        {
            CV::color(1, 1, 1);
            CVpro::text_align(anchorX + width/2.0, anchorY + 40, 'c', "Adjustments");
            CV::line(anchorX + 30, anchorY + 50, anchorX + width - 30, anchorY + 50);

            if (layer_manager->is_valid())
            {
                // header
                CV::line(anchorX + 30, anchorY + 50, anchorX + width - 30, anchorY + 50);
                CVpro::autotext(anchorX + width/2.0, anchorY + 75, 'c', 15, "Use the sliders to adjust properties and then apply them.");

                // sliders
                for (int i = 0; i < 5; i++)
                {
                    draw_adjustment_slider(i);
                }

                // button
                CV::color(1, 1, 1);
                CV::rect(anchorX + 3*width/4.0 - 75, anchorY + height - 70, anchorX + 3*width/4.0 + 75, anchorY + height - 30);
                CVpro::text_align(anchorX + 3*width/4.0, anchorY + height - 45, 'c', "Accept");

                // preview
                CVpro::text_align(anchorX + 3*width/4.0, anchorY + 110, 'c', "Preview");
                adjustments_preview->display_bitmap_anchored(anchorX + 3*width/4.0, anchorY + 120, 1/3.0, 'c', 't');

            }

            else
            {
                CVpro::autotext(anchorX + width/2.0, anchorY + height/2.0, 'c', 15, "No active layer.\nSelect a layer to apply an adjustment.");
            }
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

            if (export_failure)
            {
                CV::color(1, 0, 0);
                CVpro::autotext(anchorX + width/2.0, anchorY + 340, 'c', 15, "Something went wrong.\nTry again later.");
            }
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

            if (export_failure)
            {
                CV::color(1, 0, 0);
                CVpro::autotext(anchorX + width/2.0, anchorY + 340, 'c', 15, "Something went wrong.\nTry again later.");
            }
        }

        void display_special_click_configs(const char *title)
        {
            int size = ((std_selectable_values *)var)->size;

            CV::color(1, 1, 1);
            CVpro::text_align(anchorX + width/2.0, anchorY + 40, 'c', "%s settings", title);
            CV::line(anchorX + 30, anchorY + 50, anchorX + width - 30, anchorY + 50);
            CVpro::autotext(anchorX + width/2.0, anchorY + 75, 'c', 15, "Change the %s's settings.", title);

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


            // tip types (for pencil and eraser)
            if (strcmp(title, "Pencil") == 0 || strcmp(title, "Eraser") == 0)
            {
                CV::color(1, 1, 1);
                CVpro::text_align(anchorX + 30, anchorY + 200, 'l', "Type");

                const char* type_names[3] = {"Circle", "Square", "Grid\n(Pixel Art)"};

                int type_box_width = 125;
                int type_box_height = 125;

                for (int type = 0; type < 3; type++)
                {
                    CVpro::color(90, 90, 90);
                    CV::rectFill(anchorX + 30 + type_box_width*type + 20*type, anchorY + 220, anchorX + 30 + type_box_width*(type+1) + 20*type, anchorY + 220 + type_box_height);

                    if (((std_selectable_values *)var)->type == type)
                    {
                        CVpro::color(255, 255, 255);
                        CV::rect(anchorX + 30 + type_box_width*type + 20*type, anchorY + 220, anchorX + 30 + type_box_width*(type+1) + 20*type, anchorY + 220 + type_box_height);
                    }
                    
                    CVpro::color(255, 255, 255);

                    if (type == SELECTABLE_TYPE_CIRCLE)
                    {
                        CV::circleFill(anchorX + 30 + type_box_width*type + 20*type + type_box_width/2.0, anchorY + 220 + type_box_height/3.0, 20, 20); 
                    }

                    else if (type == SELECTABLE_TYPE_SQUARE)
                    {
                        CV::rectFill(anchorX + 30 + type_box_width*type + 20*type + type_box_width/2.0 - 20, anchorY + 220 + type_box_height/3.0 - 20,
                                        anchorX + 30 + type_box_width*type + 20*type + type_box_width/2.0 + 20, anchorY + 220 + type_box_height/3.0 + 20); 
                    }
                    
                    else
                    {
                        for (int i = 0; i < 5; i++)
                        {
                            CV::line(anchorX + 30 + type_box_width*type + 20*type + type_box_width/2.0 - 20, anchorY + 220 + type_box_height/3.0 - 20 + 10*i,
                                    anchorX + 30 + type_box_width*type + 20*type + type_box_width/2.0 + 20, anchorY + 220 + type_box_height/3.0 - 20 + 10*i); 
                        }

                        for (int j = 0; j < 5; j++)
                        {
                            CV::line(anchorX + 30 + type_box_width*type + 20*type + type_box_width/2.0 - 20 + 10*j, anchorY + 220 + type_box_height/3.0 - 20,
                                    anchorX + 30 + type_box_width*type + 20*type + type_box_width/2.0 - 20 + 10*j, anchorY + 220 + type_box_height/3.0 + 20); 
                        }
                    }
                    
                    CVpro::color(255, 255, 255);
                    CVpro::autotext(anchorX + 30 + type_box_width*type + 20*type + type_box_width/2.0, anchorY + 220 + type_box_height-24, 'c', 12, type_names[type]);
                }

                CVpro::autotext(anchorX + 30 + type_box_width*3 + 20*3.5, anchorY+220, 'l', 15, "Tip:\n\nGrid type (Pixel\nArt Mode) allows you\nto draw with the\nhelp of a grid.\n\nUse it to create\namazing pixel art\non BIMP.");
            }
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
                char path[256];
                sprintf(path, "./MeuPrograma/projects/%s.bimp", keyboard_buffer);
                if(layer_manager->save_project(path))
                {
                    close();
                }

                else
                {
                    export_failure = true;
                }
            }
        }

        void update_export_routine(int state, int button, int x, int y)
        {
            if (check_common_accept_button(state, button, x, y))
            {
                char path[256];
                sprintf(path, "./MeuPrograma/exports/%s.bmp", keyboard_buffer);
                if(layer_manager->export_image(path))
                {
                    close();
                }

                else
                {
                    export_failure = true;
                }
                
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
            ((std_selectable_values *)(var))->size = std::clamp((x/250.0)*100, 1.0, 100.0);
        }

        void update_special_click_configs(int state, int button, int x, int y, bool held)
        {
            if(check_common_slider(state, button, x, y))
            {
                translate_common_slider(x, y);
            }
        }

        int check_clicked_type(int x, int y)
        {
            int type_box_width = 125;
            int type_box_height = 125;

            for (int type = 0; type < 3; type++)
            {
                if (x > anchorX + 30 + type_box_width*type + 20*type &&
                    y > anchorY + 220 &&
                    x < anchorX + 30 + type_box_width*(type+1) + 20*type &&
                    y < anchorY + 220 + type_box_height)
                {
                    return type;
                }
            }

            return -1;
        }

        void update_type_special_click_configs(int state, int button, int x, int y)
        {
            if (state == 0 && button == 0)
            {
                int pick = check_clicked_type(x, y);
                if (pick != -1)
                {
                    ((std_selectable_values *)var)->type = pick;
                }
            }
        }

        int check_selected_effect(int x, int y)
        {
            for (int k = 0; k < 4; k++)
            {
                if(x > anchorX + 30 + k*(width-90)/4.0 + k*10 &&
                    y > anchorY + 100 &&
                    x < anchorX + 30 + (k+1)*(width-90)/4.0 + k*10 &&
                    y < anchorY + 300)
                {
                    return k;
                }
            }
            
            return -1;
        }

        bool check_accept_effect(int x, int y)
        {
            return (x > anchorX + width/2.0 - 75 &&
                    y > anchorY + height - 70 &&
                    x < anchorX + width/2.0 + 75 &&
                    y < anchorY + height - 30);
        }

        void update_routine_effects(int state, int button, int x, int y)
        {
           
            if (state != 0 || button != 0)
            {
                return;
            }
            
            if (check_accept_effect(x, y) && this->effect_selected != -1 && layer_manager->is_valid())
            {
                Layer *l = layer_manager->get_active_layer_ptr();
                l->image = previews[this->effect_selected];
                close();
            }

            else
            {
                this->effect_selected = check_selected_effect(x, y);
            }
            
        }

        bool adjustment_slider_check(int index, int state, int button, int x, int y, bool held)
        {
            int actual_anchorY = anchorY + 110;

            bool tap =
                state == 0 && button == 0 &&
                x > anchorX + 30 &&
                y > actual_anchorY + 10 + 60*index &&
                x < anchorX + 280 &&
                y < actual_anchorY + 20 + 60*index;

            adjustment_tracker[index].held = adjustment_tracker[index].held || tap;

            return tap || adjustment_tracker[index].held;
        }

        void translate_adjustment_slider(int index, int x, int y)
        {
            x = (x-anchorX-30);

            if (index < 4)  // 4 is gamma correction, which has a different scale
            {
                adjustment_tracker[index].value = std::clamp((int)((x/250.0)*200-100), -100, 100);  
            }
            
            else            // gamma correction scale
            {
                adjustment_tracker[index].value = std::clamp((x/250.0)*2+1, 1.0, 3.0);  
            }
        }

        bool check_accept_adjustments(int button, int state, int x, int y)
        {
            return button == 0 && state == 0 &&
                    x > anchorX + 3*width/4.0 - 75 &&
                    y > anchorY + height - 70 && 
                    x < anchorX + 3*width/4.0 + 75 &&
                    y < anchorY + height - 30;
        }

        void update_adjustments_routine(int state, int button, int x, int y, bool held)
        {
            if (!layer_manager->is_valid())
            {
                return;
            }
            
            for (int i = 0; i < adjustment_tracker.size(); i++)
            {
                if (adjustment_slider_check(i, state, button, x, y, held))
                {
                    translate_adjustment_slider(i, x, y);
                }
            } 

            if (button == 0 && state == 1)
            {
                adjustments_preview = layer_manager->create_adjustments_preview(
                    adjustments_preview,
                    adjustment_tracker[0].value,
                    adjustment_tracker[1].value,
                    adjustment_tracker[2].value,
                    adjustment_tracker[3].value,
                    adjustment_tracker[4].value
                );
            }

            if (check_accept_adjustments(button, state, x, y))
            {
                layer_manager->layers[layer_manager->active_index].image = adjustments_preview;
                close();
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

        void update_helds(bool held)
        {
            simple_held &= held;
            for (int i = 0; i < adjustment_tracker.size(); i++)
            {
                adjustment_tracker[i].held &= held;
            }
        }

        void update(int state, int button, int x, int y, bool held)
        {
            update_helds(held);
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

                case POPUP_ROUTINE_ADJUSTMENTS:
                    update_adjustments_routine(state, button, x, y, held);
                    break;

                case POPUP_ROUTINE_EFFECTS:
                    update_routine_effects(state, button, x, y);
                    break;

                case POPUP_ROUTINE_SPRAY_SETTINGS:
                case POPUP_ROUTINE_MARKER_SETTINGS:
                    update_special_click_configs(state, button, x, y, held);
                    break;

                case POPUP_ROUTINE_ERASER_SETTINGS:
                case POPUP_ROUTINE_PENCIL_SETTINGS:
                    update_special_click_configs(state, button, x, y, held);
                    update_type_special_click_configs(state, button, x, y);
                    break;
                
                default:
                    break;
            }
        }
};

#endif