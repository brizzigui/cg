#include "editor.h"
#include "gl_canvas2d.h"
#include "canvas_pro.h"

Editor::Editor(std::vector<Vector2> *points, float screen_height, float screen_width)
{
    this->points = points;

    this->padding = 30;
    this->width = screen_width/2 - padding*2;
    this->height = screen_height - padding*2;
    this->anchorX = padding;
    this->anchorY = padding;
    this->button_size = 40;
    this->button_anchorX = width - button_size - button_size/3;
    this->button_anchorY = button_size/3;

    this->held = false;
    this->mode = 0;
    this->control_point_held = -1;

    this->icons.push_back(CVpro::load_bitmap("./t4guilhermebrizzi/assets/move.bmp"));
    this->icons.push_back(CVpro::load_bitmap("./t4guilhermebrizzi/assets/add.bmp"));
    this->icons.push_back(CVpro::load_bitmap("./t4guilhermebrizzi/assets/delete.bmp"));
}

Editor::~Editor()
{
}

void Editor::screen_adjust_position()
{
    CV::translate(anchorX, anchorY);
}

void Editor::screen_deadjust_position()
{
    CV::translate(0, 0);
}

void Editor::draw_background()
{
    CVpro::color(30, 30, 30);
    CV::rectFill(0, 0, width, height);

    for (int i = 0; i < 10; i++)
    {
        CVpro::color(50, 50, 50);

        if (i == 5)
        {
            CVpro::color(255, 255, 255);
        }

        CV::line(0, i*height/10, width, i*height/10);
        CV::line(i*width/10, 0, i*width/10, height);
        
    }

    CVpro::color(255, 255, 255);
    CV::rect(0, 0, width, height);
}

void Editor::draw_control()
{
    CVpro::color(241, 120, 38);
    for (auto p : control_points)
    {
        CV::circleFill(p.x, p.y, 5, 10);
    }
}

void Editor::draw_bezier()
{
    CVpro::color(164, 81, 25);
    for (auto p : internal_points)
    {
        CV::circleFill(p.x, p.y, 3, 10);
    }
}

void Editor::draw_interface()
{
    for (int i = 0; i < 3; i++)
    {
        icons[i]->display(button_anchorX, button_anchorY + 1.33*i*button_size);

        if (i == mode)
        {
            CVpro::color(200, 200, 200);
            CV::rect(button_anchorX, button_anchorY + 1.33*i*button_size,
                            button_anchorX + button_size, button_anchorY + 1.33*i*button_size + button_size);    
        }
    }
}

void Editor::screen_adjust_position_middle()
{
    CV::translate(anchorX + width/2.0, anchorY + height/2.0);
}

void Editor::draw()
{
    screen_adjust_position();
    draw_background();
    screen_adjust_position_middle();
    draw_control();
    draw_bezier();
    screen_adjust_position();
    draw_interface();
    screen_deadjust_position();
}

bool Editor::interface_interaction(int button, int state, int x, int y)
{
    for (int i = 0; i < 3; i++)
    {
        if (button == 0 && state == 0 &&
            x > button_anchorX && y > button_anchorY + 1.33*i*button_size &&
            x < button_anchorX + button_size && y < button_anchorY + 1.33*i*button_size + button_size)
        {
            mode = i;   
            return true;
        }
    }
    return false;
}

bool Editor::move(int button, int state, int x, int y, bool held)
{
    if (x < -width/2.0 || x > width/2.0 || y < -height/2.0 || y > height/2.0)
    {
        return false;
    }

    if (control_point_held == -1)
    {
        for (int i = 0; i < (int)control_points.size(); i++)
        {
            if (button == 0 && state == 0 &&
                pow(x-control_points[i].x, 2.0) + pow(y-control_points[i].y, 2.0) < 25.0)
            {
                control_point_held = i;
                return true;  
            }
        }
    }
    
    else if(held)
    {
        control_points[control_point_held].x = x;
        control_points[control_point_held].y = y;  
        return true;  
    }

    else
    {
        control_point_held = -1;
        return false;
    }

    return false;
}

bool Editor::add(int button, int state, int x, int y)
{
    if (x < -width/2.0 || x > width/2.0 || y < -height/2.0 || y > height/2.0 || button != 0 || state != 0)
    {
        return false;
    }
    
    control_points.push_back(Vector2(x, y));
    return true;
}

bool Editor::kill(int button, int state, int x, int y)
{
    for (int i = 0; i < (int)control_points.size(); i++)
    {
        if (button == 0 && state == 0 &&
            pow(x-control_points[i].x, 2.0) + pow(y-control_points[i].y, 2.0) < 25.0)
        {
            control_points.erase(control_points.begin()+i, control_points.begin()+i+1);
            return true;
        }
    }

    return false;
}

Vector2 Editor::simplify(float v)
{
    int len = control_points.size();    
    std::vector<Vector2> last = control_points;
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

void Editor::regenerate_curve()
{
    points->clear();
    internal_points.clear();

    if (control_points.size() < 2)
    {
        return;
    }
    
    for (float v = 0; v < 1.0; v+=step)
    {
        points->push_back(simplify(v));
    }

    for (float v = 0; v < 1.0; v += 0.001)
    {
        internal_points.push_back(simplify(v));
    }   
}

bool Editor::update(int button, int state, int x, int y, bool held)
{
    x = x - anchorX;
    y = y - anchorY;

    if (!held)
    {
        control_point_held = -1;
    }

    if (!interface_interaction(button, state, x, y))
    {
        x = x - width / 2.0;
        y = y - height / 2.0;
        switch (mode)
        {
            case 0:
                return move(button, state, x, y, held);

            case 1:
                return add(button, state, x, y);

            case 2:
                return kill(button, state, x, y);
            
            default:
                break;
        }
    }

    return false;
}