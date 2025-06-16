/**
 * @file Vector2.h
 * @brief 2D vector class for geometric and mathematical operations.
 *
 * Provides a Vector2 class with basic vector arithmetic, normalization, rotation, and angle calculation for 2D graphics and geometry.
 */
#ifndef __VECTOR_2_H__
#define __VECTOR_2_H__

#include <iostream>
#include <cmath>

#define PI   3.14159265359

/**
 * @brief Represents a 2D vector with float components and common vector operations.
 *
 * Supports construction from coordinates or angle, rotation, normalization, and arithmetic operators.
 */
class Vector2
{
public:
   float x, y; /**< X and Y components of the vector. */

   /**
    * @brief Default constructor. Initializes x and y to zero.
    */
   Vector2()
   {
      x = y = 0;
   }

   /**
    * @brief Constructs a vector from x and y coordinates.
    * @param _x X component.
    * @param _y Y component.
    */
   Vector2(float _x, float _y)
   {
       x = _x;
       y = _y;
   }

   /**
    * @brief Constructs a unit vector from an angle (in radians).
    * @param angle Angle in radians.
    */
   Vector2(float angle)
   {
        this->x = cos(angle);
        this->y = sin(angle);
   }

   /**
    * @brief Rotates the vector by a given angle (in radians).
    * @param angle Angle in radians to rotate by.
    */
   void rotate_by(float angle)
   {
        float nx = x * cos(angle) - y * sin(angle);
        float ny = x * sin(angle) + y * cos(angle);

        x = nx;
        y = ny;
   }

   /**
    * @brief Returns the angle of the vector (in radians).
    * @return Angle in radians.
    */
   float get_angle()
   {
        return atan2f(y, x);
   }

   /**
    * @brief Sets the vector components to the given values.
    * @param _x New X component.
    * @param _y New Y component.
    */
   void set(float _x, float _y)
   {
       x = _x;
       y = _y;
   }

   /**
    * @brief Normalizes the vector to unit length.
    */
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

   /**
    * @brief Subtracts another vector from this vector.
    * @param v The vector to subtract.
    * @return The result of the subtraction.
    */
   Vector2 operator - (const Vector2& v)
   {
        Vector2 aux( x - v.x, y - v.y);
        return( aux );
   }

   /**
    * @brief Adds another vector to this vector.
    * @param v The vector to add.
    * @return The result of the addition.
    */
   Vector2 operator + (const Vector2& v)
   {
       Vector2 aux( x + v.x, y + v.y);
       return( aux );
   }

   //Adicionem os demais overloads de operadores aqui.
};

#endif
