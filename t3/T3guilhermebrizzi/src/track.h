#ifndef TRACK_H
#define TRACK_H

#include "entity.h"
#include <vector>
#include "Vector2.h"
#include "canvas_pro.h"

class Track : public Entity
{
    private:
        Vector2 simplify(std::vector<Vector2> points, float v);
        void imprint(std::vector<Vector2> points);
        CVpro::image *generate_blank_bmp();
        void clear_texture();
        void paint(Vector2 v, float radius);

    public:
        // the track's control points
        std::vector<std::vector<Vector2>> control;

        Track(float x, float y, std::vector<std::vector<Vector2>> points);
        void regenerate();
        virtual void draw();
};


#endif