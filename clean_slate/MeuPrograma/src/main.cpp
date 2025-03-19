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
#include "canvas_pro.h"

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

#define KEYBOARD_PLUS 43
#define KEYBOARD_MINUS 45


//largura e altura inicial da tela . Alteram com o redimensionamento de tela.
int screenWidth = 1000, screenHeight = 500;
int mouseX, mouseY; //variaveis globais do mouse para poder exibir dentro da render().

CVpro::image *img_ptr = NULL;


//funcao chamada continuamente. Deve-se controlar o que desenhar por meio de variaveis globais
//Todos os comandos para desenho na canvas devem ser chamados dentro da render().
//Deve-se manter essa fun��o com poucas linhas de codigo.
void render()
{
   CV::clear(0.1, 0.1, 0.1);
   img_ptr->display_bitmap(100, 100, 1);
}

//funcao chamada toda vez que uma tecla for pressionada.
void keyboard(int key)
{
   printf("\nTecla: %d" , key);
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
   img_ptr = CVpro::load_bitmap("./MeuPrograma/src/test.bmp");
   CV::init(&screenWidth, &screenHeight, "Clean Slate");
   CV::run();
}
