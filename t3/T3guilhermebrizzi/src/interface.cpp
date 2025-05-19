#include "interface.h"
#include <vector>
#include "Vector2.h"

// constructor for Interface, initializes screen size, points, and loads images
Interface::Interface(int screen_width, int screen_height, std::vector<std::vector<Vector2>> *points)
{
    this->screen_width = screen_width;
    this->screen_height = screen_height;
    this->points = points;
    this->title_logo = CVpro::load_bitmap("./T3guilhermebrizzi/assets/ui/logo.bmp");
    this->track_preview = new Track(0, 0, *points);
}

// displays the editor or splashscreen based on editing state
void Interface::display()
{
    if (editing)
    {
        display_editor();  
    }

    else
    {
        display_splashscreen();
    }
}

// draws the track editor interface and control points
void Interface::display_editor()
{
    for (int i = 0; i < (int)(*points).size(); i++)
    {
        for (int j = 0; j < (int)(*points)[i].size(); j++)
        {
            if (i < 4)
            {
                CVpro::color(255, 0, 0);
            }
            
            else
            {
                CVpro::color(0, 0, 255);
            }

            CV::circleFill((*points)[i][j].x, (*points)[i][j].y, 10, 15);
        }
    }

    CVpro::color(136, 0, 21);
    CV::rectFill(screen_width/2.0 - 100, screen_height - 75, screen_width/2.0 + 100, screen_height - 25);
    CVpro::color(255, 255, 255);
    CV::rect(screen_width/2.0 - 100,  screen_height - 75, screen_width/2.0 + 100, screen_height - 25);
    CVpro::text_align(screen_width/2.0, screen_height - 45, 'c', "Apply and return");
    CVpro::autotext(screen_width/2.0, 15, 'c', 15, "WARNING: drastic changes to track may break game.\nEdit at own risk.");
    track_preview->draw_barebones();
}

// draws the splashscreen with logo and menu buttons
void Interface::display_splashscreen()
{
    title_logo->display_bitmap(screen_width/2.0-title_logo->width/2.0, 100);

    CVpro::color(136, 0, 21);
    CV::rectFill(screen_width/2.0 - 100, 450, screen_width/2.0 + 100, 500);
    CV::rectFill(screen_width/2.0 - 100, 525, screen_width/2.0 + 100, 575);
    CVpro::color(255, 255, 255);
    CV::rect(screen_width/2.0 - 100, 450, screen_width/2.0 + 100, 500);
    CV::rect(screen_width/2.0 - 100, 525, screen_width/2.0 + 100, 575);
    CVpro::text_align(screen_width/2.0, 480, 'c', "Play");
    CVpro::text_align(screen_width/2.0, 555, 'c', "Edit Track");

    CVpro::text_align(screen_width/2.0, screen_height-15, 'c', "Made by Guilherme Brizzi");
}

// returns the indices of the point clicked, or (-1, -1) if none
std::pair<int, int> Interface::get_clicked_point(int x, int y)
{
    int radius = 10;
    for (int i = 0; i < (int)(*points).size(); i++)
    {
        for (int j = 0; j < (int)(*points)[i].size(); j++)
        {
            if (
                ((x-(*points)[i][j].x)*(x-(*points)[i][j].x) +
                (y-(*points)[i][j].y)*(y-(*points)[i][j].y))
                < radius * radius
            )
            {
                return std::make_pair(i, j);
            }
        }
    }
    return std::make_pair(-1, -1);
}

// checks if the exit button was clicked in the editor
bool Interface::exit_clicked(int x, int y)
{
    return x > screen_width/2.0 - 100 && y > screen_height - 75 && 
            x < screen_width/2.0 + 100 && y < screen_height - 25;
}

// updates the editor state based on mouse input
void Interface::update_editor(int button, int state, int x, int y)
{
    if (button == 0 && state == 0)
    {
        std::pair<int, int> clicked_point = get_clicked_point(x, y);
        if (clicked_point.first == -1)
        {
            holding_point = false;
            if (exit_clicked(x, y))
            {
                editing = false;    
            }            
        }
        else
        {
            holding_point = true;
            point_held = clicked_point;
        }
    }
    
    else
    {
        if (holding_point)
        {
            (*points)[point_held.first][point_held.second] = Vector2(x, y);
            if (point_held.second == 3)
            {
                int index = (point_held.first < 4) ? (point_held.first + 1) % 4 : 4 + (point_held.first + 1) % 4;

                (*points)[index][0] = Vector2(x, y);
            }
            else if (point_held.second == 0)
            {
                int index = (point_held.first == 0) ? 3 : 7;
                (*points)[index][3] = Vector2(x, y);
            }
        }
    }
}

// returns the index of the clicked splashscreen button, or -1 if none
int Interface::get_clicked_button(int button, int state, int x, int y)
{
    for (int i = 0; i < 2; i++)
    {
        if (button == 0 && state == 0 && 
            x > screen_width/2.0 - 100 && y > 450 + 75 * i &&
             x < screen_width/2.0 + 100 && y < 500 + 75 * i)
        {
            return i;
        }   
    }

    return -1;
}

// updates the splashscreen state based on mouse input, returns true if play is clicked
bool Interface::update_splashscreen(int button, int state, int x, int y)
{
    int button_clicked = get_clicked_button(button, state, x, y);
    if (button_clicked != -1)
    {
        if (button_clicked == 0)
        {
            return true;
        }
    
        else if(button_clicked == 1)
        {
            editing = true;
            return false;
        }
    }

    return false;
}

// updates the interface state based on input and returns true if game should start
bool Interface::update(int button, int state, int x, int y, bool held)
{
    bool startgame = false;
    if (editing)
    {
        holding_point &= held;
        update_editor(button, state, x, y);
        track_preview->control = *points;
        track_preview->regenerate(true);
    }

    else
    {
        startgame = update_splashscreen(button, state, x, y);
    }

    return startgame;
}