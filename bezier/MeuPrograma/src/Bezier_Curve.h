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
                    cur.push_back(std::make_pair(last[i].first + fabs(last[i].first - last[i+1].first)*v, last[i].second + fabs(last[i].second - last[i+1].second)*v));
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
                CV::color(1.0, 0.0, 0.0);
                CV::circleFill(current.first, current.second, 2, 10);
            }
        }
        
};
      
#endif
