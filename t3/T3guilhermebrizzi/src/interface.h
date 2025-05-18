#ifndef INTERFACE_H
#define INTERFACE_H

#include <vector>
#include "canvas_pro.h"
#include "Vector2.h"
#include "track.h"

class Interface
{
    private:
        int screen_width, screen_height;
        bool editing = false;
        CVpro::image *title_logo;
        Track *track_preview;
        std::vector<std::vector<Vector2>> *points;

        bool holding_point = false;
        std::pair<int, int> point_held;

        void display_editor();
        void display_splashscreen();
        void update_editor(int button, int state, int x, int y);
        bool update_splashscreen(int button, int state, int x, int y);
        int get_clicked_button(int button, int state, int x, int y);
        std::pair<int, int> get_clicked_point(int x, int y);
        bool exit_clicked(int x, int y);

    public:
        Interface(int screen_width, int screen_height, std::vector<std::vector<Vector2>> *points);
        void display();
        bool update(int button, int state, int x, int y, bool held);
};

#endif