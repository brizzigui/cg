#ifndef EVENT_H
#define EVENT_H

#define EVENT_KEY_DOWN 0
#define EVENT_KEY_UP 1
#define EVENT_MOUSE 2
#define EVENT_CREATE_ENTITY 3
#define EVENT_SUICIDE 4

class Event
{
    public:
        int type;       
        Event();
        virtual ~Event() {}
        bool is_system_event();
};

class Event_Key_Down : public Event
{
    public:
        int key;
        Event_Key_Down(int key);
};

class Event_Key_Up : public Event
{
    public:
        int key;
        Event_Key_Up(int key);
};

class Event_Mouse : public Event
{
    public:
        int button, state, wheel, direction, x, y; 
        bool held;
        Event_Mouse(int button, int state, int wheel, int direction, int x, int y, bool held);
};

// other events are defined in special_events.h

#endif