#include "enemy.h"
#include "Vector2.h"

CVpro::image *Enemy::get_texture(int type)
{
    return CVpro::load_bitmap("./T3guilhermebrizzi/assets/enemies/level1.bmp");
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
    
}