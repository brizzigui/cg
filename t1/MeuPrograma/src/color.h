#ifndef COLOR_H
#define COLOR_H

class Color
{
    private:
        void internal_rgb_to_hsv()
        {
            double r_prime = r/255.0;
            double g_prime = g/255.0;
            double b_prime = b/255.0;

            double c_max = std::max(r_prime, std::max(g_prime, b_prime));
            double c_min = std::min(r_prime, std::min(g_prime, b_prime));

            double delta = c_max - c_min;

            if (delta == 0)
            {
                h = 0;
            }
            else if (c_max == r_prime)
            {
                h = 60 * fmod(((g_prime - b_prime)/delta), 6);
            }
            else if (c_max == g_prime)
            {
                h = 60 * (((b_prime - r_prime)/delta) + 2);
            }
            else if (c_max == g_prime)
            {
                h = 60 * (((r_prime - g_prime)/delta) + 4);
            }
            
            if (c_max == 0)
            {
                s = 0;
            }
            else
            {
                s = delta / c_max;
            }

            v = c_max;   
        }

        void internal_hsv_to_rgb()
        {
            double c = v * s;
            double x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
            double m = v - c;

            if (h >= 0 && h < 60)
            {
                r = (c + m) * 255;
                g = (x + m) * 255;
                b = (0 + m) * 255;
            }
            else if (h >= 60 && h < 120)
            {
                r = (x + m) * 255;
                g = (c + m) * 255;
                b = (0 + m) * 255;
            }
            else if (h >= 120 && h < 180)
            {
                r = (0 + m) * 255;
                g = (c + m) * 255;
                b = (x + m) * 255;
            }
            else if (h >= 180 && h < 240)
            {
                r = (0 + m) * 255;
                g = (x + m) * 255;
                b = (c + m) * 255;
            }
            else if (h >= 240 && h < 300)
            {
                r = (x + m) * 255;
                g = (0 + m) * 255;
                b = (c + m) * 255;
            }
            else
            {
                r = (c + m) * 255;
                g = (0 + m) * 255;
                b = (x + m) * 255;
            }
        }

    public:
        int r, g, b;
        int h;
        double s, v;
        int a;

        Color()
        {
            r = g = b = h = s = v = a = 0;
        }

        void set_from_rgb(int r, int g, int b, int a)
        {
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = a;

            internal_rgb_to_hsv();
            //std::cout << "(" << r << ", " << g << ", " << b << ")" << std::endl; 
            //std::cout << "(" << h << ", " << s << ", " << v << ")" << std::endl; 
        }

        void set_from_hsv(int h, double s, double v, int a)
        {
            this->h = h;
            this->s = s;
            this->v = v;
            this->a = a;

            internal_hsv_to_rgb();
            //std::cout << "(" << r << ", " << g << ", " << b << ")" << std::endl; 
            //std::cout << "(" << h << ", " << s << ", " << v << ")" << std::endl; 
        }
};

#endif