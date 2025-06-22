#include "camera.h"
#include "math.h"
#define PI       3.14159265358979323846


Camera::Camera()
{
    this->pos = Vector3(0, 0, 0);
    this->direction = Vector3(0, 0, 1);
    this->up = Vector3(0, 1, 0);
    this->speed = 5;
}

Camera::~Camera()
{
}

void Camera::tick(float factor)
{
    direction.rotate_x(PI/256.0 * input.x * factor);
    direction.rotate_y(PI/256.0 * input.y * factor);
    direction.rotate_z(PI/256.0 * input.z * factor);

    up.rotate_x(PI/256.0 * input.x * factor);
    up.rotate_y(PI/256.0 * input.y * factor);
    up.rotate_z(PI/256.0 * input.z * factor);

    pos = pos + (direction * speed * factor);
}

void Camera::key_down(unsigned char key)
{
    switch (key)
    {
        case 'A': case 'a':
            input.y = 1;
            break;

        case 'D': case 'd':
            input.y = -1;
            break;

        case 'W': case 'w':
            input.x = 1;
            break;

        case 'S': case 's':
            input.x = -1;
            break;

        case 'Q': case 'q':
            input.z = 1;
            break;

        case 'E': case 'e':
            input.z = -1;
            break;

        default:
            break;
    }
}


void Camera::key_up(unsigned char key)
{
    switch (key)
    {
        case 'A': case 'a':
            input.y = 0;
            break;

        case 'D': case 'd':
            input.y = 0;
            break;

        case 'W': case 'w':
            input.x = 0;
            break;

        case 'S': case 's':
            input.x = 0;
            break;

        case 'Q': case 'q':
            input.z = 0;
            break;

        case 'E': case 'e':
            input.z = 0;
            break;

        default:
            break;
    }
}