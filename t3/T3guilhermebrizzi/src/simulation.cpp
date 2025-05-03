#include "simulation.h"
#include "special_events.h"
#include <iostream>

Simulation::Simulation(int screen_width, int screen_height)
{
    this->screen_width = screen_width;
    this->screen_height = screen_height;
}

void Simulation::display()
{
    for (auto &e : entities)
    {
        e->draw();
    }
}

void Simulation::handle_system_event(Event *e)
{
    if (e->type == EVENT_CREATE_ENTITY)
    {
        std::cout << "got a system event of type request entity creation" << std::endl;
        std::cout << "will add to vector entity with address " << ((Event_Create_Entity *)(e))->entity << std::endl;
        Entity *entity = ((Event_Create_Entity *)(e))->entity;
        entities.push_back(std::unique_ptr<Entity>(entity));
    }
    
}

void Simulation::update()
{
    for (auto &entity : entities)
    {
        entity->tick();
        for (auto &event : events)
        {
            entity->input(event.get());
        }
    }

    for (auto &event : events)
    {
        if (event->is_system_event())
        {
            handle_system_event(event.get());
        }
    }

    events.clear();
}

void Simulation::add_entity(Entity *e)
{
    e->events_ptr = &events;
    entities.push_back(std::unique_ptr<Entity>(e));
}

void Simulation::add_event(Event *e)
{
    events.push_back(std::unique_ptr<Event>(e));
}