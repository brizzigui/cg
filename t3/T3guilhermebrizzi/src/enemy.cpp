#include "enemy.h"
#include "special_events.h"
#include "Vector2.h"

CVpro::image *Enemy::get_texture(int type)
{
    switch (type)
    {
        case 1:
            return CVpro::load_bitmap("./T3guilhermebrizzi/assets/enemies/level1.bmp");
        case 2:
            return CVpro::load_bitmap("./T3guilhermebrizzi/assets/enemies/level2.bmp");
        case 3:
            return CVpro::load_bitmap("./T3guilhermebrizzi/assets/enemies/level3.bmp");
        case 4:
            return CVpro::load_bitmap("./T3guilhermebrizzi/assets/enemies/level4.bmp");
        case 5:
            return CVpro::load_bitmap("./T3guilhermebrizzi/assets/enemies/level5.bmp");
    }
    return NULL;
}

Enemy::Enemy(float x, float y, Vector2 direction, float speed, int type) : Entity(x, y)
{
    this->x = x;
    this->y = y;
    this->direction = direction;
    this->speed = speed;
    this->type = type;

    this->texture = get_texture(type);
}

void Enemy::draw()
{
    if (change)
    {
        footprint.clear();
    }
    
    box = texture->display_bitmap(x, y, footprint);
}

void Enemy::tick()
{

}

void Enemy::collide(Entity *e)
{
    // if something created by the tank (eg. a bullet)
    // or the tank itself
    if (e->god_id == 1 || e->id == 1)
    {
        switch (type)
        {
            case 1:
                events_ptr->push_back(std::unique_ptr<Event>(new Event_Suicide(id)));
                events_ptr->push_back(std::unique_ptr<Event>(new Event_Point(10)));
                break;
            
            default:
                break;
        }
    }

}