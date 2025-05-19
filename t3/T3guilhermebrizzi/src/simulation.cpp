#include "simulation.h"
#include "special_events.h"
#include "enemy.h"
#include "track.h"
#include "tank.h"
#include "health_pwrup.h"
#include <iostream>

// initializes the simulation with screen size and loads bitmaps
Simulation::Simulation(int screen_width, int screen_height)
{
    this->screen_width = screen_width;
    this->screen_height = screen_height;

    numbers_bmps.push_back(NULL);
    numbers_bmps.push_back(CVpro::load_bitmap("./T3guilhermebrizzi/assets/numbers/1.bmp"));
    numbers_bmps.push_back(CVpro::load_bitmap("./T3guilhermebrizzi/assets/numbers/2.bmp"));
    numbers_bmps.push_back(CVpro::load_bitmap("./T3guilhermebrizzi/assets/numbers/3.bmp"));
    wasted_bmp = CVpro::load_bitmap("./T3guilhermebrizzi/assets/ui/wasted.bmp");
    black_star = CVpro::load_bitmap("./T3guilhermebrizzi/assets/ui/black_star.bmp");
    red_star = CVpro::load_bitmap("./T3guilhermebrizzi/assets/ui/red_star.bmp");
}

// draws the points box and current points on the screen
void Simulation::draw_points()
{
    CVpro::color(255, 255, 255);
    CV::rectFill(screen_width/2.0-50, 10, screen_width/2.0+50, 10+30);
    CVpro::color(0, 0, 0);
    CV::rect(screen_width/2.0-50, 10, screen_width/2.0+50, 10+30);
    CVpro::text_align(screen_width/2.0, 30, 'c', "%d pts", points);
}

// shows the countdown at the start of the game
void Simulation::display_starting_countdown()
{
    int index = starting_loading_ticks/60 + 1;
    float scale = (starting_loading_ticks%60)/60.0;
    numbers_bmps[index]->display_bitmap_scaled(screen_width/2 - numbers_bmps[index]->width*scale*0.5,
                                        screen_height/2 - numbers_bmps[index]->height*scale*0.5,
                                        scale);
}

// displays the game over screen and continue button
void Simulation::display_game_over()
{
    CVpro::color(0, 0, 0);
    float vx[4] = {(float)0, (float)screen_width, (float)screen_width, (float)0};
    float vy[4] = {(float)(screen_height/3 - wasted_bmp->height/2 - 100), (float)(screen_height/3 - wasted_bmp->height/2 - 50),
                    (float)(screen_height/3 + wasted_bmp->height/2 + 300), (float)(screen_height/3 + wasted_bmp->height/2 + 250)};
    CV::polygonFill(vx, vy, 4); 
    wasted_bmp->display_bitmap_scaled(screen_width/2 - wasted_bmp->width/2, screen_height/3 - wasted_bmp->height/2, 1.0);

    CVpro::color(149, 30, 37);
    CVpro::text_align(screen_width/2.0, screen_height/2.0, 'c', "You got %d points!", points);
    CV::rectFill(screen_width/2.0 - 75, screen_height/2.0 + 50, screen_width/2.0 + 75, screen_height/2.0 + 100);
    CVpro::color(255, 255, 255);
    CVpro::text_align(screen_width/2.0, screen_height/2.0 + 80, 'c', "Continue");
    CV::rect(screen_width/2.0 - 75, screen_height/2.0 + 50, screen_width/2.0 + 75, screen_height/2.0 + 100);
}

// draws the level stars and level number
void Simulation::draw_level()
{
    int anchorX = screen_width - 180;
    int anchorY = 10;
    int padding = 5;

    for (int i = 0; i < 5; i++)
    {
        if (i < level-1)
        {
            red_star->display_bitmap(anchorX + i*red_star->width + i*padding, anchorY);   
        }
        else
        {
            black_star->display_bitmap(anchorX + i*red_star->width + i*padding, anchorY);   
        }
    }

    CVpro::text(anchorX, anchorY+red_star->height + 15, "Level: %d", level-1);
}

// draws all entities, points, and level
void Simulation::display_game()
{
    for (auto &e : entities)
    {
        e->draw();
    }

    draw_points();
    draw_level();
}

