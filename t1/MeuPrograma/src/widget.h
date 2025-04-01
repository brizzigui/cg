#ifndef WIDGET_H
#define WIDGET_H

#include "layer_manager.h"
#include "interface.h"
#include "editor.h"
#include "popup.h"

#define WIDGET_LAYER_SELECTOR 0
#define WIDGET_COLOR_PICKER 1

#define WIDGET_LAYER_NEW_BUTTON 0
#define WIDGET_LAYER_UP_BUTTON 1
#define WIDGET_LAYER_DOWN_BUTTON 2

class Widget
{
    private:
        int current;
        int anchorX = 920;
        int anchorY = 100;

        int width = 300;
        int height = 540;

        int button_width = 100;
        int button_height = 50;

        int button_x_offset = 100/3.0;
        int button_y_offset = 20;

        int usable_anchorY = anchorY + button_height + button_y_offset;

        int first_shown;

        int sv_map_width = width - 50;
        int sv_map_height = 150;
        int sv_map_margin = 25;

        bool hue_slider_simple_held = false;
        bool alpha_slider_simple_held = false;
        bool sv_map_simple_held = false;

        Layer_Manager *layer_manager;
        Interface *interface;
        Editor *editor;
        Popup *popup;

        Color last_color;

        CVpro::image *color_picker_icon;
        CVpro::image *layer_selector_icon;
        CVpro::image *up_arrow_icon;
        CVpro::image *down_arrow_icon;
        CVpro::image *new_layer_icon;
        CVpro::image *trash_icon;
        CVpro::image *open_eye_icon;
        CVpro::image *closed_eye_icon;
        CVpro::image *z_index_up_icon;
        CVpro::image *z_index_down_icon;
        CVpro::image *sv_map;
        CVpro::image *alpha_slider_base;
        CVpro::image *alpha_slider_result;
        CVpro::image *plus_icon;
        CVpro::image *minus_icon;

    public:
        Widget(Layer_Manager *layer_manager, Interface *interface, Editor *editor, Popup *popup)
        {
            this->current = WIDGET_LAYER_SELECTOR;
            this->layer_manager = layer_manager;
            this->interface = interface;
            this->editor = editor;
            this->popup = popup;

            this->color_picker_icon = CVpro::load_bitmap("./MeuPrograma/images/picker_icon.bmp");
            this->layer_selector_icon = CVpro::load_bitmap("./MeuPrograma/images/layer_selector.bmp");
            this->up_arrow_icon = CVpro::load_bitmap("./MeuPrograma/images/up_arrow.bmp");
            this->down_arrow_icon = CVpro::load_bitmap("./MeuPrograma/images/down_arrow.bmp");
            this->new_layer_icon = CVpro::load_bitmap("./MeuPrograma/images/new_layer.bmp");
            this->trash_icon = CVpro::load_bitmap("./MeuPrograma/images/trash.bmp");
            this->open_eye_icon = CVpro::load_bitmap("./MeuPrograma/images/open_eye.bmp");
            this->closed_eye_icon = CVpro::load_bitmap("./MeuPrograma/images/closed_eye.bmp");
            this->z_index_up_icon = CVpro::load_bitmap("./MeuPrograma/images/z-index_up.bmp");
            this->z_index_down_icon = CVpro::load_bitmap("./MeuPrograma/images/z-index_down.bmp");
            this->plus_icon = CVpro::load_bitmap("./MeuPrograma/images/plus.bmp");
            this->minus_icon = CVpro::load_bitmap("./MeuPrograma/images/minus.bmp");

            this->sv_map = generate_sv_map_base();
            regenerate_sv_map();

            this->alpha_slider_base = generate_alpha_slider_base();
            this->alpha_slider_result = generate_alpha_slider_result();
            regenerate_alpha_slider();

            first_shown = layer_manager->layers.size() - 3;
            first_shown = (first_shown >= 0) ? first_shown : 0;

            last_color = editor->active_color;
        }

