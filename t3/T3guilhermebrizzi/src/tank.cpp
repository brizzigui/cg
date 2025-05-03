#include "tank.h"
#include "canvas_pro.h"
#include "event.h"
#include "special_events.h"

Tank::Tank(float x, float y, Vector2 direction, float speed) : Entity(x, y)
{
    this->x = x;
    this->y = y;
    this->direction = direction;
    this->speed = speed;

    gun_direction = Vector2(1, 0);
}

void Tank::draw()
{
    CVpro::color(0, 0, 255);
    CV::circleFill(x, y, 15, 15);
    CVpro::color(255, 255, 255);
    CV::line(x, y, x+gun_direction.x*20, y+gun_direction.y*20);
}

void Tank::tick()
{
    x += direction.x * speed;
    y += direction.y * speed;

    unlock_actions();
}

void Tank::recalc_gun_angle(Event_Mouse *e)
{
    gun_direction = Vector2(e->x-x, e->y-y);
    gun_direction.normalize();
}

void Tank::handle_mouse_input(Event_Mouse *e)
{
    recalc_gun_angle(e);

    if (tick_lock_gun == 0 && e->state == 0 && e->button == 0)
    {
        // clicked left button - shoot!
        events_ptr->push_back(std::unique_ptr<Event>(
            new Event_Create_Entity(
                new Gunshot(x, y, gun_direction, GUNSHOT_DEFAULT_SPEED)
            )
        ));
        tick_lock_gun = gun_cooldown;
    }    
}

void Tank::handle_keyboard_input(Event_Key_Down *e)
{
    if (tick_lock_rotation == 0 && (e->key == 100 || e->key == 97))
    {
        if (e->key == 100) // 100 is D
        {
            direction.rotate_by(PI/8);
        }
        
        else    // 97 is A
        {
            direction.rotate_by(-PI/8);
        }   

        tick_lock_rotation = rotation_cooldown;
    }
}

void Tank::unlock_actions()
{
    tick_lock_rotation = (tick_lock_rotation-1 < 0) ? 0 : tick_lock_rotation-1;
    tick_lock_gun = (tick_lock_gun-1 < 0) ? 0 : tick_lock_gun-1;
}

void Tank::input(Event *e)
{
    if (e->type == EVENT_MOUSE)
    {
        handle_mouse_input((Event_Mouse *)e);
    }

    else if (e->type == EVENT_KEY_DOWN)
    {
        handle_keyboard_input((Event_Key_Down *)e);
    }
}