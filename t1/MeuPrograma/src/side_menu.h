#ifndef SIDE_MENU_H
#define SIDE_MENU_H

#include <iostream>
#include <vector>
#include "gl_canvas2d.h"
#include "canvas_pro.h"

#define ICON_SIZE 40
#define MENU_ANCHOR 10
#define MENU_SPACING 50

struct action
{
    // ponteiros para função do comportamento
    // de clique dos botões de ação

    // innate properties
    void (*left_click_behavior)(void);
    void (*right_click_behavior)(void);
    CVpro::image *icon;
    const char *label;

    // state properties
    bool selected = false;
};

typedef struct action Action;

class Side_Menu
{
    private:
        std::vector<Action> actions;
        int screenWidth, screenHeight;

        void display_background()
        {
            CVpro::color(63, 63, 63);
            CV::rectFill(0, 0, 60, screenHeight);
        }

    public:
        Side_Menu(int screenWidth, int screenHeight)
        {
            this->screenWidth = screenWidth;
            this->screenHeight = screenHeight;
        }

        void register_action(const char *label, CVpro::image *icon, void (*left_click_behavior)(void), void (*right_click_behavior)(void))
        {
            Action a;
            a.label = label;
            a.icon = icon;
            a.left_click_behavior = left_click_behavior;
            a.right_click_behavior = right_click_behavior;

            actions.push_back(a);
        }

        bool validate_click(int i, int x, int y)
        {
            return (x >= MENU_ANCHOR) &&
                   (x <= MENU_ANCHOR + ICON_SIZE) && 
                   (y >= MENU_ANCHOR + MENU_SPACING * i) &&
                   (y <= MENU_ANCHOR + MENU_SPACING * i + ICON_SIZE);
        }

        void change_selected_action(int i)
        {
            for (int i = 0; i < actions.size(); i++)
            {
                actions[i].selected = false;
            }
            
            actions[i].selected = true;
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
                            printf("Called a left click event on action labled '%s'", actions[i].label);
                            if (actions[i].left_click_behavior != NULL)
                            {
                                (actions[i].left_click_behavior)();
                            }
                        }

                        else    // right click
                        {
                            printf("Called a right click event on action labled '%s'", actions[i].label);
                            if (actions[i].right_click_behavior != NULL)
                            {
                                (actions[i].right_click_behavior)();
                            }
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