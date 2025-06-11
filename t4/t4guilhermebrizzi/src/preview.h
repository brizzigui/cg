#ifndef PREVIEW_H
#define PREVIEW_H

#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "canvas_pro.h"
#include "color.h"

#define WIREFRAME_MODE 0
#define PIXEL_LIGHTING_MODE 1

#define DEFAULT_COLOR Color(241, 120, 38, 255)

struct Triangle
{
    Vector3 va, vb, vc;
    Triangle(Vector3 va, Vector3 vb, Vector3 vc)
    {
        this->va = va;
        this->vb = vb;
        this->vc = vc;
    }
};

class Preview
{
    private:
        int mode = PIXEL_LIGHTING_MODE;

        CVpro::image *result = NULL;

        std::vector<Vector2> *R2_points;
        std::vector<Triangle> R2_projections;
        std::vector<Triangle> R3_triangles;

        float anchorX, anchorY;
        bool held = false;
        float grabX, grabY;
        float width, height;
        float padding;

        float dist = 3000.0;
        float scale = 1.0;

        int slices = 32;
        bool grow_y = false;
        bool perspective = true;

        std::vector<float> zbuffer;
        Color paint_color = DEFAULT_COLOR;

        std::vector<CVpro::image *> icons;
        float button_anchorX, button_anchorY, button_size;

        Vector3 project(Vector3 point, float d);
        std::vector<std::vector<Vector3>> rotate_bezier();
        void triangularize();
        void flatten();
        void paint_result();
        void line_to_bmp(int x0, int y0, int x1, int y1);
        void set_pixel(int x, int y);
        void triangle_to_wireframe();
        void triangle_to_vertex_lighting();
        void triangle_to_pixel_lighting();
        Color compute_vertex_lighting(Vector3 pos, Vector3 normal, Vector3 light_dir);
        void draw_background();
        void rotate(float roll, float pitch, float yaw);
        Vector3 obtain_rotation(int x, int y);
        void draw_buttons();
        int check_buttons(int button, int state, int x, int y);
        void handle_model_manipulation(int button, int state, int x, int y);
        void handle_ui_input(int button, int state, int x, int y);
        
    public:
        Preview(std::vector<Vector2> *points, float screen_height, float screen_width);
        ~Preview();

        void draw();
        void update(int button, int state, int direction, int x, int y);
        void recreate();
        void recreate(float roll, float pitch, float yaw);
};

#endif