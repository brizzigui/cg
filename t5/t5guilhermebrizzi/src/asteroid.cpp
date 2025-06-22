#include <GL/glut.h>
#include <iostream>

#include "asteroid.h"

int random_whole(int limit)
{
    int sign = (rand()%2) ? 1 : -1;
    return sign * rand()%limit;
}

int random_int(int limit)
{
    return rand()%limit;
}

Asteroid::Asteroid()
{
    this->color = Color(random_int(50) + 100, random_int(50) + 100, random_int(50) + 100);
    this->pos = Vector3(random_whole(repeat_param/2), random_whole(repeat_param/2), random_whole(repeat_param/2));
    this->size = random_int(15);
}

Asteroid::~Asteroid()
{
}

float calc_dist(Vector3 v1, Vector3 v2)
{
   return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
}

Vector3 Asteroid::find_real_position(Vector3 camera)
{
    Vector3 delta = camera - pos;
    float n_x = round(delta.x / repeat_param);
    float n_y = round(delta.y / repeat_param);
    float n_z = round(delta.z / repeat_param);

    Vector3 target = Vector3(pos.x + n_x * repeat_param,
                            pos.y + n_y * repeat_param,
                            pos.z + n_z * repeat_param);

    return target;
}

void Asteroid::draw(Vector3 camera)
{
    Vector3 target = find_real_position(camera);
    int definition = 100.0 / log2(calc_dist(target, camera));

    glPushMatrix();
        GLfloat rock_ambient[]  = {0.0f, 0.0f, 0.0f, 1.0f};
        GLfloat rock_diffuse[]  = {0.4f, 0.4f, 0.4f, 1.0f};
        GLfloat rock_specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
        GLfloat rock_shininess  = 30.0f;

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   rock_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   rock_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  rock_specular);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, rock_shininess);

        glColor3ub(color.r, color.g, color.b);
        glTranslated(target.x, target.y, target.z);
        glutSolidSphere(size, definition, definition);

    glPopMatrix();
}

void Asteroid::tick()
{

}