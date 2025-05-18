#ifndef HEALTH_POWER_UP
#define HEALTH_POWER_UP

#include "entity.h"

class Health_Powerup : public Entity
{
    private:    

    public:
        Health_Powerup(float x, float y);
        virtual void draw();
        virtual void collide(Entity *e);

};




#endif