#ifndef SIDE_MENU_H
#define SIDE_MENU_H

#include <iostream>
#include <vector>
#include "gl_canvas2d.h"
#include "canvas_pro.h"
#include "layer_manager.h"
#include "popup.h"
#include "shared.h"

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
    bool label_active = false;
    void *values_ptr; // values may mean different things, such as size, type, etc
};

typedef struct action Action;

class Interface
{
    private:
        int screenWidth, screenHeight;
        Layer_Manager *layer_manager;
        Popup *popup;

        void display_background()
        {
            CVpro::color(63, 63, 63);
            CV::rectFill(0, 0, 60, screenHeight);
        }

    public:
        std::vector<Action> actions;

        Interface(int screenWidth, int screenHeight, Layer_Manager *layer_manager, Popup *popup)
        {
            this->screenWidth = screenWidth;
            this->screenHeight = screenHeight;
            this->layer_manager = layer_manager;
            this->popup = popup;
        }

        void register_action(const char *label, CVpro::image *icon, bool selectable, void *values_ptr)
        {
            Action a;
            a.label = label;
            a.icon = icon;
            a.selectable = selectable;
            a.values_ptr = values_ptr;

            actions.push_back(a);
        }

        void create_default_actions()
        {
            std_selectable_values *values;

            CVpro::image *move_icon = CVpro::load_bitmap("./MeuPrograma/images/move.bmp");
            register_action("Move", move_icon, true, NULL);

            values = new std_selectable_values(5, 0);
            CVpro::image *pencil_icon = CVpro::load_bitmap("./MeuPrograma/images/pencil.bmp");
            register_action("Pencil", pencil_icon, true, values);

            values = new std_selectable_values(10, 0);
            CVpro::image *spray_icon = CVpro::load_bitmap("./MeuPrograma/images/spray.bmp");
            register_action("Spray", spray_icon, true, values);

            values = new std_selectable_values(10, 0);
            CVpro::image *marker_icon = CVpro::load_bitmap("./MeuPrograma/images/marker.bmp");
            register_action("Marker", marker_icon, true, values);

            CVpro::image *fill_icon = CVpro::load_bitmap("./MeuPrograma/images/fill.bmp");
            register_action("Fill", fill_icon, true, NULL);

            values = new std_selectable_values(10, 0);
            CVpro::image *eraser_icon = CVpro::load_bitmap("./MeuPrograma/images/eraser.bmp");
            register_action("Eraser", eraser_icon, true, values);

            CVpro::image *picker_icon = CVpro::load_bitmap("./MeuPrograma/images/picker.bmp");
            register_action("Color Picker", picker_icon, true, NULL);
            
            CVpro::image *resize_icon = CVpro::load_bitmap("./MeuPrograma/images/resize.bmp");
            register_action("Resize", resize_icon, true, NULL);
            
            CVpro::image *effects_icon = CVpro::load_bitmap("./MeuPrograma/images/effects.bmp");
            register_action("Effects", effects_icon, false, NULL);

            CVpro::image *adjustments_icon = CVpro::load_bitmap("./MeuPrograma/images/adjustments.bmp");
            register_action("Adjustments", adjustments_icon, false, NULL);

            CVpro::image *horizontal_flip = CVpro::load_bitmap("./MeuPrograma/images/horizontal_flip.bmp");
            register_action("Horizontal Flip", horizontal_flip, false, NULL);

            CVpro::image *vertical_flip = CVpro::load_bitmap("./MeuPrograma/images/vertical_flip.bmp");
            register_action("Vertical Flip", vertical_flip, false, NULL);

            CVpro::image *save_icon = CVpro::load_bitmap("./MeuPrograma/images/save.bmp");
            register_action("Save", save_icon, false, NULL);

            CVpro::image *export_icon = CVpro::load_bitmap("./MeuPrograma/images/export.bmp");
            register_action("Export", export_icon, false, NULL);            
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
                popup->open(POPUP_ROUTINE_EFFECTS);
            }
            else if (strcmp(a.label, "Adjustments") == 0)
            {
                std::cout << "Called Adjustments execute" << std::endl;
                popup->open(POPUP_ROUTINE_ADJUSTMENTS);
            }
            else if (strcmp(a.label, "Horizontal Flip") == 0)
            {
                layer_manager->flip_active_horizontal();
            }
            else if (strcmp(a.label, "Vertical Flip") == 0)
            {
                layer_manager->flip_active_vertical();
            }
            else if (strcmp(a.label, "Save") == 0)
            {
                std::cout << "Called Save execute" << std::endl;
                popup->open(POPUP_ROUTINE_SAVE);
            }
            else if (strcmp(a.label, "Export") == 0)
            {
                std::cout << "Called Export execute" << std::endl;
                popup->open(POPUP_ROUTINE_EXPORT);
            }
        }

        void deselect_all_actions()
        {
            for (int i = 0; i < actions.size(); i++)
            {
                actions[i].selected = false;
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
            if (strcmp(actions[i].label, "Pencil") == 0)
            {
                popup->open(POPUP_ROUTINE_PENCIL_SETTINGS, actions[i].values_ptr);
            }
            else if (strcmp(actions[i].label, "Spray") == 0)
            {
                popup->open(POPUP_ROUTINE_SPRAY_SETTINGS, actions[i].values_ptr);
            }
            else if (strcmp(actions[i].label, "Marker") == 0)
            {
                popup->open(POPUP_ROUTINE_MARKER_SETTINGS, actions[i].values_ptr);
            }
            else if (strcmp(actions[i].label, "Eraser") == 0)
            {
                popup->open(POPUP_ROUTINE_ERASER_SETTINGS, actions[i].values_ptr);
            }
        }

        void set_label_active(int i)
        {
            actions[i].label_active = true;
        }

        void set_label_inactive(int i)
        {
            actions[i].label_active = false;
        }

        void update_state(int state, int button, int x, int y)
        {
            for (int i = 0; i < actions.size(); i++)
            {
                if (validate_click(i, x, y))
                {
                    if ((button == 0 || button == 2) && state == 0)
                    {
                        change_selected_action(i);

                        if (button == 2)
                        {
                            // right click
                            printf("Called a right click event on action labled '%s'\n", actions[i].label);
                            process_special_clicks(i);
                        }
                    }

                    else
                    {
                        set_label_active(i);
                    }
                }

                else
                {
                    set_label_inactive(i);
                }
            }
        }

        void draw_label(Action a, int x, int y)
        {
            int text_width = strlen(a.label) * 10;

            CVpro::color(0, 0, 0);
            CV::rectFill(x + 10, y+(ICON_SIZE/4.0), x + 30 + text_width, y + (ICON_SIZE/4.0) + 20);

            CVpro::color(255, 255, 255);
            CV::rect(x + 10, y+(ICON_SIZE/4.0), x + 30 + text_width, y + (ICON_SIZE/4.0) + 20);

            CVpro::text_align(x + 10 + 10 + text_width/2.0, y+(ICON_SIZE/4.0)+20/2.0+5, 'c', a.label);
        }

        void draw_action(Action a, int x, int y)
        {
            a.icon->display_bitmap(x, y, 1);
            if (a.selected)
            {
                CV::color(0.5, 0.5, 0.5);
                CV::rect(x, y, x + ICON_SIZE, y + ICON_SIZE);
            }

            if (a.label_active)
            {
                draw_label(a, x + MENU_SPACING, y);
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