/*********************************************************************
// GTA 2D - Trabalho 3 de Computação Gráfica
// Guilherme Brizzi
// Jogo arcade de tiro e combate
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

int main(void)
{
   srand(time(NULL));
   Footprint::width = screenWidth;
   Footprint::height = screenHeight;

   controller = new Controller(60, true);
   simulation = new Simulation(screenWidth, screenHeight);
   simulation->add_entity(new Tank(200, 100, Vector2(PI/4), 1));

   /*---------------------------------------------------------------------------------*/
   CV::init(&screenWidth, &screenHeight, "GTA 2D");
   CV::run();
}
