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
                s = 0;
            } 
            
            else 
            {
                s = delta / c_max;
        
                if (r_prime == c_max) 
                {
                    h = (g_prime - b_prime) / delta;
                }
                
                else if (g_prime == c_max)
                {
                    h = 2.0f + (b_prime - r_prime) / delta;
                } 
                
                else 
                {
                    h = 4.0f + (r_prime - g_prime) / delta;
                }
        
                h *= 60.0f;
                if (h < 0) 
                {
                    h += 360.0f;
                }
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
        double h, s, v;
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

        void set_from_hsv(double h, double s, double v, int a)
        {
            this->h = h;
            this->s = s;
            this->v = v;
            this->a = a;

            internal_hsv_to_rgb();
            //std::cout << "(" << r << ", " << g << ", " << b << ")" << std::endl; 
            //std::cout << "(" << h << ", " << s << ", " << v << ")" << std::endl; 
        }

        bool operator==(const Color& other) const {
            return this->r == other.r &&
                    this->g == other.g &&
                    this->b == other.b &&
                    this->a == other.a;
        }
        
};

typedef struct pixel
{
    int r, g, b, a;
} pixel;

pixel alpha_blend_pixel(float r0, float g0, float b0, float a0,
                        float r1, float g1, float b1, float a1)
{
    r0 /= 255, g0 /= 255, b0 /= 255, a0 /= 255;
    r1 /= 255, g1 /= 255, b1 /= 255, a1 /= 255;

    float r_out = r1 * a1 + r0 * (1 - a1);
    float g_out = g1 * a1 + g0 * (1 - a1);
    float b_out = b1 * a1 + b0 * (1 - a1);
    float a_out = a1 + a0 * (1 - a1);

    pixel p;
    p.r = r_out * 255;
    p.g = g_out * 255;
    p.b = b_out * 255;
    p.a = a_out * 255;

    return p;
}

#endif