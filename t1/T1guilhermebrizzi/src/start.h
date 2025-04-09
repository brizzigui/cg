#ifndef START_H
#define START_H

#include "canvas_pro.h"
#include "layer_manager.h"
#include <iostream>

#define START_NEW_PROJECT 0
#define START_LOAD_PROJECT 1
#define START_LOAD_DEMO 2

class Start
{
    private:
        CVpro::image *logo;
        CVpro::image *project_to_layer_icon;
        CVpro::image *back_arrow;

        int screen_width;
        int screen_height;
        bool opening_project;

        int button_height = 50;
        int button_width = 200;
        int button_spacing = 15;

        Layer_Manager *layer_manager;

        bool import_failure;

        char keyboard_buffer[256];
        char demo_path[64];

    public:
        bool is_open;

        Start(int screen_width, int screen_height, Layer_Manager *layer_manager)
        {
            this->screen_width = screen_width;
            this->screen_height = screen_height;

            logo = CVpro::load_bitmap("./T1guilhermebrizzi/images/logo.bmp");
            project_to_layer_icon = CVpro::load_bitmap("./T1guilhermebrizzi/images/project_to_layer.bmp");
            back_arrow = CVpro::load_bitmap("./T1guilhermebrizzi/images/back_arrow.bmp");

            is_open = true;
            opening_project = false;
            import_failure = false;

            this->layer_manager = layer_manager;
            strcpy(keyboard_buffer, "");
            strcpy(demo_path, "./T1guilhermebrizzi/projects/demo.bimp");
        };

        void close()
        {
            is_open = false;
            opening_project = false;
            import_failure = false;

            logo->destroy_bitmap();
            project_to_layer_icon->destroy_bitmap();
            back_arrow->destroy_bitmap();

            strcpy(keyboard_buffer, "");
        }

        void display_opening_project()
        {
            int anchorX = screen_width/2.0;
            int anchorY = screen_height/4.0;

            project_to_layer_icon->display_bitmap_anchored(anchorX, anchorY, 2.0, 'c', 't');

            CVpro::color(255, 255, 255);
            CVpro::autotext(anchorX, anchorY+150, 'c', 15, "Type a BIMP project file name:\n(root dir: ./T1guilhermebrizzi/projects/)");

            CVpro::color(0, 0, 0);
            CV::rectFill(anchorX-150, anchorY+200, anchorX+150, anchorY+230);

            CVpro::color(23, 196, 0);
            CVpro::autotext(anchorX, anchorY+220, 'c', 15, "%s.bimp", keyboard_buffer);

            CVpro::color(70, 70, 70);
            CV::rectFill(anchorX-75, anchorY+250, anchorX+75, anchorY+290);
            
            CVpro::color(255, 255, 255);
            CV::rect(anchorX-75, anchorY+250, anchorX+75, anchorY+290);

            CVpro::text_align(anchorX, anchorY+275, 'c', "Open");
            if (import_failure)
            {
                CVpro::color(255, 0, 0);
                CVpro::text_align(anchorX, anchorY+315, 'c', "File not found, try again.");
            }
            

            back_arrow->display_bitmap(10, 10, 1.0);
        }

