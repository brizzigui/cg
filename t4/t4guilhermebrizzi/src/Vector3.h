#ifndef __VECTOR_3_H__
#define __VECTOR_3_H__

#include "stdio.h"
#include "math.h"

class Vector3
{
public:
   float x, y, z;

   Vector3()
   {
      x = y = z = 0;
   }

   Vector3(float _x, float _y, float _z)
   {
       x = _x;
       y = _y;
       z = _z;
   }

   void set(float _x, float _y, float _z)
   {
       x = _x;
       y = _y;
       z = _z;
   }

   void normalize()
   {
       float norm = (float)sqrt(x*x + y*y + z*z);

       if(norm==0.0)
       {
          printf("\n\nNormalize::Divisao por zero");
          x = 1;
          y = 1;
          z = 1;
          return;
       }
       x /= norm;
       y /= norm;
       z /= norm;
   }

   float angle(const Vector3 v)
   {
        float norm_a = (float)sqrt(x*x + y*y);
        float norm_b = (float)sqrt(v.x*v.x + v.y*v.y);

        return acos((this->x * v.x + this->y * v.y) / (norm_a*norm_b));
   }

   void rotate_x(float rad)
    { 
        float n_y = y*cos(rad) + z*sin(rad);
        float n_z = -y*sin(rad) + z*cos(rad);
        y = n_y;
        z = n_z;
    }

    void rotate_y(float rad)
    {
        float n_x = x*cos(rad) + z*sin(rad);
        float n_z = -x*sin(rad) + z*cos(rad);
        x = n_x;
        z = n_z;
    }

    void rotate_z(float rad)
    {
        float n_x = x*cos(rad) + y*sin(rad);
        float n_y = -x*sin(rad) + y*cos(rad);
        x = n_x;
        y = n_y;
    }

    float dot(Vector3 v)
    {
        return this->x * v.x + this->y * y + this->z * z;
    }

    Vector3 cross(Vector3 v)
    {
        return Vector3(
            this->y * v.z - this->z * v.y,
            this->z * v.x - this->x * v.z,
            this->x * v.y - this->y * v.x
        );
    }

    Vector3 operator - (const Vector3& v)
    {
        Vector3 aux( x - v.x, y - v.y, z - v.z);
        return( aux );
    }

    Vector3 operator + (const Vector3& v)
    {
        Vector3 aux( x + v.x, y + v.y, z + v.z);
        return( aux );
    }

    Vector3 operator * (const double v)
    {
        return Vector3(x * v, y * v, z * v);
    }

    Vector3 operator * (const int v)
    {
        return Vector3(x * v, y * v, z * v);
    }

    Vector3 operator * (const Vector3& v)
    {
        return Vector3(x*v.x, y*v.y, z * v.z);
    }

   //Adicionem os demais overloads de operadores aqui.


};

#endif
