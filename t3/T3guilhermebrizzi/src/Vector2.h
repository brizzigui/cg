#ifndef __VECTOR_2_H__
#define __VECTOR_2_H__

#include <iostream>
#include <cmath>

#define PI   3.14159265359

class Vector2
{
public:
   float x, y;

   Vector2()
   {
      x = y = 0;
   }

   Vector2(float _x, float _y)
   {
       x = _x;
       y = _y;
   }

   // Creates and returns a unit vector with said angle
   Vector2(float angle)
   {
        this->x = cos(angle);
        this->y = sin(angle);
   }

   // Adds an angle to the current vector
   void rotate_by(float angle)
   {
        float nx = x * cos(angle) - y * sin(angle);
        float ny = x * sin(angle) + y * cos(angle);

        x = nx;
        y = ny;
   }

   // Returns the angle of the vector. Will only work correctly if the vector is normalized
   float get_angle()
   {
        return atan2f(y, x);
   }

   void set(float _x, float _y)
   {
       x = _x;
       y = _y;
   }

   void normalize()
   {
       float norm = (float)sqrt(x*x + y*y);

       if(norm == 0.0)
       {
          std::cout << "void Vector2::normalize()\n\tZero division error." << std::endl;
          x = 0.0;
          y = 1.0;
          return;
       }
       x /= norm;
       y /= norm;
   }

   Vector2 operator - (const Vector2& v)
   {
        Vector2 aux( x - v.x, y - v.y);
        return( aux );
   }

   Vector2 operator + (const Vector2& v)
   {
       Vector2 aux( x + v.x, y + v.y);
       return( aux );
   }

   //Adicionem os demais overloads de operadores aqui.


};

#endif
