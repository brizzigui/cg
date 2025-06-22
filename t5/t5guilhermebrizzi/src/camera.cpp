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
    // calculates first person rotation
    // so that rotation looks as expected
    float yaw = input.y * factor * (PI/128.0f);
    float pitch = input.x * factor * (PI/128.0f);

    Vector3 right = up.cross(direction);
    right.normalize();

    if (yaw != 0.0f)
    {
        float cosYaw = cos(yaw);
        float sinYaw = sin(yaw);
        Vector3 d = direction;
        direction = (d * cosYaw) + (right * sinYaw);
        direction.normalize();
    }

    right = up.cross(direction);
    right.normalize();

    if (pitch != 0.0f)
    {
        float cosPitch = cos(pitch);
        float sinPitch = sin(pitch);
        Vector3 d = direction;
        direction = (d * cosPitch) + (up * sinPitch);
        direction.normalize();
    }

    right = up.cross(direction);
    right.normalize();
    up = direction.cross(right);
    up.normalize();

    // moves camera according to direction
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
            input.x = -1;
            break;

        case 'S': case 's':
            input.x = 1;
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

        default:
            break;
    }
}