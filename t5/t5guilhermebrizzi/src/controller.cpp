#include "controller.h"

#include <chrono>
#include <iostream>
#include <Windows.h>

Controller::Controller()
{

}

Controller::Controller(int target_fps, bool debug_show_fps)
{
    this->target_fps = target_fps;
    this->debug_show_fps = debug_show_fps;
    debug_time = std::chrono::high_resolution_clock::now();

    frame_duration = (1.0 / target_fps)*1000;
}

void Controller::start()
{
    this->start_time = std::chrono::high_resolution_clock::now();
}

void Controller::calculate_fps()
{
    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - debug_time).count();
    
    if (elapsed_time >= 1000)
    {
        debug_fps = debug_frame_counter;
        debug_frame_counter = 0;
        debug_time = end_time;
    }
}

float Controller::end()
{
    if (debug_show_fps)
    {
        calculate_fps();
        std::cout << debug_fps << std::endl;
    }
    
    debug_frame_counter += 1;

    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    int sleep_time = frame_duration - elapsed_time;

    if (sleep_time > 0)
    {
        Sleep(sleep_time);
        return 1.0;
    }

    return elapsed_time / (1000/60.0);
}