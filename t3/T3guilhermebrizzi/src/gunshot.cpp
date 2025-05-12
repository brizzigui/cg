#include "gunshot.h"
#include "special_events.h"

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
    }
    
    box = texture->display_bitmap(x, y, footprint);
}

void Gunshot::tick()
{
    x += direction.x * speed;
    y += direction.y * speed;

    change = true;
}

void Gunshot::collide(Entity *e)
{
    if (e->id == god_id)
    {
        return;
    }

    events_ptr->push_back(
        std::unique_ptr<Event>(
            new Event_Suicide(id)
        )
    );
    
}