#ifndef CAMERA_H
#define CAMERA_H

#include "Vector3.h"

// Class definition for the camera
class Camera
{
    public:
        Vector3 pos;
        Vector3 direction;
        Vector3 up;
        Vector3 input;
        float speed;

        Camera();
        ~Camera();

        void tick(float factor);
        void key_down(unsigned char key);
        void key_up(unsigned char key);
};

#endif