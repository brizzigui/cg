/*********************************************************************
// GTA 2D - Trabalho 3 de Computação Gráfica
// Guilherme Brizzi
// Jogo arcade de tiro e combate
// 
// Algumas imagens são contribuições de Kenney Vleugels (www.kenney.nl)
// pela licensa CC0
// 
// *********************************************************************/

#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>

#include "gl_canvas2d.h"
#include "canvas_pro.h"
#include "keyboard.h"
#include "controller.h"
#include "event.h"
#include "special_events.h"
#include "simulation.h"
#include "tank.h"
#include "footprint.h"
#include "track.h"

Controller *controller = NULL;
Simulation *simulation = NULL;

int screenWidth = 1280, screenHeight = 720;
bool held = false;

void render()
{
   controller->start();

   CV::clear(0.1, 0.1, 0.1);
   simulation->update();
   simulation->display();

   controller->end();
}

void keyboard(int key)
{
   //printf("\nTecla: %d" , key);
   simulation->add_event(new Event_Key_Down(key));
}

void keyboardUp(int key)
{
   // printf("\nLiberou: %d" , key);
   simulation->add_event(new Event_Key_Up(key));
}

void mouse(int button, int state, int wheel, int direction, int x, int y)
{
   held = ((held && state != 1) || state == 0) ? true : false;
   simulation->add_event(new Event_Mouse(button, state, wheel, direction, x, y, held));
}

std::vector<std::vector<Vector2>> generate_default_track_points()
{
   std::vector<std::vector<Vector2>> trackCurves = {
      // Outer Top
      { Vector2(200, 200), Vector2(400, 100), Vector2(880, 100), Vector2(1080, 200) },
      // Outer Right
      { Vector2(1080, 200), Vector2(1180, 300), Vector2(1180, 420), Vector2(1080, 520) },
      // Outer Bottom
      { Vector2(1080, 520), Vector2(880, 620), Vector2(400, 620), Vector2(200, 520) },
      // Outer Left
      { Vector2(200, 520), Vector2(100, 420), Vector2(100, 300), Vector2(200, 200) },

      // Inner Top
      { Vector2(400, 300), Vector2(500, 250), Vector2(780, 250), Vector2(880, 300) },
      // Inner Right
      { Vector2(880, 300), Vector2(930, 350), Vector2(930, 370), Vector2(880, 420) },
      // Inner Bottom
      { Vector2(880, 420), Vector2(780, 470), Vector2(500, 470), Vector2(400, 420) },
      // Inner Left
      { Vector2(400, 420), Vector2(350, 370), Vector2(350, 350), Vector2(400, 300) }
   };

   return trackCurves;
}

int main(void)
{
   srand(time(NULL));
   Footprint::width = screenWidth;
   Footprint::height = screenHeight;

   controller = new Controller(60, true);
   simulation = new Simulation(screenWidth, screenHeight);
   
   auto points = generate_default_track_points();
   simulation->add_entity(new Track(0, 0, points));
   simulation->add_entity(new Tank(300, 300, Vector2(PI/4), 1));

   /*---------------------------------------------------------------------------------*/
   CV::init(&screenWidth, &screenHeight, "GTA 2D");
   CV::run();
}
