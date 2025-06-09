#include "preview.h"
#include "canvas_pro.h"

Preview::Preview(std::vector<Vector2> *points, float screen_height, float screen_width)
{
    this->padding = 30;
    this->width = screen_width/2 - padding*2;
    this->height = screen_height - padding*2;
    this->result = new CVpro::image(width, height);
    this->anchorX = screen_width/2 + padding;
    this->anchorY = padding;

    this->R2_points = points;
}

Preview::~Preview()
{
}

Vector3 Preview::project(Vector3 point, float d)
{
    return Vector3(
        point.x * d / point.z, point.y * d / point.z, 0.0
    );
}

void Preview::draw()
{
    CV::translate(anchorX, anchorY);
    result->display();   
    CV::translate(0, 0);
}

std::vector<std::vector<Vector3>> Preview::rotate_bezier()
{
    int slices = 16;
    float step = 2*PI / slices;

    std::vector<Vector2> cur_R2_points = (*R2_points);
    std::vector<std::vector<Vector3>> tmp;

    for (int i = 0; i < R2_points->size(); i++)
    {
        tmp.push_back(std::vector<Vector3>());

        for (int j = 0; j < slices; j++)
        {
            Vector3 p = Vector3(cur_R2_points[i].x*scale, cur_R2_points[i].y, 0.0);
            float angle = step * j;
            p.rotate_y(angle);
            tmp[i].push_back(p);
        }
    }

    return tmp;
}

void Preview::set_pixel(int x, int y)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        int i = (y * result->width + x) * result->bytes;
        result->matrix[i + 0] = 255; 
        result->matrix[i + 1] = 0;   
        result->matrix[i + 2] = 0;   
        result->matrix[i + 3] = 255; 
    }
}

void Preview::line_to_bmp(int x0, int y0, int x1, int y1)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int steps = std::max(abs(dx), abs(dy));

    float x_inc = dx / (float)steps;
    float y_inc = dy / (float)steps;

    float x = x0 + width/2;
    float y = y0;

    for (int i = 0; i <= steps; i++) 
    {
        set_pixel(x, y);
        x += x_inc;
        y += y_inc;
    }
}


void Preview::paint_result()
{
    for (int p = 0; p < (int)R2_projections.size(); p++)
    {
        auto tri = R2_projections[p];
        line_to_bmp(tri.va.x, tri.va.y, tri.vb.x, tri.vb.y);
        line_to_bmp(tri.va.x, tri.va.y, tri.vc.x, tri.vc.y);
        line_to_bmp(tri.vb.x, tri.vb.y, tri.vc.x, tri.vc.y);
    }
}

void Preview::triangularize()
{
    std::vector<std::vector<Vector3>> vertices = rotate_bezier();

    if ((int)vertices.size() == 0)
    {
        return;
    }
    
    int slices = vertices[0].size();
    
    for (int i = 0; i < (int)vertices.size()-1; i++)
    {
        for (int j = 0; j < slices; j++)
        {
            int next_j = (j + 1) % slices;
            
            R3_triangles.push_back(Triangle(
                vertices[i][j], 
                vertices[i+1][j], 
                vertices[i][next_j]
            ));
            
            R3_triangles.push_back(Triangle(
                vertices[i+1][j], 
                vertices[i+1][next_j], 
                vertices[i][next_j]
            ));
        }
    }
}

void Preview::flatten()
{
    for (int t = 0; t < (int)R3_triangles.size(); t++)
    {
        Triangle cur = R3_triangles[t];
        cur.va.z += dist;
        cur.vb.z += dist;
        cur.vc.z += dist;

        R2_projections.push_back(Triangle(project(cur.va, dist), project(cur.vb, dist), project(cur.vc, dist)));
    }   
}

void Preview::recreate()
{   
    R2_projections.clear();
    R3_triangles.clear();
    result->clear();

    triangularize();
    flatten();
    paint_result();
}

void Preview::update()
{

}