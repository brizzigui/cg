#ifndef ENEMY_H
#define ENEMY_H

#include "entity.h"
#include "Vector2.h"
#include "canvas_pro.h"

class Enemy : public Entity
{
    private:
        Vector2 direction;
        float speed;
        int type;

        int health = 100;
        int gun_cooldown = -1;
        bool alternate_pattern = false;

        Vector2 tank_pos;

        CVpro::image *get_texture(int type);
        void draw_health_bar();
        void set_special_behavior();

        void handle_type1_collision();
        void handle_type2_collision();
        void handle_type3_collision();
        void handle_type4_collision();
        void handle_type5_collision();

        void handle_type1_tick();
        void handle_type2_tick();
        void handle_type3_tick();
        void handle_type4_tick();
        void handle_type5_tick();

        void shoot_back(int quantum, int speed);

    public:
        Enemy(float x, float y, Vector2 direction, float speed, int type);
        virtual void draw();
        virtual void tick();
        virtual void input(Event *e);
        virtual void collide(Entity *e);
};


#endif