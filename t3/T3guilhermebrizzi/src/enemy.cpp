#include "enemy.h"
#include "gunshot.h"
#include "special_events.h"
#include "Vector2.h"
#include "explosion.h"

// returns the texture image for the enemy based on its type
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

// sets special behavior for certain enemy types (like gun cooldown)
void Enemy::set_special_behavior()
{
    switch (type)
    {
        case 3:
            gun_cooldown = 60;
            break;

        case 4:
            gun_cooldown = 60;
            break;

        case 5:
            gun_cooldown = 30;
            break;
    }
}

// constructor for Enemy, initializes position, direction, speed, type, and texture
Enemy::Enemy(float x, float y, Vector2 direction, float speed, int type) : Entity(x, y)
{
    this->x = x;
    this->y = y;
    this->direction = direction;
    this->speed = speed;
    this->type = type;
    set_special_behavior();

    this->texture = get_texture(type);
}

// draws the health bar above the enemy if health is less than 100
void Enemy::draw_health_bar()
{
    if(health < 100)
    {
        int base_x = x-20+texture->width/2;
        CVpro::color(184, 2, 2);
        CV::rectFill(base_x, box.max_y+10, base_x+40, box.max_y+20);

        CVpro::color(14, 184, 2);
        float health_len = 40 * (health/100.0);
        CV::rectFill(base_x, box.max_y+10, base_x+health_len, box.max_y+20);
    }
}

// draws the enemy and its health bar if needed
void Enemy::draw()
{
    if (change)
    {
        footprint.clear();
        change = false;
    }
    
    box = texture->display_bitmap(x, y, footprint);
    if (type >= 2)
    {
        draw_health_bar();
    }
}

// handles tick logic for type 1 enemy (does nothing)
void Enemy::handle_type1_tick()
{
    return;
}

// handles tick logic for type 2 enemy (random movement)
void Enemy::handle_type2_tick()
{
    change = true;
    x += rand()%5 * ((rand()%3)-1);
    y += rand()%5 * ((rand()%3)-1); 
}

// shoots a bullet back at the tank if gun cooldown is zero
void Enemy::shoot_back(int quantum, int speed)
{
    gun_cooldown--;
    gun_cooldown = (gun_cooldown < 0) ? 0 : gun_cooldown;

    if (gun_cooldown == 0)
    {
        Vector2 gunshot_direction = Vector2(tank_pos.x-x, tank_pos.y-y);
        gunshot_direction.normalize();
        events_ptr->push_back(std::unique_ptr<Event>(
                new Event_Create_Entity(
                    new Gunshot(x+texture->width/2, y+texture->height/2, gunshot_direction, speed, id)
                )
            ));

        gun_cooldown = quantum;
    }
}

// handles tick logic for type 3 enemy (shoots back at tank)
void Enemy::handle_type3_tick()
{
    shoot_back(120, GUNSHOT_DEFAULT_SPEED/2);
}

// handles tick logic for type 4 enemy (random movement and shoots back)
void Enemy::handle_type4_tick()
{
    change = true;
    x += rand()%5 * ((rand()%3)-1);
    y += rand()%5 * ((rand()%3)-1); 
    shoot_back(60, GUNSHOT_DEFAULT_SPEED);
}

// handles tick logic for type 5 enemy (shoots bullets in all directions)
void Enemy::handle_type5_tick()
{
    gun_cooldown--;
    gun_cooldown = (gun_cooldown < 0) ? 0 : gun_cooldown;

    if (gun_cooldown == 0)
    {
        for (float angle = 0; angle < 2*PI; angle += PI/4.0)
        {
            Vector2 gunshot_direction = Vector2(cos(angle), sin(angle));
            events_ptr->push_back(std::unique_ptr<Event>(
                    new Event_Create_Entity(
                        new Gunshot(x+texture->width/2, y+texture->height/2, gunshot_direction, GUNSHOT_DEFAULT_SPEED, id)
                    )
                ));
        }
        
        gun_cooldown = 60;
    }

}

