#include "gunshot.h"
#include "special_events.h"
#include "health_pwrup.h"

#define GUNSHOT_SPEED 5

Gunshot::Gunshot(float x, float y, Vector2 direction, float speed, int creator) : Entity(x, y)
{
    texture = CVpro::load_bitmap("./T3guilhermebrizzi/assets/bullets/bulletRed_outline.bmp");
    this->direction = direction;
    this->speed = speed;
    this->god_id = creator;
}

void Gunshot::draw()
{
    if (change)
    {
        footprint.clear();
        change = false;
    }
    
    box = texture->display_bitmap(x, y, direction.get_angle() + PI/2.0, footprint);
}

void Gunshot::tick()
{
    x += direction.x * speed * 2;
    y += direction.y * speed * 2;

    change = true;
}

void Gunshot::collide(Entity *e)
{
    if (e->id == god_id || e->god_id == god_id || (e->thin && god_id != 1))
    {
        return;
    }

    if (e->thin && god_id == 1)
    {
        if (dynamic_cast<Health_Powerup *>(e) != NULL)
        {
            events_ptr->push_back(
                std::unique_ptr<Event>(
                    new Event_Health(25)
                )
            );
        }
    }

    events_ptr->push_back(
        std::unique_ptr<Event>(
            new Event_Suicide(id)
        )
    );
    
}