        int check_frame_click(int x, int y)
        {
            if (x > anchorX + button_x_offset 
                && x < anchorX + button_x_offset + button_width 
                && y > anchorY 
                && y < anchorY + button_height)
            {
                return WIDGET_LAYER_SELECTOR;
            }

            if (x > anchorX + button_x_offset*2 + button_width
                && x < anchorX + button_x_offset*2 + button_width*2 
                && y > anchorY 
                && y < anchorY + button_height)
            {
                return WIDGET_COLOR_PICKER;
            }

            return -1;
        }

        void update_widget_frame(int x, int y)
        {
            int choice = check_frame_click(x, y);
            if (choice == -1)
            {
                return;
            }

            else
            {
                current = choice;
            }
        }

        int get_clicked_layer(int x, int y)
        {
            int total_layers = layer_manager->layers.size();
            int max_iterations = first_shown + std::min(total_layers, 3);
            for (int i = first_shown; i < max_iterations; i++)
            {
                if(x > anchorX+10 &&
                    y > usable_anchorY+60+85*(max_iterations-i-1) &&
                    x < anchorX+width-10 &&
                    y < usable_anchorY+60+85*(max_iterations-i-1)+75)
                {
                    return i;
                }
            }

            return -1;
        }

        int check_other_layer_buttons(int x, int y)
        {
            if (x > anchorX + width/2.0 - new_layer_icon->width/2.0 &&
                y > usable_anchorY+400 &&
                x < anchorX + width/2.0 + 120 - new_layer_icon->width/2.0 &&
                y < usable_anchorY+400+50)
            {
                return WIDGET_LAYER_NEW_BUTTON;
            }

            if (x > anchorX+width/2.0 - up_arrow_icon->width/2.0 &&
                y > usable_anchorY+25 &&
                x < anchorX+width/2.0+50 - up_arrow_icon->width/2.0 &&
                y < usable_anchorY+25+25)
            {
                return WIDGET_LAYER_UP_BUTTON;
            }
        
            if (x > anchorX+width/2.0 - up_arrow_icon->width/2.0&&
                y > usable_anchorY+315 &&
                x < anchorX+width/2.0+50 - up_arrow_icon->width/2.0 &&
                y < usable_anchorY+315+25)
            {
                return WIDGET_LAYER_DOWN_BUTTON;
            }
            
            return -1;
        }

        bool click_delete(int layer_id, int x, int y)
        {
            int total_layers = layer_manager->layers.size();
            int max_iterations = first_shown + std::min(total_layers, 3);

            return x > anchorX + width-50 - trash_icon->width &&
                    y > usable_anchorY+60+85*(max_iterations-layer_id-1)+75/2.0 - trash_icon->height/2.0 &&
                    x < anchorX + width-50 &&
                    y < usable_anchorY+60+85*(max_iterations-layer_id-1)+75/2.0 + trash_icon->height/2.0;
        }

        bool click_toggle_eye(int layer_id, int x, int y)
        {
            int total_layers = layer_manager->layers.size();
            int max_iterations = first_shown + std::min(total_layers, 3);

            return x > anchorX + width-80 - open_eye_icon->width &&
                    y > usable_anchorY+60+85*(max_iterations-layer_id-1)+75/2.0 - trash_icon->height/2.0 &&
                    x < anchorX + width-80 &&
                    y < usable_anchorY+60+85*(max_iterations-layer_id-1)+75/2.0 + trash_icon->height/2.0;
        }

        bool click_z_index_up(int layer_id, int x, int y)
        {
            int total_layers = layer_manager->layers.size();
            int max_iterations = first_shown + std::min(total_layers, 3);

            return x > anchorX + width-20 - open_eye_icon->width &&
                    y > usable_anchorY+60+85*(max_iterations-layer_id-1)+75/2.0 - trash_icon->height &&
                    x < anchorX + width-20 &&
                    y < usable_anchorY+60+85*(max_iterations-layer_id-1)+75/2.0;
        }

