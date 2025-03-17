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
#include "Pacman.h"
#include "Food.h"

#define KEYBOARD_R 114

//largura e altura inicial da tela . Alteram com o redimensionamento de tela.
int screenWidth = 500, screenHeight = 500;
int mouseX, mouseY; //variaveis globais do mouse para poder exibir dentro da render().

Pacman *pacman = NULL;
Food *food = NULL;

int score = 0;
int highscore;
bool gameover = false;

void restart()
{
   gameover = false;
   score = 0;
   pacman->restart();
   food->restart();
}

void draw_score()
{
   CV::color(1, 1, 1);
   char tmp[32];
   sprintf(tmp, "SCORE: %03d", score);
   CV::text(360, 480, tmp);
}

void write_score()
{   
   if(score > highscore)
   {
      FILE *file = fopen("highscore.txt", "w");
      fprintf(file, "%d", score);
      highscore = score;
   }
}

int get_highscore()
{
   FILE *file = fopen("highscore.txt", "r");
   fscanf(file, "%d", &highscore);

   return highscore;
}

void draw_gameover()
{
   CV::color(1, 1, 1);
   CV::text(200, 350, "GAMEOVER!");
   CV::text(180, 230, "Tente de novo!");

   char string[32];
   sprintf(string, "Highscore: %03d pts", highscore);
   CV::text(155, 210, string);

   CV::text(135, 150, "Clique 'R' para iniciar");
}

//funcao chamada continuamente. Deve-se controlar o que desenhar por meio de variaveis globais
//Todos os comandos para desenho na canvas devem ser chamados dentro da render().
//Deve-se manter essa fun��o com poucas linhas de codigo.
void render()
{
   CV::clear(0.1, 0.1, 0.1);
   CV::color(1, 1, 1);
   CV::text(20, 480, "SNAKE PACMAN");
   draw_score();

   if(gameover)
   {
      draw_gameover();
      return;
   }

   pacman->animate();
   std::pair<bool, bool> result = pacman->eat(food);

   if(result.first){
      score++;           
   }

   if (result.second){
      gameover = true;
      highscore = get_highscore();
      write_score();
   }

   food->draw_food();

   Sleep(10); //nao eh controle de FPS. Somente um limitador de FPS.
}

//funcao chamada toda vez que uma tecla for pressionada.
void keyboard(int key)
{
   printf("\nTecla: %d" , key);
   pacman->change_dir(key);

   if(key == KEYBOARD_R)
   {
      restart();
   }
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

   printf("\nmouse %d %d %d %d %d %d", button, state, wheel, direction,  x, y);
}

int main(void)
{
   srand(time(NULL));
   // args are (initial x, initial y, speed, ptrs to screen dimensions)
   pacman = new Pacman(screenWidth/2, screenHeight/2, 1.5, &screenWidth, &screenHeight);
   food = new Food(&screenWidth, &screenHeight);
   food->generate_food();

   CV::init(&screenWidth, &screenHeight, "Pacman demo");
   CV::run();
}
