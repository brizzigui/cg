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
#include "popup.h"
#include "start.h"

//largura e altura inicial da tela . Alteram com o redimensionamento de tela.
int screenWidth = 1280, screenHeight = 720;

Start *start = NULL;
Interface *interface = NULL;
Layer_Manager *layer_manager = NULL;
Editor *editor = NULL;
Widget *widget = NULL;
Popup *popup = NULL;

bool mouse_held = false;

//funcao chamada continuamente. Deve-se controlar o que desenhar por meio de variaveis globais
//Todos os comandos para desenho na canvas devem ser chamados dentro da render().
//Deve-se manter essa fun��o com poucas linhas de codigo.
void render()
{
   CV::clear(0.1, 0.1, 0.1);
   
   if (start->is_open)
   {
      start->display();
   }
   
   else
   {
      layer_manager->display();
      interface->display();
      widget->display();
      popup->display();
   }

   Sleep(0);
}

//funcao chamada toda vez que uma tecla for pressionada.
void keyboard(int key)
{
   // printf("\nTecla: %d" , key);
   popup->listen(key);
   start->listen(key);
}

//funcao chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
   // printf("\nLiberou: %d" , key);
}

//funcao para tratamento de mouse: cliques, movimentos e arrastos
void mouse(int button, int state, int wheel, int direction, int x, int y)
{
   if (state == 0) // state == 0 é um clique qualquer em um dos botões
   {
      mouse_held = true;
   }

   else if (state == 1)
   {
      mouse_held = false;
   }

   //printf("\nmouse %d %d %d %d %d %d", button, state, wheel, direction, x, y);
   if (popup->is_open)
   {
      popup->update(state, button, x, y, mouse_held);
   }

   else if (start->is_open)
   {
      start->update(state, button, x, y);
   }
   
   else
   {
      interface->update_state(state, button, x, y);   
      widget->update_state(state, button, x, y, mouse_held);
      editor->update_state(state, button, x, y, mouse_held);
   }
}

int main(void)
{
   std::cout << "BIMP is starting." << std::endl;
   srand(time(NULL));

   popup = new Popup(screenWidth, screenHeight);
   layer_manager = new Layer_Manager();
   start = new Start(screenWidth, screenHeight, layer_manager);
   
   interface = new Interface(screenWidth, screenHeight, layer_manager, popup);
   interface->create_default_actions();

   editor = new Editor(layer_manager, interface);
   widget = new Widget(layer_manager, interface, editor, popup);
   popup->give_root_access(layer_manager);

   // async for flatten() function (layer blending, check layer_manager.h for more info)
   std::thread(&Layer_Manager::flatten_worker, layer_manager).detach();

   std::cout << "Opening BIMP UI." << std::endl;

   /*---------------------------------------------------------------------------------*/
   CV::init(&screenWidth, &screenHeight, "BIMP - Brizzi Image Manipulation Program");
   CV::run();

   std::cout << std::endl;
}
