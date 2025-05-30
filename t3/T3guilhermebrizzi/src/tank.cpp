#include "tank.h"
#include "canvas_pro.h"
#include "event.h"
#include "special_events.h"
#include "health_pwrup.h"

// constructor for Tank, initializes position, direction, speed, and loads images
Tank::Tank(float x, float y, Vector2 direction, float speed) : Entity(x, y)
{
    texture = CVpro::load_bitmap("./T3guilhermebrizzi/assets/tanks/tankRed_outline.bmp");
    barrel = CVpro::load_bitmap("./T3guilhermebrizzi/assets/barrels/barrelRed_outline.bmp");

    this->x = x;
    this->y = y;
    this->direction = direction;
    this->speed = speed;
    gun_direction = direction;
}

// draws the tank base with a given angle and updates the footprint
void Tank::draw_base(float angle)   
{   
    footprint.clear();
    box = texture->display_bitmap(x, y, angle, footprint);
}

// draws the tank's gun (barrel) with a given angle
void Tank::draw_gun(float angle)
{
    CVpro::color(255, 255, 255);
    barrel->display_bitmap(x, y, angle+PI/2.0);
}

// draws the health bar above the tank
void Tank::draw_health_bar()
{
    CVpro::color(184, 2, 2);
    CV::rectFill(x-30, box.max_y+10, x+30, box.max_y+20);

    CVpro::color(14, 184, 2);
    float health_len = 60 * (health/100.0);
    CV::rectFill(x-30, box.max_y+10, x-30+health_len, box.max_y+20);
}

// draws the tank, its gun, and health bar
void Tank::draw()
{
    float angle = direction.get_angle();
    draw_base(angle);
    float gun_angle = gun_direction.get_angle();
    draw_gun(gun_angle);
    draw_health_bar();
}

// checks if the tank's health is zero and triggers game over event
void Tank::check_game_over()
{
    if (health == 0)
    {
        events_ptr->push_back(
            std::unique_ptr<Event>(new Event_Game_Over())
        );
    }
}

// updates the tank's position and state each tick
void Tank::tick()
{
    x += direction.x * speed;
    y += direction.y * speed;

    unlock_actions();
    recalc_gun_angle();

    change = true;

    check_game_over();
}

// recalculates the gun's direction based on where it should point
void Tank::recalc_gun_angle()
{
    gun_direction = Vector2(gun_pointing_to_x-x, gun_pointing_to_y-y);
    gun_direction.normalize();
}

// recalculates the gun's direction using mouse event coordinates
void Tank::recalc_gun_angle(Event_Mouse *e)
{
    gun_pointing_to_x = e->x;
    gun_pointing_to_y = e->y;
    recalc_gun_angle();
}

// returns the position of the tip of the barrel
Vector2 Tank::get_barrel_tip()
{
    return Vector2(x + gun_direction.x*barrel->height/2.0,
                   y + gun_direction.y*barrel->height/2.0);    
}

// handles mouse input for aiming and shooting
void Tank::handle_mouse_input(Event_Mouse *e)
{
    recalc_gun_angle(e);

    if (tick_lock_gun == 0 && e->state == 0 && e->button == 0)
    {
        Vector2 start = get_barrel_tip();
        // clicked left button - shoot!
        events_ptr->push_back(std::unique_ptr<Event>(
            new Event_Create_Entity(
                new Gunshot(start.x, start.y, gun_direction, GUNSHOT_DEFAULT_SPEED, id)
            )
        ));
        tick_lock_gun = gun_cooldown;
    }    
}

// handles keyboard input for rotating the tank
void Tank::handle_keyboard_input(Event_Key_Down *e)
{
    if (tick_lock_rotation == 0 && (e->key == 100 || e->key == 97))
    {
        if (e->key == 100) // 100 is D
        {
            direction.rotate_by(PI/64);
        }
        
        else    // 97 is A
        {
            direction.rotate_by(-PI/64);
        }   

        tick_lock_rotation = rotation_cooldown;
    }
}

// unlocks tank actions by updating cooldown timers
void Tank::unlock_actions()
{
    tick_lock_rotation = (tick_lock_rotation-1 < 0) ? 0 : tick_lock_rotation-1;
    tick_lock_gun = (tick_lock_gun-1 < 0) ? 0 : tick_lock_gun-1;
    tick_lock_health = (tick_lock_health-1 < 0) ? 0 : tick_lock_health-1;
}

// processes input events for the tank
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

// handles collision with other entities
void Tank::collide(Entity *e)
{
    // touched own bullet
    if (e->god_id == id)
    {
        return;
    }

    // went against barrier
    if (e->id == 0)
    {       
        x -= direction.x * speed;
        y -= direction.y * speed;
    }

    if (e->thin)
    {
        if (dynamic_cast<Health_Powerup *>(e) != NULL)
        {
            health += 25;
            health = (health > 100) ? 100 : health;
        }
    }
    
    // change for an any damage
    else if (tick_lock_health == 0)
    {
        tick_lock_health = health_cooldown;
        health -= 10;
        if (e->id == 0)
            health -= 5;
        health = (health < 0) ? 0 : health;
    }
}