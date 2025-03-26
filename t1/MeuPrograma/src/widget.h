#ifndef WIDGET_H
#define WIDGET_H

#include "layer_manager.h"
#include "interface.h"
#include "editor.h"

#define WIDGET_LAYER_SELECTOR 0
#define WIDGET_COLOR_PICKER 1

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

        Layer_Manager *layer_manager;
        Interface *interface;
        Editor *editor;

        CVpro::image *color_picker_icon;
        CVpro::image *layer_selector_icon;
        CVpro::image *up_arrow_icon;
        CVpro::image *down_arrow_icon;
        CVpro::image *new_layer_icon;

    public:
        Widget(Layer_Manager *layer_manager, Interface *interface, Editor *editor)
        {
            this->current = WIDGET_LAYER_SELECTOR;
            this->layer_manager = layer_manager;
            this->interface = interface;
            this->editor = editor;

            this->color_picker_icon = CVpro::load_bitmap("./MeuPrograma/images/picker_icon.bmp");
            this->layer_selector_icon = CVpro::load_bitmap("./MeuPrograma/images/layer_selector.bmp");
            this->up_arrow_icon = CVpro::load_bitmap("./MeuPrograma/images/up_arrow.bmp");
            this->down_arrow_icon = CVpro::load_bitmap("./MeuPrograma/images/down_arrow.bmp");
            this->new_layer_icon = CVpro::load_bitmap("./MeuPrograma/images/new_layer.bmp");
        }

        int check_frame_click(int button, int x, int y)
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

        void update_widget_frame(int button, int x, int y)
        {
            if (button == 0)
            {
                int choice = check_frame_click(button, x, y);
                if (choice == -1)
                {
                    return;
                }

                else
                {
                    current = choice;
                }
            }
        }

        void update_layer_selector(int button, int x, int y)
        {

        }

        void update_color_picker(int button, int x, int y)
        {

        }

        void update_state(int button, int x, int y)
        {
            update_widget_frame(button, x, y);

            if (current == WIDGET_LAYER_SELECTOR)
            {
                update_layer_selector(button, x, y);
            }

            if (current == WIDGET_COLOR_PICKER)
            {
                update_color_picker(button, x, y);
            }
        }

        void display_widget_frame()
        {
            CVpro::color(63, 63, 63);
            CV::rectFill(anchorX, usable_anchorY, anchorX + width, anchorY + height);

            layer_selector_icon->display_bitmap(anchorX + button_x_offset, anchorY, 1.0);
            color_picker_icon->display_bitmap(anchorX + button_x_offset*2 + button_width, anchorY, 1.0);
        }

        void display_layer_selector()
        {
            CV::color(1, 1, 1);
            up_arrow_icon->display_bitmap_anchored(anchorX+width/2.0, usable_anchorY+25, 1.0, 'c', 't');

            CVpro::color(32, 32, 32);
            for (int i = 0; i < 3; i++)
            {
                CV::rectFill(anchorX+10, usable_anchorY+60+85*i, anchorX+width-10, usable_anchorY+60+85*i+75);
            }

            down_arrow_icon->display_bitmap_anchored(anchorX+width/2.0, usable_anchorY+315, 1.0, 'c', 't');

            CV::color(1, 1, 1);
            CVpro::autotext(anchorX+width/2.0, usable_anchorY+360, 'c', 12, "Showing layers #[%d-%d]\nout of %d layers.", 1, 1, 1);
            new_layer_icon->display_bitmap_anchored(anchorX + width/2.0, usable_anchorY+400, 1.0, 'c', 't');
        }

        void display_color_picker()
        {
            CV::color(1, 1, 1);
            CVpro::autotext(anchorX+width/2.0, anchorY+height/2.0, 'c', 15, "I'm a color picker.\nMaybe implement me later.");
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