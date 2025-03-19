#ifndef __PACMAN_H__
#define __PACMAN_H__

#include "gl_canvas2d.h"

#include <vector>
#include <iostream>

typedef std::vector<std::pair<double, double>> pts_list; 

class Bezier_Curve{ 
    pts_list *points;
    
    public:
        Bezier_Curve(pts_list *points)
        {
            this->points = points;
        }

        std::pair<double, double> simplify(double v)
        {
            int len = (*points).size();
            double result = 0.0;

            pts_list last = *points;
            pts_list cur;

            while (len > 1)
            {
                for (int i = 0; i < last.size()-1; i++)
                {
                    cur.push_back(std::make_pair((1-v) * last[i].first + v * last[i+1].first, (1-v) * last[i].second + v * last[i+1].second));
                }
                
                len = cur.size();
                last = cur;
                cur.clear();
            }

            return cur[0];
        }

        void draw()
        {
            for (double v = 0; v < 1.0; v += 0.001)
            {
                std::pair<double, double> current = simplify(v);
                CV::color(0.5215, 0.3333, 0.8509);
                CV::circleFill(current.first, current.second, 1, 10);
            }

            for (int i = 0; i < (*points).size(); i++)
            {
                CV::color(0.8, 0.8, 0.8);
                char text[8];
                sprintf(text, "P%d", i+1);
                CV::text((*points)[i].first + 10, (*points)[i].second + 10, text);
            }
            
        }
        
};
      
#endif