        bool click_z_index_down(int layer_id, int x, int y)
        {
            int total_layers = layer_manager->layers.size();
            int max_iterations = first_shown + std::min(total_layers, 3);

            return x > anchorX + width-20 - open_eye_icon->width &&
                    y > usable_anchorY+60+85*(max_iterations-layer_id-1)+75/2.0 &&
                    x < anchorX + width-20 &&
                    y < usable_anchorY+60+85*(max_iterations-layer_id-1)+75/2.0 + trash_icon->height;
        }

        int safe_first_shown_index_update()
        {
            if (first_shown + 3 > layer_manager->layers.size())
            {
                if (first_shown - 1 < 0)
                {
                    return 0;
                }
                else
                {
                    return first_shown - 1;
                }
            }

            return first_shown;
        }

        void update_layer_selector(int x, int y)
        {
            int layer_id = get_clicked_layer(x, y);
            
            if (layer_id == -1)
            {
                int clicked_button = check_other_layer_buttons(x, y);

                if (clicked_button == WIDGET_LAYER_NEW_BUTTON)
                {
                    popup->open(POPUP_ROUTINE_NEW_LAYER);
                }
                else if (clicked_button == WIDGET_LAYER_UP_BUTTON)
                {
                    first_shown = (first_shown + 3 + 1 > layer_manager->layers.size()) ? first_shown : first_shown + 1;
                }
                else if (clicked_button == WIDGET_LAYER_DOWN_BUTTON)
                {
                    first_shown = (first_shown - 1 < 0) ? first_shown : first_shown - 1;
                }
            }

            else
            {
                if (click_delete(layer_id, x, y))
                {
                    layer_manager->safe_delete_layer(layer_id);
                    first_shown = safe_first_shown_index_update();
                }

                else if (click_toggle_eye(layer_id, x, y))
                {
                    layer_manager->layers[layer_id].visible = !layer_manager->layers[layer_id].visible;
                }

                else if (click_z_index_up(layer_id, x, y))
                {
                    layer_manager->switch_order(layer_id, layer_id+1);
                }

                else if (click_z_index_down(layer_id, x, y))
                {
                    layer_manager->switch_order(layer_id, layer_id-1);
                }

                else
                {
                    layer_manager->set_active_layer(layer_id);
                }
            }
        }

        bool click_sv_map(int x, int y, bool held)
        {
            bool tap = held &&
                    x > anchorX + sv_map_margin &&
                    y > usable_anchorY + sv_map_margin &&
                    x < anchorX + sv_map_margin + sv_map_width &&
                    y < usable_anchorY + sv_map_margin + sv_map_height;

            sv_map_simple_held = sv_map_simple_held || tap;

            return tap || sv_map_simple_held;
        }

        void translate_sv_coords(int x, int y)
        {
            int i = y - usable_anchorY - sv_map_margin;
            int j = x - anchorX - sv_map_margin;

            i = (i < 0) ? 0 : i;
            i = (i > sv_map_height) ? sv_map_height : i;

            j = (j < 0) ? 0 : j;
            j = (j > sv_map_width) ? sv_map_width : j;

            editor->active_color.set_from_hsv(editor->active_color.h, (double)j/sv_map_width, (double)(sv_map_height-i)/sv_map_height, editor->active_color.a);
        }

        bool click_hue_slider(int button, int x, int y, bool held)
        {
            bool tap =
                    button == 0 &&
                    x > anchorX + sv_map_margin &&
                    y > usable_anchorY + sv_map_margin*2 + sv_map_height &&
                    x < anchorX + sv_map_margin + sv_map_width &&
                    y < usable_anchorY + sv_map_margin*2 + sv_map_height + 10;

            hue_slider_simple_held = hue_slider_simple_held || tap;

            return tap || hue_slider_simple_held;
        }

        void translate_hue_coords(int x, int y)
        {
            x = (x-anchorX-sv_map_margin);
            x = (x < 0) ? 0 : x;
            x = (x > sv_map_width) ? sv_map_width : x;

            int hue = ((double)(x)/sv_map_width)*360.0;
            editor->active_color.set_from_hsv(hue, editor->active_color.s, editor->active_color.v, editor->active_color.a);
        }
        
