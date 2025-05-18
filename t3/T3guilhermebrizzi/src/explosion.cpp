#include "explosion.h"
#include "special_events.h"

Explosion::Explosion(float x, float y, int tick_lenght) : Entity(x, y)
{
    this->tick_lenght = tick_lenght;
    this->tick_counter = tick_lenght;

    this->collidable = false;
    for (int i = 0; i < 9; i++)
    {
        char path[128];
        sprintf(path, "./T3guilhermebrizzi/assets/smoke/explosion%d.bmp", i+1);
        bmps.push_back(CVpro::load_bitmap(path));
    }

    this->x = x+bmps[0]->width/2.0;
    this->y = y+bmps[0]->height/2.0;
}

void Explosion::draw()
{
    float scale = (float)tick_counter/tick_lenght;
    int index = tick_counter % (int)bmps.size();
    float realx = x - (bmps[index]->width/2.0) * scale;
    float realy = y - (bmps[index]->height/2.0) * scale;
    bmps[index]->display_bitmap_scaled(realx, realy, scale);
}

void Explosion::tick()
{
    tick_counter--;
    if (tick_counter == 0)
    {
        events_ptr->push_back(
            std::unique_ptr<Event>(new Event_Suicide(id))
        );
    }
}

Explosion::~Explosion()
{
    for (int i = 0; i < (int)bmps.size(); i++)
    {
        free(bmps[i]);
    }
}