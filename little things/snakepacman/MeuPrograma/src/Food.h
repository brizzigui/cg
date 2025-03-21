#ifndef __FOOD_H__
#define __FOOD_H__

#include "gl_canvas2d.h"

#include <iostream>
#include <vector>

#define AMOUNT_OF_FOOD 25

class Food{
    int *scr_width;
    int *scr_height;
  
    public:
        std::vector<int> x = std::vector<int>();
        std::vector<int> y = std::vector<int>();
        std::vector<bool> is_evil = std::vector<bool>();


        Food(int *scr_width, int *scr_height)
        {
            this->scr_width = scr_width;
            this->scr_height = scr_height;
        }

        void generate_food()
        {
            for (int i = 0; i < AMOUNT_OF_FOOD; i++)
            {
                x.push_back(10 + rand() % (*scr_width - 20));
                y.push_back(10 + rand() % (*scr_height - 20));
                is_evil.push_back((bool)(rand() % 2));
            }
        }

        bool get_evilness()
        {
            int evil = 0, good = 0;

            for (auto e : is_evil)
            {
                if (e)
                {
                    evil++;
                }
                else
                {
                    good++;
                }
            }

            return (good < 1) ? false : (bool)(rand() % 2);
        }

        void update_food(int pos)
        {
            // removes 
            x.erase(x.begin()+pos);
            y.erase(y.begin()+pos);
            is_evil.erase(is_evil.begin()+pos);

            // adds new
            x.push_back(10 + rand() % (*scr_width - 20));
            y.push_back(10 + rand() % (*scr_height - 20));
            is_evil.push_back(get_evilness());
        }

        void draw_food()
        {
            for (int i = 0; i < x.size(); i++)
            {
                if(is_evil[i])
                {
                    CV::color(1, 0.1, 0.1);
                    CV::circleFill(x[i], y[i], 5.0, 15);
                }

                else
                {
                    CV::color(1, 1, 1);
                    CV::circleFill(x[i], y[i], 5.0, 15);
                }
            }
        }

        void restart()
        {
            x.clear();
            y.clear();
            is_evil.clear();

            generate_food();
        }
};
      
#endif
