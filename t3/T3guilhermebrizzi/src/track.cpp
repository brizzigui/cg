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

void Track::paint(Vector2 v, float radius, CVpro::image *target, Color c)
{
    for (int offset_y = -radius; offset_y < radius; offset_y++)
    {
        for (int offset_x = -radius; offset_x < radius; offset_x++)
        {
            if (offset_x * offset_x + offset_y * offset_y < radius * radius)
            {
                int actual_x = v.x + offset_x;
                int actual_y = v.y + offset_y;

                if (actual_x >= 0 && actual_x < target->width && actual_y >= 0 && actual_y < target->height)
                {
                    int base_index = actual_y * target->width * 4 + actual_x * 4;
                    target->matrix[base_index + 2] = (unsigned char)c.r;
                    target->matrix[base_index + 1] = (unsigned char)c.g;
                    target->matrix[base_index    ] = (unsigned char)c.b;
                    target->matrix[base_index + 3] = (unsigned char)c.a;
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
        paint(current, 2.5, texture, Color());
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
    memset(background->matrix, 0, sizeof(subpixel) * background->width * background->height * 4);
}

bool Track::is_border_pixel(int base_index)
{
    return texture->matrix[base_index + 2] == (unsigned char)TRACK_BORDER_COLOR_R &&
                    texture->matrix[base_index + 1] == (unsigned char)TRACK_BORDER_COLOR_G &&
                    texture->matrix[base_index    ] == (unsigned char)TRACK_BORDER_COLOR_B &&
                    texture->matrix[base_index + 3] == (unsigned char)TRACK_BORDER_COLOR_A;
}

#include <queue>

void Track::flood_fill(std::vector<uint8_t> &mask, int start_x, int start_y, int label)
{
    if (start_x < 0 || start_y < 0 || start_x >= texture->width || start_y >= texture->height)
        return;

    std::queue<std::pair<int, int>> q;
    q.push({start_x, start_y});

    while (!q.empty())
    {
        auto [x, y] = q.front();
        q.pop();

        int index = y * texture->width + x;
        int base_index = y * texture->width * 4 + x * 4;

        if (mask[index] != 0 || is_border_pixel(base_index))
            continue;

        mask[index] = label;

        if (x > 0) q.push({x - 1, y});
        if (x < texture->width - 1) q.push({x + 1, y});
        if (y > 0) q.push({x, y - 1});
        if (y < texture->height - 1) q.push({x, y + 1});
    }
}

void Track::prettyfy()
{
    CVpro::image *grass = CVpro::load_bitmap("./T3guilhermebrizzi/assets/environment/grass.bmp");
    CVpro::image *asphalt = CVpro::load_bitmap("./T3guilhermebrizzi/assets/environment/asphalt.bmp");

    std::vector<uint8_t> mask(texture->width * texture->height, 0);

    Vector2 mid_pt = Vector2((control[0][0].x + control[4][0].x)/2, (control[0][0].y + control[4][0].y)/2);
    flood_fill(mask, 0, 0, 1);                                      // outside = 1
    flood_fill(mask, mid_pt.x, mid_pt.y, 2);                        // middle = 2
    flood_fill(mask, texture->width / 2, texture->height / 2, 3);   // center = 3

    for (int i = 0; i < texture->height; i++)
    {
        for (int j = 0; j < texture->width; j++)
        {
            int base_index = i * texture->width * 4 + j * 4;
            if(background->matrix[base_index + 3] == 0)
            {
                uint8_t region = mask[i * texture->width + j];

                const CVpro::image *src = (region == 2) ? asphalt : grass;
                int src_index = (i % src->height) * src->width * 4 + (j % src->width) * 4;

                background->matrix[base_index + 2] = src->matrix[src_index + 2];
                background->matrix[base_index + 1] = src->matrix[src_index + 1];
                background->matrix[base_index    ] = src->matrix[src_index    ];
                background->matrix[base_index + 3] = (unsigned char)255;
            }
        }
    }
}

void Track::imprint_with_detail(std::vector<Vector2> outer, std::vector<Vector2> inner)
{
    bool dash = false;
    for (float v = 0; v < 1.0; v += 0.001)
    {
        Vector2 outer_value = simplify(outer, v);
        Vector2 inner_value = simplify(inner, v);

        paint(outer_value, 2.5, texture, Color());
        paint(inner_value, 2.5, texture, Color());
        if ((dash && ((int)(v*1000))%20 == 0) || (!dash && ((int)(v*1000))%100 == 0))
        {
            dash = !dash;
        }
        
        if (dash)
        {
            paint(
                Vector2((outer_value.x+inner_value.x)/2.0, (outer_value.y+inner_value.y)/2.0),
                5, background, Color(168, 152, 0, 255));
        }
    }
}

void Track::regenerate(bool barebones)
{
    clear_texture();

    for (int i = 0; i < (int)control.size(); i++)
    {
        if(!barebones)
        {
            imprint_with_detail(control[i], control[i+4]);
            if (i == 3) break;
        }
        else
        {
            imprint(control[i]);
        }
    }

    if (!barebones)
        prettyfy();
}


#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

void Track::draw_background()
{
    // Assume background->matrix is a pointer to BGRA pixel data (1280x720)
    static GLuint bgTexture = 0;

    // Initialize the texture once
    if (bgTexture == 0) {
        glGenTextures(1, &bgTexture);
        glBindTexture(GL_TEXTURE_2D, bgTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // Allocate texture storage
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 720, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
    }

    // Upload the BGRA pixel data into the texture
    glBindTexture(GL_TEXTURE_2D, bgTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1280, 720, GL_BGRA, GL_UNSIGNED_BYTE, background->matrix);

    // Save GL state
    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
    GLint depthMask = 0;
    glGetIntegerv(GL_DEPTH_WRITEMASK, &depthMask);

    // Disable depth testing to ensure background is always behind
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    // Set up an orthographic projection covering the viewport
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, 1280.0, 0.0, 720.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Draw a fullscreen textured quad
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f,   0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(1280.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(1280.0f, 720.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f,   720.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // Restore matrices and state
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    if (depthTestEnabled) glEnable(GL_DEPTH_TEST);
    glDepthMask(depthMask);

    // Now the background is drawn; subsequent 3rd-party rendering can proceed on top
}

void Track::draw_barebones()
{
    texture->display_bitmap(x, y);
}

void Track::draw()
{
    if (change)
    {
        footprint.clear();
    }
    
    draw_background();
    box = texture->display_bitmap(x, y, footprint);
}

Track::Track(float x, float y, std::vector<std::vector<Vector2>> points) : Entity(x, y)
{
    control = points;
    texture = generate_blank_bmp();
    background = generate_blank_bmp();
    regenerate(false);
}