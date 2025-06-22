#ifndef CONTROLLER_H
#include <chrono>

// Class definition for the fps controller
class Controller
{
    private:
        int target_fps;
        bool debug_show_fps;
        float frame_duration;
        
        std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
        
        int debug_frame_counter = 0;
        int debug_fps = 0;
        std::chrono::time_point<std::chrono::high_resolution_clock> debug_time;

        void calculate_fps();

    public:
        Controller();
        Controller(int target_fps, bool debug_show_fps);
        void start();
        float end();
};

#endif