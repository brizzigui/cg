#ifndef __PACMAN_H__
#define __PACMAN_H__

#include "gl_canvas2d.h"
#include "food.h"

#include <iostream>

class Pacman{
  int radius = 15;

  float x, y;
  float speed;

  int mouth_angle = 30;
  bool growing_mouth = true;


  int *scr_width;
  int *scr_height;

  enum Directions {
    UP,
    RIGHT,
    DOWN,
    LEFT
  }; 

  enum Directions cur_direction = UP;

  void update_mouth_angle()
  {
    if (growing_mouth)
    {
      mouth_angle += 2;
      if (mouth_angle > 90)
      {
        growing_mouth = false;
      }
      
    }
    else
    {
      mouth_angle -= 2;
      if (mouth_angle < 30)
      {
        growing_mouth = true;
      }
    }
    
    
  }

  void draw_mouth()
  {
    update_mouth_angle();

    double angle_increment = 0;
    switch (cur_direction)
    {
      case UP:
        angle_increment = 90;
        break;

      case RIGHT:
        angle_increment = 0;
        break;

      case DOWN:
        angle_increment = 270;
        break;

      case LEFT:
        angle_increment = 180;
        break;
    }

    for (int angle = 0; angle < 360; angle++)
    {
      if (!(angle > (360 - mouth_angle/2) || angle < (mouth_angle/2)))
      {
        double used_angle = angle + angle_increment;
        CV::line(x, y, x+cos(PI*used_angle/180.0)*radius, y+sin(PI*used_angle/180.0)*radius);
      }
    }
  }
  
  public:
    Pacman(float x, float y, float speed, int *scr_width, int *scr_height)
    {
      this->x = x-radius/2;
      this->y = y-radius/2;
      this->speed = speed;
      this->scr_width = scr_width;
      this->scr_height = scr_height;
    }
    
    void animate()
    {
      switch (cur_direction)
      {
        case UP:
          y += speed;
          break;

        case RIGHT:
          x += speed;
          break;

        case DOWN:
          y -= speed;
          break;

        case LEFT:
          x -= speed;
          break;
      }

      if (x-radius > *scr_width)
      {
        x = -radius;
      }

      else if (x+radius < 0)
      {
        x = *scr_width+radius;
      }

      if (y-radius > *scr_height)
      {
        y = -radius;
      }

      else if (y+radius < 0)
      {
        y = *scr_height+radius;
      }

      CV::color(255/255.0, 200/255.0, 0);
      draw_mouth();
    }

    void change_dir(int key)
    {
      switch (key)
      {
        case 201:
          cur_direction = UP;
          break;

        case 202:
          cur_direction = RIGHT;
          break;

        case 203:
          cur_direction = DOWN;
          break;

        case 200:
          cur_direction = LEFT;
          break;
        
        default:
          break;
      }
    }

    std::pair<bool, bool> eat(Food *food)
    {
      for (int i = 0; i < food->x.size(); i++)
      {
        if(fabs(x-food->x[i]) < radius && fabs(y-food->y[i]) < radius)
        {
          if (food->is_evil[i])
          {
            food->update_food(i);
            return std::make_pair(false, true);
          }

          else
          {
            food->update_food(i);
            return std::make_pair(true, false);
          }
        
        }
      }

      return std::make_pair(false, false);
    }

    void restart()
    {
      x = *scr_width/2-radius;
      y = *scr_height/2-radius;
    }
};
      
#endif
