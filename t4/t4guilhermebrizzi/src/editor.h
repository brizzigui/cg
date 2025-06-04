#ifndef EDITOR_H
#define EDITOR_H

#include <vector>
#include "Vector2.h"

class Editor
{
    private:
        float anchorX, anchorY;
        float width, height;
        float padding;
        float button_anchorX, button_anchorY, button_size;

        int mode;
        int control_point_held;

        std::vector<Vector2> *points;
        std::vector<Vector2> control_points;

        bool held;
        void screen_adjust_position();
        void screen_deadjust_position();
        void draw_background();
        void draw_control();
        void draw_bezier();
        void draw_interface();
        bool interface_interaction(int button, int state, int x, int y);
        bool move(int button, int state, int x, int y, bool held);
        bool add(int button, int state, int x, int y);
        bool kill(int button, int state, int x, int y);
        Vector2 simplify(float v);

    public:
        Editor(std::vector<Vector2> *points, float screen_height, float screen_width);
        ~Editor();

        void draw();
        bool update(int button, int state, int x, int y, bool held);
        void regenerate_curve();

};

#endif