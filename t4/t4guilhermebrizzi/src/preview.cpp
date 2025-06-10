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

void Preview::draw_background()
{
    CVpro::color(30, 30, 30);
    CV::rectFill(0, 0, width, height);
    CVpro::color(255, 255, 255);
    CV::rect(0, 0, width, height);
}

void Preview::draw()
{
    CV::translate(anchorX, anchorY);
    draw_background();
    result->display();   
    CV::translate(0, 0);
}

std::vector<std::vector<Vector3>> Preview::rotate_bezier()
{
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

void Preview::triangle_to_wireframe()
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

Color Preview::compute_vertex_lighting(Vector3 pos, Vector3 normal, Vector3 light_dir)
{
    light_dir.normalize();
    float intensity = std::max(0.0f, normal.dot(light_dir));
    return Color((int)(paint_color.r * intensity), (int)(paint_color.g * intensity), (int)(paint_color.b * intensity), 255);
}

void Preview::triangle_to_vertex_lighting()
{
    Vector3 light_dir = Vector3(0, 0, -1);

    // Clear zbuffer
    zbuffer.assign(width * height, std::numeric_limits<float>::infinity());

    for (int t = 0; t < (int)R3_triangles.size(); t++)
    {
        Triangle cur = R3_triangles[t];

        Vector3 va = cur.va; va.z += dist;
        Vector3 vb = cur.vb; vb.z += dist;
        Vector3 vc = cur.vc; vc.z += dist;

        Vector3 edge1 = vb - va;
        Vector3 edge2 = vc - va;
        Vector3 normal = edge1.cross(edge2);
        normal.normalize();

        Color ca = compute_vertex_lighting(va, normal, light_dir);
        Color cb = compute_vertex_lighting(vb, normal, light_dir);
        Color cc = compute_vertex_lighting(vc, normal, light_dir);

        Vector3 pa = project(va, dist);
        Vector3 pb = project(vb, dist);
        Vector3 pc = project(vc, dist);

        // Bounding box
        int minX = std::min((int)pa.x, std::min((int)pb.x, (int)pc.x));
        int maxX = std::max((int)pa.x, std::max((int)pb.x, (int)pc.x));
        int minY = std::min((int)pa.y, std::min((int)pb.y, (int)pc.y));
        int maxY = std::max((int)pa.y, std::max((int)pb.y, (int)pc.y));

        minX = std::max(0, minX + (int)(width/2));
        maxX = std::min((int)width-1, maxX + (int)(width/2));
        minY = std::max(0, minY);
        maxY = std::min((int)height-1, maxY);

        float denom = ((pb.y - pc.y)*(pa.x - pc.x) + (pc.x - pb.x)*(pa.y - pc.y));
        if (fabs(denom) < 1e-5) continue;

        for (int y = minY; y <= maxY; y++)
        {
            for (int x = minX; x <= maxX; x++)
            {
                float fx = x - width/2;
                float fy = y;

                float w1 = ((pb.y - pc.y)*(fx - pc.x) + (pc.x - pb.x)*(fy - pc.y)) / denom;
                float w2 = ((pc.y - pa.y)*(fx - pc.x) + (pa.x - pc.x)*(fy - pc.y)) / denom;
                float w3 = 1.0f - w1 - w2;

                if (w1 >= 0 && w2 >= 0 && w3 >= 0)
                {
                    float z = va.z * w1 + vb.z * w2 + vc.z * w3;
                    int idx = y * width + x;
                    if (z < zbuffer[idx])
                    {
                        zbuffer[idx] = z;
                        // Interpolate color and clamp
                        int r = std::max(0, std::min(255, (int)(ca.r * w1 + cb.r * w2 + cc.r * w3)));
                        int g = std::max(0, std::min(255, (int)(ca.g * w1 + cb.g * w2 + cc.g * w3)));
                        int b = std::max(0, std::min(255, (int)(ca.b * w1 + cb.b * w2 + cc.b * w3)));
                        int a = std::max(0, std::min(255, (int)(ca.a * w1 + cb.a * w2 + cc.a * w3)));
                        int i = (y * result->width + x) * result->bytes;
                        result->matrix[i + 2] = (unsigned char)r;
                        result->matrix[i + 1] = (unsigned char)g;
                        result->matrix[i    ] = (unsigned char)b;
                        result->matrix[i + 3] = (unsigned char)a;
                    }
                }
            }
        }
    }
}

void Preview::triangle_to_pixel_lighting()
{
    Vector3 light_dir = Vector3(0, 0, -1);

    // Clear zbuffer
    zbuffer.assign(width * height, std::numeric_limits<float>::infinity());

    for (int t = 0; t < (int)R3_triangles.size(); t++)
    {
        Triangle cur = R3_triangles[t];

        Vector3 va = cur.va; va.z += dist;
        Vector3 vb = cur.vb; vb.z += dist;
        Vector3 vc = cur.vc; vc.z += dist;

        Vector3 edge1 = vb - va;
        Vector3 edge2 = vc - va;
        Vector3 face_normal = edge1.cross(edge2);
        face_normal.normalize();

        // For demonstration, use face normal as vertex normal (replace with real vertex normals if available)
        Vector3 na = face_normal;
        Vector3 nb = face_normal;
        Vector3 nc = face_normal;

        Vector3 pa = project(va, dist);
        Vector3 pb = project(vb, dist);
        Vector3 pc = project(vc, dist);

        int minX = std::min((int)pa.x, std::min((int)pb.x, (int)pc.x));
        int maxX = std::max((int)pa.x, std::max((int)pb.x, (int)pc.x));
        int minY = std::min((int)pa.y, std::min((int)pb.y, (int)pc.y));
        int maxY = std::max((int)pa.y, std::max((int)pb.y, (int)pc.y));

        minX = std::max(0, minX + (int)(width/2));
        maxX = std::min((int)width-1, maxX + (int)(width/2));
        minY = std::max(0, minY);
        maxY = std::min((int)height-1, maxY);

        float denom = ((pb.y - pc.y)*(pa.x - pc.x) + (pc.x - pb.x)*(pa.y - pc.y));
        if (fabs(denom) < 1e-5) continue;

        for (int y = minY; y <= maxY; y++)
        {
            for (int x = minX; x <= maxX; x++)
            {
                float fx = x - width/2;
                float fy = y;

                float w1 = ((pb.y - pc.y)*(fx - pc.x) + (pc.x - pb.x)*(fy - pc.y)) / denom;
                float w2 = ((pc.y - pa.y)*(fx - pc.x) + (pa.x - pc.x)*(fy - pc.y)) / denom;
                float w3 = 1.0f - w1 - w2;

                if (w1 >= 0 && w2 >= 0 && w3 >= 0)
                {
                    float z = va.z * w1 + vb.z * w2 + vc.z * w3;
                    int idx = y * width + x;
                    if (z < zbuffer[idx])
                    {
                        zbuffer[idx] = z;
                        Vector3 pos = va * w1 + vb * w2 + vc * w3;

                        // Interpolate normal per pixel (Phong shading)
                        Vector3 normal = na * w1 + nb * w2 + nc * w3;
                        normal.normalize();

                        Color c = compute_vertex_lighting(pos, normal, light_dir);
                        int r = std::max(0, std::min(255, c.r));
                        int g = std::max(0, std::min(255, c.g));
                        int b = std::max(0, std::min(255, c.b));
                        int a = std::max(0, std::min(255, c.a));
                        int i = (y * result->width + x) * result->bytes;
                        result->matrix[i + 2] = (unsigned char)r;
                        result->matrix[i + 1] = (unsigned char)g;
                        result->matrix[i    ] = (unsigned char)b;
                        result->matrix[i + 3] = (unsigned char)a;
                    }
                }
            }
        }
    }
}

void Preview::flatten()
{
    switch (mode)
    {
        case WIREFRAME_MODE:
            triangle_to_wireframe();
            break;

        case VERTEX_LIGHTING_MODE:
            triangle_to_vertex_lighting();
            break;

        case PIXEL_LIGHTING_MODE:
            triangle_to_pixel_lighting();
            break;
        
        default:
            break;
    }
}

void Preview::recreate()
{   
    R2_projections.clear();
    R3_triangles.clear();
    result->clear();

    slices = (mode == 0) ? 16 : 64;

    triangularize();
    flatten();
    paint_result();
}

void Preview::update()
{
    mode = (mode + 1)%3;
    recreate();
}