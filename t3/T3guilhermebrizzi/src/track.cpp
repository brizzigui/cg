#include "track.h"

Vector2 Track::simplify(std::vector<Vector2> points, float v)
{
    int len = points.size();
    std::vector<Vector2> last = points;
    std::vector<Vector2> cur;

    while (len > 1)
    {
        for (int i = 0; i < (int)last.size()-1; i++)
        {
            cur.push_back(Vector2((1-v) * last[i].x + v * last[i+1].x, (1-v) * last[i].y + v * last[i+1].y));
        }
        
        len = cur.size();
        last = cur;
        cur.clear();
    }

    return cur[0];
}

void Track::paint(Vector2 v, float radius)
{
    for (int offset_y = -radius; offset_y < radius; offset_y++)
    {
        for (int offset_x = -radius; offset_x < radius; offset_x++)
        {
            if (offset_x * offset_x + offset_y * offset_y < radius * radius)
            {
                int actual_x = v.x + offset_x;
                int actual_y = v.y + offset_y;

                if (actual_x >= 0 && actual_x < texture->width && actual_y >= 0 && actual_y < texture->height)
                {
                    int base_index = actual_y * texture->width * 4 + actual_x * 4;
                    texture->matrix[base_index + 2] = (unsigned char)255;
                    texture->matrix[base_index + 1] = (unsigned char)255;
                    texture->matrix[base_index    ] = (unsigned char)255;
                    texture->matrix[base_index + 3] = (unsigned char)255;
                }
            }
        }
    }
}

void Track::imprint(std::vector<Vector2> points)
{
    for (float v = 0; v < 1.0; v += 0.001)
    {
        Vector2 current = simplify(points, v);
        paint(current, 2.5);
    }
}

CVpro::image *Track::generate_blank_bmp()
{
    int bytes = 4; // (rgb + alpha = 4)
    subpixel *matrix = (subpixel *)calloc(1, sizeof(subpixel) * Footprint::width * Footprint::height * bytes);
    CVpro::image *img = new CVpro::image(Footprint::width, Footprint::height, matrix);

    return img;
}

void Track::clear_texture()
{
    memset(texture->matrix, 0, sizeof(subpixel) * texture->width * texture->height * 4);
}

void Track::regenerate()
{
    clear_texture();
    for (int i = 0; i < (int)control.size(); i++)
    {
        imprint(control[i]);
    }   
}

void Track::draw()
{
    if (change)
    {
        footprint.clear();
    }
    
    box = texture->display_bitmap(x, y, footprint);
}

Track::Track(float x, float y, std::vector<std::vector<Vector2>> points) : Entity(x, y)
{
    control = points;
    texture = generate_blank_bmp();
    regenerate();
}