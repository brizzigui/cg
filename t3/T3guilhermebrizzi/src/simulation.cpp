#include "simulation.h"
#include "special_events.h"
#include "enemy.h"
#include "clamp.h"
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

void Simulation::repopulate()
{
    if (rand()%100 == 0)
    {
        add_entity(new Enemy(rand()%screen_width, rand()%screen_height, Vector2(0, 1), 0, 1));
    }
}

bool Simulation::check_collision(Entity *a, Entity *b)
{
    // will need to do it pixel based but with bounding box approximation
    // its the only way
    // kill all the Footprints
    // entity holds Bounding_Box
    // Entity::display is responsible for updating Bounding_Box with accuracy
    // check if bounding box overlaps here
    // for overlap, check pixels
    // if do, check collision
    // return true if collide
    // keep going if not

    // updating bounding box shouldnt be hard.
    // get max_x, max_y, min_x, min_y
    // bam! bounding box

    // i think this is finally a viable solution

    if (a->box.min_x > b->box.max_x || a->box.max_x < b->box.min_x || a->box.min_y > b->box.max_y || a->box.max_y < b->box.min_y)
    {
        return false;
    }
    
    Bounding_Box overlap = Bounding_Box(
                                        std::max(a->box.min_x, b->box.min_x),
                                        std::max(a->box.min_y, b->box.min_y),
                                        std::min(a->box.max_x, b->box.max_x),
                                        std::min(a->box.max_y, b->box.max_y)
                                    );
    
    for (int i = overlap.min_y; i < overlap.max_y; i++)
    {
        for (int j = overlap.min_x; j < overlap.max_x; j++)
        {
            int base_index = i * Footprint::width + j;
            if (a->footprint.matrix[base_index] && b->footprint.matrix[base_index])
            {
                return true;
            }
        }
    }
    
    return false;
}

void Simulation::collide()
{
    for (int outer = 0; outer < (int)entities.size(); outer++)
    {
        for (int inner = outer + 1; inner < (int)entities.size(); inner++)
        {
            if (check_collision(entities[outer].get(), entities[inner].get()))
            {
                std::cout << "collided entities with index " << outer << ", " << inner << std::endl;
                //entities[outer].get()->collide();
                //entities[inner].get()->collide();
            }
        }
    }
}

void Simulation::update()
{
    collide();

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

    repopulate();

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