        void translate_alpha_coords(int x, int y)
        {
            x = (x-anchorX-sv_map_margin);
            x = (x < 0) ? 0 : x;
            x = (x > sv_map_width) ? sv_map_width : x;

            int alpha = ((double)(x)/sv_map_width)*255.0;
            editor->active_color.set_from_hsv(editor->active_color.h, editor->active_color.s, editor->active_color.v, alpha);
        }

        bool click_alpha_slider(int button, int x, int y, bool held)
        {
            bool tap =
                    button == 0 &&
                    x > anchorX + sv_map_margin &&
                    y > usable_anchorY + sv_map_margin*3.5 + sv_map_height &&
                    x < anchorX + sv_map_margin + sv_map_width &&
                    y < usable_anchorY + sv_map_margin*3.5 + sv_map_height + 10;

            alpha_slider_simple_held = alpha_slider_simple_held || tap;

            return tap || alpha_slider_simple_held;
        }

        int click_rgba_plus_minus(int button, int x, int y)
        {
            int custom_anchorX = anchorX + sv_map_margin;
            int custom_anchorY = usable_anchorY + sv_map_margin*5 + sv_map_height;

            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    if (button == 0 &&
                        x > custom_anchorX + 110 + 40*j &&
                        y > custom_anchorY + 40*i &&
                        x < custom_anchorX + 110 + 40*j + 30 &&
                        y < custom_anchorY + 40*i + 30
                    )
                    {
                        return i * 2 + j;
                    }
                }
            }

            return -1;
        }

        void update_color_picker(int button, int x, int y, bool held, int state)
        {
            if (click_hue_slider(button, x, y, held))
            {
                translate_hue_coords(x, y);
            }

            else if (click_alpha_slider(button, x, y, held))
            {
                translate_alpha_coords(x, y);
            }

            else if (click_sv_map(x, y, held))
            {
                translate_sv_coords(x, y);
            }

            else if(state == 0)
            {
                int button_clicked = click_rgba_plus_minus(button, x, y);
                if (button_clicked == -1)
                {
                    return;
                }
                
                int r, g, b, a;
                switch (button_clicked)
                {
                    case 0:
                        r = (editor->active_color.r > 0) ? editor->active_color.r-1 : editor->active_color.r;
                        editor->active_color.set_from_rgb(r, editor->active_color.g, editor->active_color.b, editor->active_color.a);
                        break;

                    case 1:
                        r = (editor->active_color.r < 255) ? editor->active_color.r+1 : editor->active_color.r;
                        editor->active_color.set_from_rgb(r, editor->active_color.g, editor->active_color.b, editor->active_color.a);
                        break;

                    case 2:
                        g = (editor->active_color.g > 0) ? editor->active_color.g-1 : editor->active_color.g;
                        editor->active_color.set_from_rgb(editor->active_color.r, g, editor->active_color.b, editor->active_color.a);
                        break;

                    case 3:
                        g = (editor->active_color.g < 255) ? editor->active_color.g+1 : editor->active_color.g;
                        editor->active_color.set_from_rgb(editor->active_color.r, g, editor->active_color.b, editor->active_color.a);
                        break;

                    case 4:
                        b = (editor->active_color.b > 0) ? editor->active_color.b-1 : editor->active_color.b;
                        editor->active_color.set_from_rgb(editor->active_color.r, editor->active_color.g, b, editor->active_color.a);
                        break;

                    case 5:
                        b = (editor->active_color.b < 255) ? editor->active_color.b+1 : editor->active_color.b;
                        editor->active_color.set_from_rgb(editor->active_color.r, editor->active_color.g, b, editor->active_color.a);
                        break;
                        
                    case 6:
                        a = (editor->active_color.b > 0) ? editor->active_color.a-1 : editor->active_color.a;
                        editor->active_color.set_from_rgb(editor->active_color.r, editor->active_color.g, editor->active_color.b, a);
                        break;

                    case 7:
                        a = (editor->active_color.a < 255) ? editor->active_color.a+1 : editor->active_color.a;
                        editor->active_color.set_from_rgb(editor->active_color.r, editor->active_color.g, editor->active_color.b, a);
                        break;
                }

            }
        }

        bool inside_widget(int x, int y)
        {
            return x > anchorX &&
                    y > anchorY &&
                    x < anchorX + width &&
                    y < anchorY + height;
        }

        void update_slider_bools(bool held)
        {
            hue_slider_simple_held = hue_slider_simple_held && held;
            alpha_slider_simple_held = alpha_slider_simple_held && held;
            sv_map_simple_held = sv_map_simple_held && held;
        }

        void update_state(int state, int button, int x, int y, bool held)
        {
            update_slider_bools(held);
            if (inside_widget(x, y)) // checks for left click
            {
                if (state == 0 && button == 0)
                {
                    update_widget_frame(x, y);

                    if (current == WIDGET_LAYER_SELECTOR)
                    {
                        update_layer_selector(x, y);
                    }
                }
            }            
            
            if (current == WIDGET_COLOR_PICKER)
            {
                update_color_picker(button, x, y, held, state);
            }
        }

        void display_widget_frame()
        {
            CVpro::color(63, 63, 63);
            CV::rectFill(anchorX, usable_anchorY, anchorX + width, anchorY + height);

            layer_selector_icon->display_bitmap(anchorX + button_x_offset, anchorY, 1.0);
            color_picker_icon->display_bitmap(anchorX + button_x_offset*2 + button_width, anchorY, 1.0);
        }

        void show_arrows()
        {
            up_arrow_icon->display_bitmap_anchored(anchorX+width/2.0, usable_anchorY+25, 1.0, 'c', 't');
            down_arrow_icon->display_bitmap_anchored(anchorX+width/2.0, usable_anchorY+315, 1.0, 'c', 't');
        }

        void show_layer_dropdown()
        {
            int actual_shown = 0;
            int total_layers = layer_manager->layers.size();

            int iterations = std::min(total_layers, 3);
            int max_iterations = iterations + first_shown;
            for (int i = first_shown; i < iterations + first_shown; i++)
            {
                // background
                CVpro::color(32, 32, 32);
                CV::rectFill(anchorX+10, usable_anchorY+60+85*(max_iterations-i-1), anchorX+width-10, usable_anchorY+60+85*(max_iterations-i-1)+75);

                // show layer info
                CVpro::color(255, 255, 255);
                CVpro::text(anchorX+20, usable_anchorY+60+85*(max_iterations-i-1)+20, "#%d: %s", i+1, layer_manager->layers[i].name);

                // show image scaled to small size
                layer_manager->layers[i].image->display_bitmap(anchorX+20, usable_anchorY+60+85*(max_iterations-i-1)+30, 0.075);

                // active highlight
                if (layer_manager->active_index == i)
                {
                    CVpro::color(255, 255, 255);
                    CV::rect(anchorX+10, usable_anchorY+60+85*(max_iterations-i-1), anchorX+width-10, usable_anchorY+60+85*(max_iterations-i-1)+75);
                }

                // delete icon
                trash_icon->display_bitmap_anchored(anchorX + width-50, usable_anchorY+60+85*(max_iterations-i-1)+75/2.0, 1.0, 'r', 'c');

                // visibility
                if (layer_manager->layers[i].visible)
                {
                    open_eye_icon->display_bitmap_anchored(anchorX + width-80, usable_anchorY+60+85*(max_iterations-i-1)+75/2.0, 1.0, 'r', 'c');
                }

                else
                {
                    closed_eye_icon->display_bitmap_anchored(anchorX + width-80, usable_anchorY+60+85*(max_iterations-i-1)+75/2.0, 1.0, 'r', 'c');
                }

                // z-index
                z_index_up_icon->display_bitmap_anchored(anchorX + width-20, usable_anchorY+60+85*(max_iterations-i-1)+75/2.0, 1.0, 'r', 'b');
                z_index_down_icon->display_bitmap_anchored(anchorX + width-20, usable_anchorY+60+85*(max_iterations-i-1)+75/2.0, 1.0, 'r', 't');

                actual_shown++;
            }
            
            CV::color(1, 1, 1);
            if (actual_shown > 0)
            {
                CVpro::autotext(anchorX+width/2.0, usable_anchorY+360, 'c', 12,
                    "Showing layers #[%d-%d]\nout of %d layers.",
                     first_shown+1, first_shown+actual_shown, total_layers);
            }
            
            else
            {
                CVpro::autotext(anchorX+width/2.0, usable_anchorY+360, 'c', 12,
                    "There are no\nlayers to show.",
                     first_shown+1, first_shown+actual_shown, total_layers);
            }
        }

        void show_new_layer_icon()
        {
            new_layer_icon->display_bitmap_anchored(anchorX + width/2.0, usable_anchorY+400, 1.0, 'c', 't');
        }

        void display_layer_selector()
        {
            show_arrows();
            show_layer_dropdown();
            show_new_layer_icon();
        }

        void regenerate_sv_map()
        {
            Color tmp;

            for (int i = 0; i < sv_map_height; i++)
            {
                for (int j = 0; j < sv_map_width; j++)
                {
                    tmp.set_from_hsv(editor->active_color.h, (double)j/sv_map_width, (double)(sv_map_height-i)/sv_map_height, editor->active_color.a);
                    int base_index = i * sv_map_width * 4 + j * 4;
                    sv_map->matrix[base_index + 2] = tmp.r;
                    sv_map->matrix[base_index + 1] = tmp.g; 
                    sv_map->matrix[base_index    ] = tmp.b; 
                    sv_map->matrix[base_index + 3] = tmp.a; 
                }
            }
        }

        CVpro::image *generate_sv_map_base()
        {
            int bytes = 4; // (rgb + alpha = 4)
            subpixel *matrix = (subpixel *)calloc(1, sizeof(subpixel) * sv_map_width * sv_map_height * bytes);
            CVpro::image *img = new CVpro::image(sv_map_width, sv_map_height, matrix);

            return img;
        }

        CVpro::image *generate_alpha_slider_base()
        {
            int bytes = 4; // (rgb + alpha = 4)
            subpixel *matrix = (subpixel *)calloc(1, sizeof(subpixel) * sv_map_width * 10 * bytes);

            for (int i = 0; i < 10; i++)
            {
                for (int j = 0; j < sv_map_width; j++)
                {
                    int base_index = (sv_map_width * i * bytes + j * bytes);
                    if (i > 4)
                    {
                        if ((j / 5) % 2 == 0)
                        {
                            matrix[base_index + 2] = (unsigned char)192;
                            matrix[base_index + 1] = (unsigned char)192;
                            matrix[base_index    ] = (unsigned char)192;
                            matrix[base_index + 3] = (unsigned char)255;
                        }
                        
                        else
                        {
                            matrix[base_index + 2] = (unsigned char)128;
                            matrix[base_index + 1] = (unsigned char)128;
                            matrix[base_index    ] = (unsigned char)128;
                            matrix[base_index + 3] = (unsigned char)255;
                        }
                    }

                    else
                    {
                        if ((j / 5) % 2 != 0)
                        {
                            matrix[base_index + 2] = (unsigned char)192;
                            matrix[base_index + 1] = (unsigned char)192;
                            matrix[base_index    ] = (unsigned char)192;
                            matrix[base_index + 3] = (unsigned char)255;
                        }
                        
                        else
                        {
                            matrix[base_index + 2] = (unsigned char)128;
                            matrix[base_index + 1] = (unsigned char)128;
                            matrix[base_index    ] = (unsigned char)128;
                            matrix[base_index + 3] = (unsigned char)255;
                        }
                    }                    
                }                   
            }
            
            CVpro::image *img = new CVpro::image(sv_map_width, 10, matrix);
            return img;
        }

        CVpro::image *generate_alpha_slider_result()
        {
            int bytes = 4; // (rgb + alpha = 4)
            subpixel *matrix = (subpixel *)calloc(1, sizeof(subpixel) * sv_map_width * 10 * bytes);
            CVpro::image *img = new CVpro::image(sv_map_width, 10, matrix);

            return img;
        }

        void regenerate_alpha_slider()
        {
            for (int j = 0; j < sv_map_width; j++)
            {
                for (int i = 0; i < 10; i++)
                {
                    int base_index = i * sv_map_width * 4 + j * 4;
                    pixel blended = alpha_blend_pixel(alpha_slider_base->matrix[base_index + 2], alpha_slider_base->matrix[base_index + 1], alpha_slider_base->matrix[base_index], alpha_slider_base->matrix[base_index + 3],
                                                        editor->active_color.r, editor->active_color.g, editor->active_color.b, 255*(j/(float)sv_map_width));
                    
                    alpha_slider_result->matrix[base_index + 2] = blended.r;
                    alpha_slider_result->matrix[base_index + 1] = blended.g;
                    alpha_slider_result->matrix[base_index    ] = blended.b;
                    alpha_slider_result->matrix[base_index + 3] = blended.a;
                }
            }
        }

        void display_sv_map()
        {
           sv_map->display_bitmap(anchorX + sv_map_margin, usable_anchorY + sv_map_margin, 1.0);   

           CVpro::color(editor->active_color.r, editor->active_color.g, editor->active_color.b, editor->active_color.a);
           CV::circleFill(anchorX + sv_map_margin + editor->active_color.s*sv_map_width, usable_anchorY + sv_map_margin + (1-editor->active_color.v)*sv_map_height, 10, 20);

           CVpro::color(255, 255, 255);
           CV::circle(anchorX + sv_map_margin + editor->active_color.s*sv_map_width, usable_anchorY + sv_map_margin + (1-editor->active_color.v)*sv_map_height, 10, 20);         
        }

        void display_hue_slider()
        {
            Color tmp;
            for (int j = 0; j < sv_map_width; j++)
            {
                tmp.set_from_hsv(((double)j/sv_map_width)*360, 1, 1, 255);
                CVpro::color(tmp.r, tmp.g, tmp.b, tmp.a);

                for (int i = 0; i < 10; i++)
                {
                    CV::point(anchorX + sv_map_margin + j, usable_anchorY + sv_map_margin*2 + sv_map_height + i);
                }   
            }
            
            int posX = (editor->active_color.h/360.0)*sv_map_width;

            tmp.set_from_hsv(editor->active_color.h, 1, 1, 255);
            CVpro::color(tmp.r, tmp.g, tmp.b, tmp.a);
            CV::circleFill(anchorX + sv_map_margin + posX, usable_anchorY + sv_map_margin*2 + sv_map_height + 5, 10, 20);

            CVpro::color(255, 255, 255);
            CV::circle(anchorX + sv_map_margin + posX, usable_anchorY + sv_map_margin*2 + sv_map_height + 5, 10, 20);
        }

        void display_alpha_slider()
        {
            alpha_slider_result->display_bitmap(anchorX + sv_map_margin, usable_anchorY + sv_map_margin*3.5 + sv_map_height, 1.0);

            Color tmp;
            tmp.set_from_hsv(editor->active_color.h, std::min((double)editor->active_color.a/256.0, editor->active_color.s), std::max(1-(double)editor->active_color.a/256.0, editor->active_color.v), 255);

            int posX = (editor->active_color.a/256.0)*sv_map_width;
            CVpro::color(tmp.r, tmp.g, tmp.b);
            CV::circleFill(anchorX + sv_map_margin + posX, usable_anchorY + sv_map_margin*3.5 + sv_map_height + 5, 10, 20);

            CVpro::color(255, 255, 255);
            CV::circle(anchorX + sv_map_margin + posX, usable_anchorY + sv_map_margin*3.5 + sv_map_height + 5, 10, 20);
        }

        void draw_special_alpha_symbol(int x, int y)
        {
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    CVpro::color(192, 192, 192);
                    CVpro::color(128, 128, 128);

                    if (i % 2 == 0)
                    {
                        if (j % 2 == 0)
                        {
                            CVpro::color(192, 192, 192);
                        }

                        else
                        {
                            CVpro::color(128, 128, 128);
                        }
                    }

                    else
                    {
                        if (j % 2 == 0)
                        {
                            CVpro::color(128, 128, 128);
                        }

                        else
                        {
                            CVpro::color(192, 192, 192);
                        }
                    }
                    
                    CV::rectFill(x + j*30/2, y + 40*3 + i*30/2, x + (j+1)*30/2, y + 40*3 + (i+1)*30/2);
                }
            }
        }

        void display_rgb_values()
        {
            int custom_anchorX = anchorX + sv_map_margin;
            int custom_anchorY = usable_anchorY + sv_map_margin*5 + sv_map_height;
            for (int i = 0; i < 4; i++)
            {
                CVpro::color(40, 40, 40);
                CV::rectFill(custom_anchorX + 40, custom_anchorY + 40*i, custom_anchorX + 100, custom_anchorY + 40*i + 30);

                CVpro::color(255, 255, 255);
                CV::rect(custom_anchorX + 40, custom_anchorY + 40*i, custom_anchorX + 100, custom_anchorY + 40*i + 30);
                
                minus_icon->display_bitmap(custom_anchorX + 110, custom_anchorY + 40*i, 1.0);
                plus_icon->display_bitmap(custom_anchorX + 150, custom_anchorY + 40*i, 1.0);
            }

            CVpro::color(255, 0, 0);
            CV::rectFill(custom_anchorX, custom_anchorY, custom_anchorX + 30, custom_anchorY + 30);
            CVpro::color(0, 255, 0);
            CV::rectFill(custom_anchorX, custom_anchorY + 40*1, custom_anchorX + 30, custom_anchorY + 40*1 + 30);
            CVpro::color(0, 0, 255);
            CV::rectFill(custom_anchorX, custom_anchorY + 40*2, custom_anchorX + 30, custom_anchorY + 40*2 + 30);
            
            draw_special_alpha_symbol(custom_anchorX, custom_anchorY);
            
            CVpro::text_align(custom_anchorX + 40 + 60/2, custom_anchorY + 20, 'c', "%d", editor->active_color.r);
            CVpro::text_align(custom_anchorX + 40 + 60/2, custom_anchorY + 20 + 40*1, 'c', "%d", editor->active_color.g);
            CVpro::text_align(custom_anchorX + 40 + 60/2, custom_anchorY + 20 + 40*2, 'c', "%d", editor->active_color.b);
            CVpro::text_align(custom_anchorX + 40 + 60/2, custom_anchorY + 20 + 40*3, 'c', "%d", editor->active_color.a);

            CVpro::color(255, 255, 255);
            CV::rect(custom_anchorX + 190, custom_anchorY, anchorX + sv_map_width + sv_map_margin, custom_anchorY + 150);
            CVpro::color(editor->active_color.r, editor->active_color.g, editor->active_color.b);
            CV::rectFill(custom_anchorX + 190, custom_anchorY, anchorX + sv_map_width + sv_map_margin, custom_anchorY + 150);
        }

        void display_color_picker()
        {
            if (!(last_color == editor->active_color))
            {
                last_color = editor->active_color;
                regenerate_alpha_slider();
                regenerate_sv_map();
            }

            CV::color(1, 1, 1);
            display_sv_map();
            display_hue_slider();
            display_alpha_slider();
            display_rgb_values();
        }

        void display()
        {
            display_widget_frame();

            if (current == WIDGET_LAYER_SELECTOR)
            {
                display_layer_selector();
            }

            if (current == WIDGET_COLOR_PICKER)
            {
                display_color_picker();
            }
        }

};


#endif