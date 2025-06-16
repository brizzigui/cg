#include <GL/glut.h>

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
    this->pos = Vector3(random_whole(10000), random_whole(10000), random_whole(10000));
    this->size = random_int(100);
}

Asteroid::Asteroid(Color color, Vector3 pos, float size)
{
    this->color = color;
    this->pos = pos;
    this->size = size;
}

Asteroid::Asteroid(Color color, Vector3 pos, float size, Vector3 dir, float speed)
{
    this->color = color;
    this->pos = pos;
    this->size = size;
    this->dir = dir;
    this->speed = speed;
}

Asteroid::~Asteroid()
{
}

void Asteroid::draw()
{
    glPushMatrix();
        GLfloat rock_ambient[]  = {0.1f, 0.1f, 0.1f, 1.0f};
        GLfloat rock_diffuse[]  = {0.4f, 0.4f, 0.4f, 1.0f};
        GLfloat rock_specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
        GLfloat rock_shininess  = 20.0f;

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   rock_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   rock_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  rock_specular);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, rock_shininess);

        glColor3ub(color.r, color.g, color.b);
        glTranslated(pos.x, pos.y, pos.z);
        glutSolidSphere(size, 50, 50);

    glPopMatrix();
}
