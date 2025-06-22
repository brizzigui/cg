/**
*   Programa para ilustrar:
*   - Uso da gluLookAt
*   - Pilha de transformacoes
*   - Projecao pespectiva
*   - Interacao com o mouse/teclado
*   - cullFace, frontFace

*   Autor: Cesar Tadeu Pozzer
*   UFSM - 15/06/2007
*
*   pozzer@inf.ufsm.br
*   pozzer3@gmail.com
*
**/

#include <GL/glut.h>

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include "Vector3.h"
#include "asteroid.h"
#include "camera.h"
#include "controller.h"

#define SCREEN_X 1280
#define SCREEN_Y 720

float rx = 0, rz = 0; //parametros de rotacao do objeto.

float abertura = 60.0;
float znear  = 1;
float zfar   = 100000;
float aspect = (float)SCREEN_X/(float)SCREEN_Y;

void setup_opengl()
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity( );
   gluPerspective(abertura, aspect, znear, zfar);
   glMatrixMode(GL_MODELVIEW);

   glClearColor(0, 0, 0, 1);

   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

   glEnable(GL_DEPTH_TEST);
}

void update_lighting()
{
   // Lighting setup
   GLfloat light_pos[] = {0.0f, 0.0f, 0.0f, 1.0f}; // positional light
   GLfloat light_amb[] = {0.2f, 0.2f, 0.2f, 1.0f};
   GLfloat light_dif[] = {0.8f, 0.8f, 0.8f, 1.0f};
   GLfloat light_spe[] = {1.0f, 1.0f, 1.0f, 1.0f};

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
   glLightfv(GL_LIGHT0, GL_AMBIENT,  light_amb);
   glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_dif);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_spe);

   glEnable(GL_COLOR_MATERIAL);
   glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void init()
{
   setup_opengl();
   update_lighting();
}

Controller controller;
std::vector<Asteroid> asteroids;
Camera camera;
float factor = 1.0;

void clear()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void configure_camera()
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   gluPerspective(abertura, aspect, znear, zfar);

   glMatrixMode(GL_MODELVIEW);

   Vector3 target = camera.pos + camera.direction;
   glLoadIdentity();
   gluLookAt(camera.pos.x, camera.pos.y, camera.pos.z,   //from. Posicao onde a camera esta posicionada
             target.x, target.y, target.z,               //to. Posicao absoluta onde a camera esta vendo
             camera.up.x, camera.up.y, camera.up.z);                                   //up. Vetor Up.
}

////////////////////////////////////////////////////////////////////////////////////////
void display(void)
{
   controller.start();

   clear();
   configure_camera();
   camera.tick(factor);
   for (int i = 0; i < (int)asteroids.size(); i++)
   {
      asteroids[i].draw(camera.pos);   
      asteroids[i].tick();
   }

   glutSwapBuffers();

   factor = controller.end();
}

void create_asteroids()
{
   for (int i = 0; i < 5000; i++)
   {
      asteroids.push_back(Asteroid());
   }
}

//faz a leitura da entrada do usuario
void keyboard_down(unsigned char key, int x, int y)
{
   camera.key_down(key);
}

//faz a leitura da entrada do usuario
void keyboard_up(unsigned char key, int x, int y)
{
   camera.key_up(key);
}

void MotionFunc(int x, int y)
{
}

void MouseFunc(int botao, int estado, int x, int y)
{
}

////////////////////////////////////////////////////////////////////////////////////////

void glut_set_params()
{
   int argc = 0;
   glutInit(&argc, NULL);

   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

   glutInitWindowSize (SCREEN_X, SCREEN_Y);
   glutInitWindowPosition (450, 10);
   /* create window */
   glutCreateWindow ("Project Stardust");
   init();

   glutDisplayFunc(display);
   glutMotionFunc(MotionFunc);
   glutMouseFunc(MouseFunc);
   glutIdleFunc(display);
   glutKeyboardFunc(keyboard_down);
   glutKeyboardUpFunc(keyboard_up);
}

void start_glut()
{
   glutMainLoop();
}

void start_music()
{
   PlaySound(TEXT("./t5guilhermebrizzi/assets/imperial_march.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
}

int main()
{ 
   srand(time(NULL));
   glut_set_params();
   controller = Controller(60, false);
   create_asteroids();
   start_music();
   start_glut();
   return 0;
}
