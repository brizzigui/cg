/*********************************************************************
// BIMP - Brizzi Image Manipulation Program
// Editor de imagens no estilo Photoshop usando a API gráfica Canvas 2D
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
#include "interface.h"
#include "layer_manager.h"
#include "editor.h"
#include "widget.h"

//largura e altura inicial da tela . Alteram com o redimensionamento de tela.
int screenWidth = 1280, screenHeight = 720;
int mouseX, mouseY; //variaveis globais do mouse para poder exibir dentro da render().

Interface *interface = NULL;
Layer_Manager *layer_manager = NULL;
Editor *editor = NULL;
Widget *widget = NULL;

bool mouse_held = false;

//funcao chamada continuamente. Deve-se controlar o que desenhar por meio de variaveis globais
//Todos os comandos para desenho na canvas devem ser chamados dentro da render().
//Deve-se manter essa fun��o com poucas linhas de codigo.
void render()
{
   auto start = std::chrono::high_resolution_clock::now();

   CV::clear(0.1, 0.1, 0.1);
   layer_manager->display();
   interface->display();
   widget->display();

   auto stop = std::chrono::high_resolution_clock::now();
   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
   //std::cout << 1000.0 / duration.count() << std::endl;
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

   // printf("\nmouse %d %d %d %d %d %d", button, state, wheel, direction, x, y);
   editor->update_state(button, x, y, mouse_held);

   if (state == 0) // state == 0 é um clique qualquer em um dos botões
   {
      interface->update_state(button, x, y);
      widget->update_state(button, x, y);
      mouse_held = true;
   }

   else if (state == 1)
   {
      mouse_held = false;
   }
}


int main(void)
{
   layer_manager = new Layer_Manager();
   layer_manager->add_bmp_layer("./MeuPrograma/images/seville.bmp");
   layer_manager->add_blank_layer();
   layer_manager->add_blank_layer();
   
   interface = new Interface(screenWidth, screenHeight, layer_manager);
   interface->create_default_actions();

   editor = new Editor(layer_manager, interface);

   // async for flatten() function (layer blending, check layer_manager.h for more info)
   std::thread(&Layer_Manager::flatten_worker, layer_manager).detach();

   widget = new Widget(layer_manager, interface, editor);

   /*---------------------------------------------------------------------------------*/
   CV::init(&screenWidth, &screenHeight, "BIMP - Brizzi Image Manipulation Program");
   CV::run();
}
