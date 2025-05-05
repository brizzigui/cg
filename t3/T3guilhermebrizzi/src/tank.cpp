#include "tank.h"
#include "canvas_pro.h"
#include "event.h"
#include "special_events.h"

Tank::Tank(float x, float y, Vector2 direction, float speed) : Entity(x, y)
{
    texture = CVpro::load_bitmap("./T3guilhermebrizzi/assets/base_tank.bmp");

    this->x = x;
    this->y = y;
    this->direction = direction;
    this->speed = speed;

    recalc_gun_angle();
}

// im not using CVpro::image::display_bitmap because of an adjustment specific to the tank's behavior
// (custom rotation pivot)
void Tank::draw_base(float angle)   
{   
    box = Bounding_Box();
    float height = texture->height;
    float width = texture->width;
    subpixel *matrix = texture->matrix;

    int max_dim = height + width;
    
    for (int y_dst = - max_dim; y_dst < max_dim; y_dst++) 
    {
        for (int x_dst = - max_dim; x_dst < max_dim; x_dst++) 
        {
            float x_src = cos(-angle) * x_dst - sin(-angle) * y_dst;
            float y_src = sin(-angle) * x_dst + cos(-angle) * y_dst;
    
            int i = (int)y_src + height/2.0;
            int j = (int)x_src;
    
            if (i >= 0 && i < height && j >= 0 && j < width) 
            {
                int base_index = i * width * 4 + j * 4;
                CVpro::color(matrix[base_index + 2], matrix[base_index + 1], matrix[base_index], matrix[base_index + 3]);
                if (matrix[base_index + 3] > 0)
                {
                    CV::point(x + x_dst, y + y_dst);
                    footprint.mark_pixel(x + x_dst, y + y_dst);
                    box.update(x + x_dst, y + y_dst);
                }
            }
        }
    }
}

Vector2 Tank::get_gun_center(float angle)
{
    float width = texture->width;
    int drop_x = cos(angle) * width / 2.0;
    int drop_y = sin(angle) * width / 2.0;

    return Vector2(x+drop_x, y+drop_y);
}

void Tank::draw_gun(float angle)
{
    Vector2 gun_center = get_gun_center(angle);

    CVpro::color(255, 255, 255);
    CV::line(gun_center.x, gun_center.y, gun_center.x+gun_direction.x*20, gun_center.y+gun_direction.y*20);
    // TODO: IF GUN IS CHANGED, NEED TO CHANGE ITS FOOTPRINT
}

// @CustomDraw
void Tank::draw()
{
    float angle = direction.get_angle();
    draw_base(angle);
    draw_gun(angle);
}

void Tank::tick()
{
    x += direction.x * speed;
    y += direction.y * speed;

    unlock_actions();
    recalc_gun_angle();
}

void Tank::recalc_gun_angle()
{
    gun_direction = Vector2(gun_pointing_to_x-x, gun_pointing_to_y-y);
    gun_direction.normalize();
}

void Tank::recalc_gun_angle(Event_Mouse *e)
{
    gun_pointing_to_x = e->x;
    gun_pointing_to_y = e->y;
    recalc_gun_angle();
}

void Tank::handle_mouse_input(Event_Mouse *e)
{
    recalc_gun_angle(e);

    if (tick_lock_gun == 0 && e->state == 0 && e->button == 0)
    {
        Vector2 gun_center = get_gun_center(direction.get_angle());

        // clicked left button - shoot!
        events_ptr->push_back(std::unique_ptr<Event>(
            new Event_Create_Entity(
                new Gunshot(gun_center.x, gun_center.y, gun_direction, GUNSHOT_DEFAULT_SPEED)
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
            direction.rotate_by(PI/128);
        }
        
        else    // 97 is A
        {
            direction.rotate_by(-PI/128);
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