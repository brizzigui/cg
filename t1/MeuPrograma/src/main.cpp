/*********************************************************************
// BIMP - Brizzi Image Manipulation Program
// Editor de imagens no estilo Photoshop usando a API gráfica Canvas 2D
// *********************************************************************/

#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl_canvas2d.h"

#include "canvas_pro.h"
#include "keyboard.h"
#include "side_menu.h"

//largura e altura inicial da tela . Alteram com o redimensionamento de tela.
int screenWidth = 1600, screenHeight = 900;
int mouseX, mouseY; //variaveis globais do mouse para poder exibir dentro da render().

Side_Menu *interface = NULL;
CVpro::image *img40px = NULL;
CVpro::image *img50px = NULL;


//funcao chamada continuamente. Deve-se controlar o que desenhar por meio de variaveis globais
//Todos os comandos para desenho na canvas devem ser chamados dentro da render().
//Deve-se manter essa fun��o com poucas linhas de codigo.
void render()
{
   CV::clear(0.1, 0.1, 0.1);

   // start->display();
   interface->display();
   // widget->display();
   // popups->display();

   Sleep(10);
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

   if (state == 0) // state == 0 é um clique qualquer em um dos botões
   {
      interface->update_state(button, x, y);

      // + updates virao aqui
   }
   
}

void create_actions()
{
   CVpro::image *pencil_icon = CVpro::load_bitmap("./MeuPrograma/images/pencil.bmp");
   interface->register_action("Pencil", pencil_icon, NULL, NULL);
   interface->register_action("Pen", pencil_icon, NULL, NULL);
   interface->register_action("Eraser", pencil_icon, NULL, NULL);
}

int main(void)
{
   interface = new Side_Menu(screenWidth, screenHeight);
   create_actions();

   /*---------------------------------------------------------------------------------*/
   CV::init(&screenWidth, &screenHeight, "BIMP - Brizzi Image Manipulation Program");
   CV::run();
}