        void display_splash_screen()
        {
            int anchorX = screen_width/2.0;
            int anchorY = screen_height/4.0 + 50;

            logo->display_bitmap_anchored(screen_width/2.0, anchorY, 1.0, 'c', 'c');
            CVpro::color(255, 127, 39);
            CVpro::text_align(screen_width/2.0 - 65, anchorY + 70, 'l', "Powered by Canvas2D");

            CVpro::color(100, 100, 100);
            CVpro::text_align(screen_width/2.0, screen_height-15, 'c', "Brizzi Image Manipulation Program - 2025");

            anchorX = screen_width/2.0;
            anchorY = 400;

            CVpro::color(255, 127, 39);
            CV::rectFill(anchorX - button_width/2.0, anchorY, anchorX + button_width/2.0, anchorY + button_height);
            CVpro::color(255, 255, 255);
            CV::rect(anchorX - 100, anchorY, anchorX + 100, anchorY + 50);
            CVpro::text_align(anchorX, anchorY+button_height*0.5 + 5, 'c', "New project");

            CVpro::color(100, 100, 100);
            CV::rectFill(anchorX - 100, anchorY + button_height + button_spacing, anchorX + 100, anchorY + button_height*2 + button_spacing);
            CVpro::color(255, 255, 255);
            CV::rect(anchorX - 100, anchorY + button_height + button_spacing, anchorX + 100, anchorY + button_height*2 + button_spacing);
            CVpro::text_align(anchorX, anchorY + button_height*1.5 + button_spacing + 5, 'c', "Load from file");

            CVpro::color(100, 100, 100);
            CV::rectFill(anchorX - 100, anchorY + button_height*2 + button_spacing*2, anchorX + 100, anchorY + button_height*3 + button_spacing*2);
            CVpro::color(255, 255, 255);
            CV::rect(anchorX - 100, anchorY + button_height*2 + button_spacing*2, anchorX + 100, anchorY + button_height*3 + button_spacing*2);
            CVpro::text_align(anchorX, anchorY+button_height*2.5 + button_spacing*2 + 5, 'c', "Load quick demo");
        }

        void display()
        {
            if (opening_project)
            {
                display_opening_project();
            }

            else
            {
                display_splash_screen();
            }

            Sleep(10);
        }

        bool clicked_open_project(int x, int y)
        {
            int anchorX = screen_width/2.0;
            int anchorY = screen_height/4.0;

            return x > anchorX-75 &&
                    y > anchorY+250 && 
                    x < anchorX+75 &&
                    y < anchorY+290;
        }

        bool check_back_button(int x, int y)
        {
            return x > 10 && y > 10 &&
                    x < 10 + back_arrow->width &&
                    y < 10 + back_arrow->height;
        }

        void update_opening_project(int x, int y)
        {
            if (clicked_open_project(x, y))
            {
                char path[256];
                sprintf(path, "./T1guilhermebrizzi/projects/%s.bimp", keyboard_buffer);
                if(layer_manager->restore_project(path))
                {
                    close();
                }

                else
                {
                    import_failure = true;
                }
            }

            else if(check_back_button(x, y))
            {
                opening_project = false;
                strcpy(keyboard_buffer, "");
                import_failure = false;
            }
        }

        int get_clicked_button(int x, int y)
        {
            int anchorX = screen_width/2.0;
            int anchorY = 400;

            for (int i = 0; i < 3; i++)
            {
                if (x > anchorX - button_width/2.0 &&
                    y > anchorY + button_height*i + button_spacing*i &&
                    x < anchorX + button_width/2.0 && 
                    y < anchorY + button_height*(i+1) + button_spacing*i)
                {
                    return i;
                }
            }

            return -1;
        }

        void update_splash_screen(int x, int y)
        {
            int button = get_clicked_button(x, y);
            switch (button)
            {
                case START_NEW_PROJECT:
                    close();
                    break;

                case START_LOAD_PROJECT:
                    opening_project = true;
                    break;

                case START_LOAD_DEMO:
                    layer_manager->restore_project(demo_path);
                    close();
                    break;
                
                default:
                    break;
            }
        }

        void update(int state, int button, int x, int y)
        {
            if (state == 0 && button == 0)
            {
                if (opening_project)
                {
                    update_opening_project(x, y);
                }
    
                else
                {
                    update_splash_screen(x, y);
                }
            }
        }

        void listen(int key)
        {
            if (is_open && opening_project)
            {
                int cur = strlen(keyboard_buffer);
                if (cur < 255)
                {
                    CVkeyboard::decode(keyboard_buffer, key);
                }
            }
        }
};


#endif