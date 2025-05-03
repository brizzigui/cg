#ifndef TANK_H
#define TANK_H

#include "canvas_pro.h"
#include "entity.h"
#include "gunshot.h"
#include "Vector2.h"

class Tank : public Entity
{
    private:
        Vector2 direction;
        float speed;

        int tick_lock_rotation = 0;
        int tick_lock_gun = 0;

        int rotation_cooldown = 10;
        int gun_cooldown = 60;

        Vector2 gun_direction;

        void handle_mouse_input(Event_Mouse *e);
        void handle_keyboard_input(Event_Key_Down *e);
        void recalc_gun_angle(Event_Mouse *e);
        void unlock_actions();

    public:
        Tank(float x, float y, Vector2 direction, float speed);
        virtual void draw();
        virtual void tick();
        virtual void input(Event *e);
    
};

#endif