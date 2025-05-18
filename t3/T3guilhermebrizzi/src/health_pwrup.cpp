#include "health_pwrup.h"
#include "special_events.h"

Health_Powerup::Health_Powerup(float x, float y) : Entity(x, y)
{
    this->x = x;
    this->y = y;
    this->texture = CVpro::load_bitmap("./T3guilhermebrizzi/assets/powerups/health.bmp");
    this->thin = true;
}

void Health_Powerup::draw()
{
    box = texture->display_bitmap(x, y, footprint);
}

void Health_Powerup::collide(Entity *e)
{
    // if collided with tank or something created by the tank
    if (e->id == 1 || e->god_id == 1)
    {
        events_ptr->push_back(
            std::unique_ptr<Event>(new Event_Suicide(id))
        );
    }
}