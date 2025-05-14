#ifndef TRACK_H
#define TRACK_H

#include "entity.h"
#include <vector>
#include "Vector2.h"
#include "canvas_pro.h"

#define TRACK_BORDER_COLOR_R 255
#define TRACK_BORDER_COLOR_G 255
#define TRACK_BORDER_COLOR_B 255
#define TRACK_BORDER_COLOR_A 255

class Track : public Entity
{
    private:
        Vector2 simplify(std::vector<Vector2> points, float v);
        void imprint(std::vector<Vector2> points);
        CVpro::image *generate_blank_bmp();
        void clear_texture();
        void paint(Vector2 v, float radius);
        void prettyfy();
        bool is_border_pixel(int base_index);
        void draw_background();
        void flood_fill(std::vector<uint8_t> &mask, int start_x, int start_y, int label);

    public:
        // the track's control points
        std::vector<std::vector<Vector2>> control;
        CVpro::image *background;

        Track(float x, float y, std::vector<std::vector<Vector2>> points);
        void regenerate();
        virtual void draw();
};


#endif