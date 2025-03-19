/*********************************************************************
// Canvas para desenho, criada sobre a API OpenGL. Nao eh necessario conhecimentos de OpenGL para usar.
//  Autor: Cesar Tadeu Pozzer
//         02/2025
//
//  Pode ser utilizada para fazer desenhos, animacoes, e jogos simples.
//  Tem tratamento de mouse e teclado
//  Estude o OpenGL antes de tentar compreender o arquivo gl_canvas.cpp
//
//  Versao 2.1
//
//  Instru��es:
//	  Para alterar a animacao, digite numeros entre 1 e 3
// *********************************************************************/

#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl_canvas2d.h"
#include "Bezier_Curve.h"

#define KEYBOARD_LEFT 200
#define KEYBOARD_TOP 201
#define KEYBOARD_RIGHT 202
#define KEYBOARD_DOWN 203

#define KEYBOARD_1 49
#define KEYBOARD_2 50
#define KEYBOARD_3 51
#define KEYBOARD_4 52
#define KEYBOARD_5 53
#define KEYBOARD_6 54
#define KEYBOARD_7 55
#define KEYBOARD_8 56
#define KEYBOARD_9 57


//largura e altura inicial da tela . Alteram com o redimensionamento de tela.
int screenWidth = 500, screenHeight = 500;
int mouseX, mouseY; //variaveis globais do mouse para poder exibir dentro da render().

Bezier_Curve *curve = NULL;
std::vector<std::pair<double, double>> points;

int selected = 0;

void draw_points()
{
   for (int i = 0; i < points.size(); i++)
   {
      auto v = points[i];
      if (i == selected-1)
      {
         CV::color(0, 0, 1);
      }
      
      else
      {
         CV::color(1, 1, 1);
      }

      CV::circleFill(v.first, v.second, 5, 10);   
   }
}


void apply_move(int key)
{
   switch (key)
   {
      case KEYBOARD_1:
         selected = 1;
         break;

      case KEYBOARD_2:
         selected = 2;
         break;

      case KEYBOARD_3:
         selected = 3;
         break;

      case KEYBOARD_4:
         selected = 4;
         break;

      case KEYBOARD_5:
         selected = 5;
         break;

      case KEYBOARD_6:
         selected = 6;
         break;

      case KEYBOARD_7:
         selected = 7;
         break;

      case KEYBOARD_8:
         selected = 8;
         break;

      case KEYBOARD_9:
         selected = 9;
         break;
      
      default:
         break;
   }

   if(selected != 0)
   {
      switch (key)
      {
         case KEYBOARD_LEFT:
            points[selected-1].first -= 5;
            break;

         case KEYBOARD_TOP:
            points[selected-1].second += 5;
            break;

         case KEYBOARD_RIGHT:
            points[selected-1].first += 5;
            break;

         case KEYBOARD_DOWN:
            points[selected-1].second -= 5;
            break;
         
         default:
            break;
      }
   }
}

//funcao chamada continuamente. Deve-se controlar o que desenhar por meio de variaveis globais
//Todos os comandos para desenho na canvas devem ser chamados dentro da render().
//Deve-se manter essa fun��o com poucas linhas de codigo.
void render()
{
   CV::clear(0.1, 0.1, 0.1);
   curve->draw();
   draw_points();
}

//funcao chamada toda vez que uma tecla for pressionada.
void keyboard(int key)
{
   printf("\nTecla: %d" , key);
   apply_move(key);
}

//funcao chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
   printf("\nLiberou: %d" , key);
}

//funcao para tratamento de mouse: cliques, movimentos e arrastos
void mouse(int button, int state, int wheel, int direction, int x, int y)
{
   mouseX = x; //guarda as coordenadas do mouse para exibir dentro da render()
   mouseY = y;

   printf("\nmouse %d %d %d %d %d %d", button, state, wheel, direction, x, y);
}

int main(void)
{
   points = {std::make_pair(50, 50), std::make_pair(30, 150), std::make_pair(300, 50), std::make_pair(200, 150)};
   curve = new Bezier_Curve(&points);

   CV::init(&screenWidth, &screenHeight, "Curva de Bezier");
   CV::run();
}
