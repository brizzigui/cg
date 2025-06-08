#ifndef PREVIEW_H
#define PREVIEW_H

#include <vector>
#include "Vector2.h"
#include "Vector3.h"

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
        std::vector<Vector2> *R2_points;
        std::vector<Triangle> R2_projections;
        std::vector<Triangle> R3_triangles;

        float anchorX, anchorY;
        float width, height;
        float padding;

        float dist = 3000.0;
        float scale = 0.5;

        Vector3 project(Vector3 point, float d);
        std::vector<std::vector<Vector3>> rotate_bezier();
        
    public:
        Preview(std::vector<Vector2> *points, float screen_height, float screen_width);
        ~Preview();

        void draw();
        void update();
        void recreate();
};

#endif