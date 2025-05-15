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

        // game stats
        int points = 0;
        int level = 1;

        // simlation state control
        bool levelup = true;
        bool halted = true;
        bool game_over = false;
        bool starting = true;

        // countdown 3 seconds to let player prepare
        int starting_loading_ticks = 3 * 60; 

        // entities and events vectors
        std::vector<std::unique_ptr<Entity>> entities;
        std::vector<std::unique_ptr<Event>> events;

        // image assets not tied to entities
        std::vector<CVpro::image *> numbers_bmps;
        CVpro::image *wasted_bmp;

        void draw_points();    
        void handle_system_event(Event *e);
        void repopulate();
        void collide();
        bool check_collision(Entity *a, Entity *b);
        void remove_entity(int id);
        bool inside_track(Vector2 pos, CVpro::image *src);
        void tick_starting();
        void display_game();
        void display_starting_countdown();
        void display_game_over();

        // enemy waves generators for each level
        // many functions because each levelk is custom
        void generate_lv1_wave();


    public:
        Simulation(int screen_width, int screen_height);
        void update();
        void display();
        void add_entity(Entity *e);
        void add_event(Event *e);
};


#endif