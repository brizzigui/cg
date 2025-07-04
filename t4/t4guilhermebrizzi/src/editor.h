#ifndef EDITOR_H
#define EDITOR_H

#include <vector>
#include "Vector2.h"
#include "canvas_pro.h"

class Editor
{
    private:
        float anchorX, anchorY;
        float width, height;
        float padding;
        float button_anchorX, button_anchorY, button_size;

        int mode;
        int control_point_held;

        int definition = 16;
        float step = 1.0/definition;

        std::vector<Vector2> *points;
        std::vector<Vector2> control_points;
        std::vector<Vector2> internal_points;

        std::vector<CVpro::image *> icons;
        std::vector<CVpro::image *> options;

        bool held;
        void screen_adjust_position();
        void screen_deadjust_position();
        void draw_background();
        void draw_control();
        void draw_bezier();
        void draw_interface();
        int interface_interaction(int button, int state, int x, int y);
        bool move(int button, int state, int x, int y, bool held);
        bool add(int button, int state, int x, int y);
        bool kill(int button, int state, int x, int y);
        Vector2 simplify(float v);
        void screen_adjust_position_middle();
        void set_definition(int definition);

    public:
        Editor(std::vector<Vector2> *points, float screen_height, float screen_width);
        ~Editor();

        void draw();
        bool update(int button, int state, int x, int y, bool held);
        void regenerate_curve();

};

#endif