// main display function, shows game or overlays depending on state
void Simulation::display()
{
    display_game();

    if (halted)
    {
        if (starting)
        {
            display_starting_countdown();
        }

        if (game_over)
        {
            display_game_over();
        }
    }
}

// handles health powerup event, increases tank health
void Simulation::handle_health_powerup(Event *e)
{
    int additive = ((Event_Health *)(e))->health;
    int health = ((Tank *)entities[1].get())->health;
    ((Tank *)entities[1].get())->health = (health + additive > 100) ? 100 : health + additive;
}

// handles system events like entity creation, points, game over, etc
void Simulation::handle_system_event(Event *e)
{
    Entity *entity = NULL;
    switch (e->type)
    {
        case EVENT_CREATE_ENTITY:
            entity = ((Event_Create_Entity *)(e))->entity;
            add_entity(entity);
            break;

        case EVENT_SUICIDE:
            remove_entity(((Event_Suicide *)(e))->id);
            break;

        case EVENT_POINT:
            points += ((Event_Point *)(e))->points;
            break;

        case EVENT_GAME_OVER:
            halted = true;
            game_over = true;
            break;

        case EVENT_HEALTH:
            handle_health_powerup(e);
            break;
        
        default:
            break;
    }
}

// checks if a position is inside the track using a source texture
bool Simulation::inside_track(Vector2 pos, CVpro::image *src)
{
    CVpro::image *img = ((Track *)(entities[0].get()))->background;

    if (pos.x >= 0 && pos.x < img->width && pos.y >= 0 && pos.y < img->height)
    {
        int base_index = (int)pos.y * img->width * 4 + (int)pos.x * 4;
        int src_index = ((int)pos.y % src->height) * src->width * 4 + ((int)pos.x % src->width) * 4;

        return  img->matrix[base_index + 2] == src->matrix[src_index + 2] &&
                img->matrix[base_index + 1] == src->matrix[src_index + 1] &&
                img->matrix[base_index    ] == src->matrix[src_index    ] &&
                img->matrix[base_index + 3] == src->matrix[src_index + 3];
    }
    
    return false;
}

// spawns enemy entities at valid positions on the track
void Simulation::respawn(int level, int amount)
{
    // loads background source texture for comparison
    CVpro::image *alphalt_src_texture = CVpro::load_bitmap("./T3guilhermebrizzi/assets/environment/asphalt.bmp");

    int valid = 0;

    while (valid < amount)
    {
        Vector2 pos = Vector2(rand()%screen_width, rand()%screen_height);
        if (inside_track(pos, alphalt_src_texture))
        {
            valid++;
            add_entity(new Enemy(pos.x, pos.y, Vector2(0, 0), 0, level));
        }
    }

    free(alphalt_src_texture);
}

// spawns health powerups at valid positions on the track
void Simulation::spawn_powerups()
{
    // loads background source texture for comparison
    CVpro::image *alphalt_src_texture = CVpro::load_bitmap("./T3guilhermebrizzi/assets/environment/asphalt.bmp");

    int amount = 3;
    int valid = 0;

    while (valid < amount)
    {
        Vector2 pos = Vector2(rand()%screen_width, rand()%screen_height);
        if (inside_track(pos, alphalt_src_texture))
        {
            valid++;
            add_entity(new Health_Powerup(pos.x, pos.y));
        }
    }

    free(alphalt_src_texture);
}

// fills the map with enemies and powerups based on the level
void Simulation::repopulate()
{
    switch (level)
    {
        case 1:
            respawn(level, 10);
            break;

        case 2:
            respawn(level, 15);
            break;

        case 3:
            respawn(level, 5);
            break;

        case 4:
            respawn(level, 10);
            break;

        case 5:
            respawn(level, 5);
            break;
    
        default:
            break;
    }
    
    spawn_powerups();
}