// updates the enemy each tick based on its type
void Enemy::tick()
{
    switch (type)
    {
        case 1:
            handle_type1_tick();
            break;

        case 2:
            handle_type2_tick();
            break;

        case 3:
            handle_type3_tick();
            break;

        case 4:
            handle_type4_tick();
            break;

        case 5:
            handle_type5_tick();
            break;

        default:
            break;
    }
}

// handles collision logic for type 1 enemy (suicide and points)
void Enemy::handle_type1_collision()
{
    events_ptr->push_back(std::unique_ptr<Event>(new Event_Suicide(id)));
    events_ptr->push_back(std::unique_ptr<Event>(new Event_Point(10)));
    events_ptr->push_back(std::unique_ptr<Event>(
        new Event_Create_Entity(new Explosion(x, y, 60))
    ));
}

// handles collision logic for type 2 enemy (reduce health, suicide if dead)
void Enemy::handle_type2_collision()
{
    health = health - 50;
    health = (health < 0) ? 0 : health;

    if (health == 0)
    {
        events_ptr->push_back(std::unique_ptr<Event>(new Event_Suicide(id)));
        events_ptr->push_back(std::unique_ptr<Event>(new Event_Point(20)));
        events_ptr->push_back(std::unique_ptr<Event>(
            new Event_Create_Entity(new Explosion(x, y, 60))
        ));
    }
}

// handles collision logic for type 3 enemy (random health loss, suicide if dead)
void Enemy::handle_type3_collision()
{
    health = health - rand()%30 - 20;
    health = (health < 0) ? 0 : health;

    if (health == 0)
    {
        events_ptr->push_back(std::unique_ptr<Event>(new Event_Suicide(id)));
        events_ptr->push_back(std::unique_ptr<Event>(new Event_Point(30)));
        events_ptr->push_back(std::unique_ptr<Event>(
            new Event_Create_Entity(new Explosion(x, y, 60))
        ));
    }
}

// handles collision logic for type 4 enemy (random health loss, suicide if dead)
void Enemy::handle_type4_collision()
{
    health = health - rand()%10 - 10;
    health = (health < 0) ? 0 : health;

    if (health == 0)
    {
        events_ptr->push_back(std::unique_ptr<Event>(new Event_Suicide(id)));
        events_ptr->push_back(std::unique_ptr<Event>(new Event_Point(40)));
        events_ptr->push_back(std::unique_ptr<Event>(
            new Event_Create_Entity(new Explosion(x, y, 60))
        ));
    }
}

// handles collision logic for type 5 enemy (reduce health, respawn if dead)
void Enemy::handle_type5_collision()
{
    health = health - 5;
    health = (health < 0) ? 0 : health;

    if (health == 0)
    {
        health = 100;
        events_ptr->push_back(std::unique_ptr<Event>(new Event_Point(100)));
    }
}

// handles collision with other entities, triggers appropriate behavior based on type
void Enemy::collide(Entity *e)
{
    // if something created by the tank (eg. a bullet)
    // or the tank itself
    if (e->god_id == 1 || e->id == 1)
    {
        switch (type)
        {
            case 1:
                handle_type1_collision();
                break;

            case 2:
                handle_type2_collision();
                break;

            case 3:
                handle_type3_collision();
                break;

            case 4:
                handle_type4_collision();
                break;

            case 5:
                if (e->god_id == 1)
                {
                    handle_type5_collision();  
                }
            
                break;

            default:
                break;
        }
    }

    else if (e->god_id != id && dynamic_cast<Enemy *>(e) == NULL && !thin)
    {
        events_ptr->push_back(std::unique_ptr<Event>(new Event_Suicide(id)));
        events_ptr->push_back(std::unique_ptr<Event>(
            new Event_Create_Entity(new Explosion(x, y, 60))
        ));
    }

}

// processes input events for the enemy (updates tank position if needed)
void Enemy::input(Event *e)
{
    if (e->type == EVENT_INTERNAL_POS_SHARE)
    {
        tank_pos = ((Event_Internal_Pos_Share *)e)->pos;
    }
}