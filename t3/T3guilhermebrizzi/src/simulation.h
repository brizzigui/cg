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

        std::vector<std::unique_ptr<Entity>> entities;
        std::vector<std::unique_ptr<Event>> events;

        void handle_system_event(Event *e);

    public:
        Simulation(int screen_width, int screen_height);
        void update();
        void display();
        void add_entity(Entity *e);
        void add_event(Event *e);
};


#endif