// checks if two entities are colliding using pixel collision
bool Simulation::check_collision(Entity *a, Entity *b)
{
    // based on pixel collision
    if (!(a->collidable && b->collidable))
    {
        return false;
    }
    
    if (a->box.min_x > b->box.max_x || a->box.max_x < b->box.min_x || a->box.min_y > b->box.max_y || a->box.max_y < b->box.min_y)
    {
        return false;
    }
    
    Bounding_Box overlap = Bounding_Box(
                                        std::max(a->box.min_x, b->box.min_x),
                                        std::max(a->box.min_y, b->box.min_y),
                                        std::min(a->box.max_x, b->box.max_x),
                                        std::min(a->box.max_y, b->box.max_y)
                                    );
    
    for (int i = overlap.min_y; i < overlap.max_y; i++)
    {
        for (int j = overlap.min_x; j < overlap.max_x; j++)
        {
            int base_index = i * Footprint::width + j;
            if (a->footprint.matrix[base_index] && b->footprint.matrix[base_index])
            {
                return true;
            }
        }
    }
    
    return false;
}

// checks collision for all pairs of entities and calls their collide methods
void Simulation::collide()
{
    for (int outer = 0; outer < (int)entities.size(); outer++)
    {
        for (int inner = outer + 1; inner < (int)entities.size(); inner++)
        {
            if (check_collision(entities[outer].get(), entities[inner].get()))
            {
                entities[outer].get()->collide(entities[inner].get());
                entities[inner].get()->collide(entities[outer].get());
            }
        }
    }
}

// decreases the starting countdown and unhalts when done
void Simulation::tick_starting()
{
    starting_loading_ticks--;
    if (starting_loading_ticks < 0)
    {
        starting = false;
        halted = false;
    }
}

// generates an internal event to share the tank's position with enemy entities that use it
void Simulation::generate_internal_position_share_event()
{
    add_event(new Event_Internal_Pos_Share(
        Vector2(entities[1].get()->x, entities[1].get()->y)
    ));
}

// checks if the continue button was clicked after game over
bool Simulation::clicked_continue()
{
    for (auto &event : events)
    {
        if (event.get()->type == EVENT_MOUSE)
        {
            auto mouse_event = (Event_Mouse *)event.get();
            return mouse_event->button == 0 && mouse_event->state == 0 &&
                    mouse_event->x > screen_width/2.0 - 75 &&
                    mouse_event->y > screen_height/2.0 + 50 &&
                    mouse_event->x < screen_width/2.0 + 75 &&
                    mouse_event->y < screen_height/2.0 + 100;
        }
    }

    return false;
}

// resets entity id counter (used on restart)
void Simulation::restart()
{
    Entity::id_counter = 0;
}

// cleans up class after gameover
// if simulation still ongoing, returns true
// else, returns false
bool Simulation::cleanup()
{
    if (clicked_continue())
    {
        points = 0;
        level = 1;
        halted = true;
        game_over = false;
        starting = true;
        starting_loading_ticks = 3 * 60;
        entities.clear();
        events.clear();

        return false;
    }
    
    events.clear();
    return true;
}

// main update function, handles game logic and state transitions
bool Simulation::update()
{
    if (halted)
    {
        if (starting)
        {
            tick_starting();
        }
        else if (game_over)
        {
            return cleanup();   
        }
    }
    
    else
    {
        collide();
        generate_internal_position_share_event();

        for (auto &entity : entities)
        {
            entity->tick();
            for (auto &event : events)
            {
                entity->input(event.get());
            }
        }

        for (auto &event : events)
        {
            if (event->is_system_event())
            {
                handle_system_event(event.get());
            }
        }
    
        if (levelup())
        {
            repopulate();
            level++;
            level = (level > 5) ? 5 : level;
        }
    }
    
    events.clear();
    return true;
}

// checks if all enemies are gone to determine level up
bool Simulation::levelup()
{
    for (int i = 0; i < (int)entities.size(); i++)
    {
        if (dynamic_cast<Enemy *>(entities[i].get()) != NULL)
        {
            return false;
        }
    }

    return true;
}

// adds an entity to the simulation
void Simulation::add_entity(Entity *e)
{
    e->events_ptr = &events;
    entities.push_back(std::unique_ptr<Entity>(e));
}

// adds an event to the simulation
void Simulation::add_event(Event *e)
{
    events.push_back(std::unique_ptr<Event>(e));
}

// removes an entity by id
void Simulation::remove_entity(int id)
{
    for (int i = 0; i < (int)entities.size(); i++)
    {
        if (entities[i]->id == id)
        {
            entities.erase(entities.begin() + i);
            break;
        }   
    }
}