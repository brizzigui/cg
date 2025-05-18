#include "event.h"
#include "entity.h"

Event::Event(){}

Event_Mouse::Event_Mouse(int button, int state, int wheel, int direction, int x, int y, bool held)
{
    this->type = EVENT_MOUSE;
    this->button = button;
    this->state = state;
    this->wheel = wheel;
    this->direction = direction;
    this->x = x;
    this->y = y;
    this->held = held;
}

Event_Key_Down::Event_Key_Down(int key)
{
    this->type = EVENT_KEY_DOWN;
    this->key = key;
}

Event_Key_Up::Event_Key_Up(int key)
{
    this->type = EVENT_KEY_UP;
    this->key = key;
}

Event_Internal_Pos_Share::Event_Internal_Pos_Share(Vector2 pos)
{
    this->type = EVENT_INTERNAL_POS_SHARE;
    this->pos = pos;
}

bool Event::is_system_event()
{
    if (type == EVENT_CREATE_ENTITY ||
        type == EVENT_SUICIDE ||
        type == EVENT_POINT ||
        type == EVENT_GAME_OVER ||
        type == EVENT_HEALTH)
    {
        return true;
    }   

    return false;
}