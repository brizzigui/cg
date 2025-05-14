#ifndef SIMULATION_H
#define SIMULATION_H

#include <memory>
#include <vector>

#include "entity.h"
#include "event.h"

class Simulation
{
    private:
        int screen_width;
        int screen_height;

        int points = 0;
        int level = 1;

        bool levelup = true;

        std::vector<std::unique_ptr<Entity>> entities;
        std::vector<std::unique_ptr<Event>> events;

        void draw_points();    
        void handle_system_event(Event *e);
        void repopulate();
        void collide();
        bool check_collision(Entity *a, Entity *b);
        void remove_entity(int id);
        bool inside_track(Vector2 pos, CVpro::image *src);
        void generate_lv1_wave();


    public:
        Simulation(int screen_width, int screen_height);
        void update();
        void display();
        void add_entity(Entity *e);
        void add_event(Event *e);
};


#endif