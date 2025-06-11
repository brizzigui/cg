#include "preview.h"
#include "canvas_pro.h"
#include "clamp.h"

Preview::Preview(std::vector<Vector2> *points, float screen_height, float screen_width)
{
    this->padding = 30;
    this->width = screen_width/2 - padding*2;
    this->height = screen_height - padding*2;
    this->result = new CVpro::image(width, height);
    this->anchorX = screen_width/2 + padding;
    this->anchorY = padding;

    this->button_size = 40;
    this->button_anchorX = width - button_size - button_size/3;
    this->button_anchorY = button_size/3;

    this->R2_points = points;

    icons.push_back(CVpro::load_bitmap("./t4guilhermebrizzi/assets/wireframe.bmp"));
    icons.push_back(CVpro::load_bitmap("./t4guilhermebrizzi/assets/fill.bmp"));
    icons.push_back(CVpro::load_bitmap("./t4guilhermebrizzi/assets/increase_faces.bmp"));
    icons.push_back(CVpro::load_bitmap("./t4guilhermebrizzi/assets/decrease_faces.bmp"));
    icons.push_back(CVpro::load_bitmap("./t4guilhermebrizzi/assets/grow_y.bmp"));
    icons.push_back(CVpro::load_bitmap("./t4guilhermebrizzi/assets/perspective.bmp"));
    icons.push_back(CVpro::load_bitmap("./t4guilhermebrizzi/assets/normal.bmp"));
}

Preview::~Preview()
{
}

Vector3 Preview::project(Vector3 point, float d)
{
    if (perspective)
    {
        return Vector3(
            point.x * d / point.z, point.y * d / point.z, 0.0
        );
    }

    else
    {
        return Vector3(
            point.x, point.y, 0.0
        );
    }
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
    draw_buttons();
    draw_info();
    CV::translate(0, 0);
}

