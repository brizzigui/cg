#ifndef SIDE_MENU_H
#define SIDE_MENU_H

#include <iostream>
#include <vector>
#include "gl_canvas2d.h"
#include "canvas_pro.h"
#include "layer_manager.h"

#define ICON_SIZE 40
#define MENU_ANCHOR 10
#define MENU_SPACING 50

struct action
{
    // ponteiros para função do comportamento
    // de clique dos botões de ação

    // innate properties
    CVpro::image *icon;
    const char *label;
    bool selectable;

    // state properties
    bool selected = false;
};

typedef struct action Action;

class Interface
{
    private:
        int screenWidth, screenHeight;
        Layer_Manager *layer_manager;

        void display_background()
        {
            CVpro::color(63, 63, 63);
            CV::rectFill(0, 0, 60, screenHeight);
        }

    public:
        std::vector<Action> actions;

        Interface(int screenWidth, int screenHeight, Layer_Manager *layer_manager)
        {
            this->screenWidth = screenWidth;
            this->screenHeight = screenHeight;
            this->layer_manager = layer_manager;
        }

        void register_action(const char *label, CVpro::image *icon, bool selectable)
        {
            Action a;
            a.label = label;
            a.icon = icon;
            a.selectable = selectable;

            actions.push_back(a);
        }

        void create_default_actions()
        {
            CVpro::image *pencil_icon = CVpro::load_bitmap("./MeuPrograma/images/pencil.bmp");
            register_action("Pencil", pencil_icon, true);

            CVpro::image *brush_icon = CVpro::load_bitmap("./MeuPrograma/images/brush.bmp");
            register_action("Brush", brush_icon, true);

            CVpro::image *marker_icon = CVpro::load_bitmap("./MeuPrograma/images/marker.bmp");
            register_action("Marker", marker_icon, true);

            CVpro::image *eraser_icon = CVpro::load_bitmap("./MeuPrograma/images/eraser.bmp");
            register_action("Eraser", eraser_icon, true);

            CVpro::image *effects_icon = CVpro::load_bitmap("./MeuPrograma/images/effects.bmp");
            register_action("Effects", effects_icon, false);

            CVpro::image *horizontal_flip = CVpro::load_bitmap("./MeuPrograma/images/horizontal_flip.bmp");
            register_action("Horizontal Flip", horizontal_flip, false);

            CVpro::image *vertical_flip = CVpro::load_bitmap("./MeuPrograma/images/vertical_flip.bmp");
            register_action("Vertical Flip", vertical_flip, false);
        }

        bool validate_click(int i, int x, int y)
        {
            return (x >= MENU_ANCHOR) &&
                   (x <= MENU_ANCHOR + ICON_SIZE) && 
                   (y >= MENU_ANCHOR + MENU_SPACING * i) &&
                   (y <= MENU_ANCHOR + MENU_SPACING * i + ICON_SIZE);
        }


        void instant_execute(Action a)
        {
            if (strcmp(a.label, "Effects") == 0)
            {
                std::cout << "Called Effects execute" << std::endl;
            }
            else if (strcmp(a.label, "Horizontal Flip") == 0)
            {
                layer_manager->flip_active_horizontal();
            }
            else if (strcmp(a.label, "Vertical Flip") == 0)
            {
                layer_manager->flip_active_vertical();
            }
        }

        void change_selected_action(int index)
        {
            for (int i = 0; i < actions.size(); i++)
            {
                actions[i].selected = false;
            }

            if (actions[index].selectable)
            {
                actions[index].selected = true;
            }
            else
            {
                instant_execute(actions[index]);
            }
        }

        void process_special_clicks(int i)
        {
            std::cout << "Recieved special click." << std::endl;
        }

        void update_state(int button, int x, int y)
        {
            if (button == 0 || button == 2) // left click or right click
            {
                for (int i = 0; i < actions.size(); i++)
                {
                    if (validate_click(i, x, y))
                    {
                        change_selected_action(i);

                        if (button == 0)    // left click
                        {
                            printf("Called a left click event on action labled '%s'\n", actions[i].label);
                        }

                        else    // right click
                        {
                            printf("Called a right click event on action labled '%s'\n", actions[i].label);
                            process_special_clicks(i);
                        }   
                    }
                }
            }
        }

        void draw_action(Action a, int x, int y)
        {
            a.icon->display_bitmap(x, y, 1);
            if (a.selected)
            {
                CV::color(0.5, 0.5, 0.5);
                CV::rect(x, y, x + ICON_SIZE, y + ICON_SIZE);
            }
        }

        void display_actions()
        {
            for (int i = 0; i < actions.size(); i++)
            {
                draw_action(actions[i], MENU_ANCHOR, MENU_ANCHOR + i*MENU_SPACING);
            }
        }

        void display()
        {
            display_background();
            display_actions();
        }
};

#endif