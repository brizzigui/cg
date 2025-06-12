/**
 * @file Vector3.h
 * @brief 3D vector class for geometric and mathematical operations.
 *
 * Provides a Vector3 class with basic vector arithmetic, normalization, rotation, dot/cross products, and angle calculation for 3D graphics and geometry.
 */
#ifndef __VECTOR_3_H__
#define __VECTOR_3_H__

#include "stdio.h"
#include "math.h"

/**
 * @brief Represents a 3D vector with float components and common vector operations.
 *
 * Supports construction, normalization, rotation, dot/cross products, and arithmetic operators.
 */
class Vector3
{
public:
   float x, y, z; /**< X, Y, and Z components of the vector. */

   /**
    * @brief Default constructor. Initializes x, y, and z to zero.
    */
   Vector3()
   {
      x = y = z = 0;
   }

   /**
    * @brief Constructs a vector from x, y, and z coordinates.
    * @param _x X component.
    * @param _y Y component.
    * @param _z Z component.
    */
   Vector3(float _x, float _y, float _z)
   {
       x = _x;
       y = _y;
       z = _z;
   }

   /**
    * @brief Sets the vector components to the given values.
    * @param _x New X component.
    * @param _y New Y component.
    * @param _z New Z component.
    */
   void set(float _x, float _y, float _z)
   {
       x = _x;
       y = _y;
       z = _z;
   }

   /**
    * @brief Normalizes the vector to unit length.
    */
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

   /**
    * @brief Returns the angle between this vector and another (in radians).
    * @param v The other vector.
    * @return Angle in radians.
    */
   float angle(const Vector3 v)
   {
        float norm_a = (float)sqrt(x*x + y*y);
        float norm_b = (float)sqrt(v.x*v.x + v.y*v.y);

        return acos((this->x * v.x + this->y * v.y) / (norm_a*norm_b));
   }

   /**
    * @brief Rotates the vector around the X axis by a given angle (in radians).
    * @param rad Angle in radians.
    */
   void rotate_x(float rad)
    { 
        float n_y = y*cos(rad) + z*sin(rad);
        float n_z = -y*sin(rad) + z*cos(rad);
        y = n_y;
        z = n_z;
    }

    /**
    * @brief Rotates the vector around the Y axis by a given angle (in radians).
    * @param rad Angle in radians.
    */
    void rotate_y(float rad)
    {
        float n_x = x*cos(rad) + z*sin(rad);
        float n_z = -x*sin(rad) + z*cos(rad);
        x = n_x;
        z = n_z;
    }

    /**
    * @brief Rotates the vector around the Z axis by a given angle (in radians).
    * @param rad Angle in radians.
    */
    void rotate_z(float rad)
    {
        float n_x = x*cos(rad) + y*sin(rad);
        float n_y = -x*sin(rad) + y*cos(rad);
        x = n_x;
        y = n_y;
    }

    /**
    * @brief Computes the dot product with another vector.
    * @param v The other vector.
    * @return The dot product.
    */
    float dot(Vector3 v)
    {
        return this->x * v.x + this->y * y + this->z * z;
    }

    /**
    * @brief Computes the cross product with another vector.
    * @param v The other vector.
    * @return The cross product as a new Vector3.
    */
    Vector3 cross(Vector3 v)
    {
        return Vector3(
            this->y * v.z - this->z * v.y,
            this->z * v.x - this->x * v.z,
            this->x * v.y - this->y * v.x
        );
    }

    /**
    * @brief Subtracts another vector from this vector.
    * @param v The vector to subtract.
    * @return The result of the subtraction.
    */
    Vector3 operator - (const Vector3& v)
    {
        Vector3 aux( x - v.x, y - v.y, z - v.z);
        return( aux );
    }

    /**
    * @brief Adds another vector to this vector.
    * @param v The vector to add.
    * @return The result of the addition.
    */
    Vector3 operator + (const Vector3& v)
    {
        Vector3 aux( x + v.x, y + v.y, z + v.z);
        return( aux );
    }

    /**
    * @brief Multiplies this vector by a scalar (double).
    * @param v The scalar value.
    * @return The scaled vector.
    */
    Vector3 operator * (const double v)
    {
        return Vector3(x * v, y * v, z * v);
    }

    /**
    * @brief Multiplies this vector by a scalar (int).
    * @param v The scalar value.
    * @return The scaled vector.
    */
    Vector3 operator * (const int v)
    {
        return Vector3(x * v, y * v, z * v);
    }

    /**
    * @brief Multiplies this vector component-wise by another vector.
    * @param v The other vector.
    * @return The result of the component-wise multiplication.
    */
    Vector3 operator * (const Vector3& v)
    {
        return Vector3(x*v.x, y*v.y, z * v.z);
    }

   //Adicionem os demais overloads de operadores aqui.


};

#endif