std::vector<std::vector<Vector3>> Preview::rotate_bezier()
{
    float step = 2*PI / slices;

    float y_growth = 0;
    if (grow_y)
    {
        y_growth = (height/2.0)/slices;
    }

    std::vector<Vector2> cur_R2_points = (*R2_points);
    std::vector<std::vector<Vector3>> tmp;

    for (int i = 0; i < R2_points->size(); i++)
    {
        tmp.push_back(std::vector<Vector3>());

        for (int j = 0; j < slices; j++)
        {
            Vector3 p = Vector3(cur_R2_points[i].x*scale, cur_R2_points[i].y + y_growth * j, 0.0);
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
        result->matrix[i + 2] = 241; 
        result->matrix[i + 1] = 120;   
        result->matrix[i + 0] = 38;   
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
    float y = y0 + height/2;

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
        // -grow_y avoids the joining of non adjacent points
        // when growing y

        for (int j = 0; j < slices - (int)grow_y; j++)
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
        cur.va.z += object_pos;
        cur.vb.z += object_pos;
        cur.vc.z += object_pos;

        R2_projections.push_back(Triangle(project(cur.va, dist), project(cur.vb, dist), project(cur.vc, dist)));
    }   
}

Color Preview::compute_vertex_lighting(Vector3 pos, Vector3 normal, Vector3 light_dir)
{
    light_dir.normalize();
    float intensity = std::max(0.0f, normal.dot(light_dir));
    return Color((int)(paint_color.r * intensity), (int)(paint_color.g * intensity), (int)(paint_color.b * intensity), 255);
}

void Preview::triangle_to_pixel_lighting()
{
    Vector3 light_pos = Vector3(0, 0, -1000);

    // Clear zbuffer
    zbuffer.assign(width * height, std::numeric_limits<float>::infinity());

    for (int t = 0; t < (int)R3_triangles.size(); t++)
    {
        Triangle cur = R3_triangles[t];

        Vector3 va = cur.va; va.z += object_pos;
        Vector3 vb = cur.vb; vb.z += object_pos;
        Vector3 vc = cur.vc; vc.z += object_pos;

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
        minY = std::max(0, minY + (int)(height/2));
        maxY = std::min((int)height-1, maxY + (int)(height/2));

        float denom = ((pb.y - pc.y)*(pa.x - pc.x) + (pc.x - pb.x)*(pa.y - pc.y));
        if (fabs(denom) < 1e-5) continue;

        for (int y = minY; y <= maxY; y++)
        {
            for (int x = minX; x <= maxX; x++)
            {
                float fx = x - width/2;
                float fy = y - height/2;

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
                        Vector3 light_dir = light_pos - pos;

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

void Preview::rotate(float roll, float pitch, float yaw)
{
    for (int i = 0; i < (int)R3_triangles.size(); i++)
    {
        R3_triangles[i].va.rotate_x(roll);
        R3_triangles[i].va.rotate_y(pitch);
        R3_triangles[i].va.rotate_z(yaw);

        R3_triangles[i].vb.rotate_x(roll);
        R3_triangles[i].vb.rotate_y(pitch);
        R3_triangles[i].vb.rotate_z(yaw);

        R3_triangles[i].vc.rotate_x(roll);
        R3_triangles[i].vc.rotate_y(pitch);
        R3_triangles[i].vc.rotate_z(yaw);
    }   
}

void Preview::calculate_normals()
{
    for (int i = 0; i < (int)R3_triangles.size(); i++)
    {
        Vector3 middle = (R3_triangles[i].va + R3_triangles[i].vb + R3_triangles[i].vc) * (1/3.0);
        middle.z += object_pos;
        Vector3 edge1 = R3_triangles[i].vb - R3_triangles[i].va;
        Vector3 edge2 = R3_triangles[i].vc - R3_triangles[i].va;
        Vector3 face_normal = edge1.cross(edge2);
        face_normal.normalize();

        Vector3 p0 = project(middle, dist);
        Vector3 p1 = project(middle - face_normal*15, dist);
        line_to_bmp(p0.x, p0.y, p1.x, p1.y);
    }
}

void Preview::flatten()
{
    switch (mode)
    {
        case WIREFRAME_MODE:
            triangle_to_wireframe();
            paint_result();
            break;

        case PIXEL_LIGHTING_MODE:
            triangle_to_pixel_lighting();
            break;
        
        default:
            break;
    }

    if (show_normal)
    {
        calculate_normals();
    }
    
}

void Preview::recreate()
{   
    R2_projections.clear();
    R3_triangles.clear();
    result->clear();

    triangularize();
    flatten();
}

void Preview::recreate(float roll, float pitch, float yaw)
{   
    R2_projections.clear();
    result->clear();

    rotate(roll, pitch, yaw);
    flatten();
}

Vector3 Preview::obtain_rotation(int x, int y)
{
    float dx = x - grabX;
    float dy = y - grabY;

    float sensitivity = 0.01f;
    return Vector3(dy * sensitivity, dx * sensitivity, 0.0f);
}

void Preview::draw_buttons()
{
    for (int i = 0; i < (int)icons.size(); i++)
    {
        icons[i]->display(button_anchorX, button_anchorY + 1.33*i*button_size);

        if (i == mode)
        {
            CVpro::color(255, 255, 255);
            CV::rect(button_anchorX, button_anchorY + 1.33*i*button_size,
                            button_anchorX + button_size, button_anchorY + 1.33*i*button_size + button_size);    
        }

        if ((i == 4 && grow_y) || (i == 5 && perspective) || (i == 6 && show_normal))
        {
            CVpro::color(241, 120, 38);
            CV::rect(button_anchorX, button_anchorY + 1.33*i*button_size,
                button_anchorX + button_size, button_anchorY + 1.33*i*button_size + button_size); 
        }
    }
}

void Preview::draw_info()
{
    CVpro::color(255, 255, 255);
    CVpro::text(15, height - 15, "Slices: %d", slices);
}

int Preview::check_buttons(int button, int state, int x, int y)
{
    for (int i = 0; i < (int)icons.size(); i++)
    {
        if (button == 0 && state == 0 &&
            x > button_anchorX && y > button_anchorY + 1.33*i*button_size &&
            x < button_anchorX + button_size && y < button_anchorY + 1.33*i*button_size + button_size)
        {
            return i;   
        }
    }

    return -1;
}

void Preview::handle_model_manipulation(int button, int state, int direction, int x, int y)
{
    if (button == 0 && state == 0) 
    {
        held = true;
        grabX = x;
        grabY = y;
        return;
    }

    if (button == 0 && state == 1) 
    {
        held = false;
        return;
    }

    if (direction == -1)
    {
        dist = (dist - 200 > 0) ? dist - 200 : dist;
        recreate(0.0, 0.0, 0.0);
    }

    else if (direction == 1)
    {
        dist = dist + 200;
        recreate(0.0, 0.0, 0.0);
    }

    if (held) 
    {
        Vector3 rotation_vector = obtain_rotation(x, y);
        recreate(-rotation_vector.x, -rotation_vector.y, rotation_vector.z);
        grabX = x;
        grabY = y;
    }
}

void Preview::handle_ui_input(int button, int state, int x, int y)
{
    int clicked = check_buttons(button, state, x, y);
    switch (clicked)
    {
        case 0:
            mode = WIREFRAME_MODE;
            recreate(0, 0, 0);
            break;

        case 1:
            mode = PIXEL_LIGHTING_MODE;
            recreate(0, 0, 0);
            break;

        case 2:
            slices *= 2;
            slices = (slices > 128) ? 128 : slices;
            recreate();
            break;

        case 3:
            slices /= 2;
            slices = (slices < 4) ? 4 : slices;
            recreate();
            break;

        case 4:
            grow_y = !grow_y;
            recreate();
            break;

        case 5:
            perspective = !perspective;
            recreate();
            break;

        case 6:
            show_normal = !show_normal;
            recreate(0.0, 0.0, 0.0);
            break;
    
    default:
        break;
    }
}

void Preview::update(int button, int state, int direction, int x, int y)
{
    x = x - anchorX;
    y = y - anchorY;

    if(x < 0 || y < 0 || x > width || y > height)
    {
        return;
    }

    handle_ui_input(button, state, x, y);
    handle_model_manipulation(button, state, direction, x, y);
}