/*********************************************************************
// T4 - Modelador 3D em wireframe
*********************************************************************/

#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.
#include "gl_canvas2d.h"

#include <vector>

#include "Vector2.h"
#include "Vector3.h"
#include "preview.h"
#include "editor.h"

Editor *editor = NULL;
Preview *preview = NULL;

bool held = false;
int screenWidth = 1280, screenHeight = 720;

void render()
{
   CV::clear(0, 0, 0);
   editor->draw();
   preview->draw();
   Sleep(10);
}

void keyboard(int key)
{

}

void keyboardUp(int key)
{

}

void mouse(int button, int state, int wheel, int direction, int x, int y)
{
   if (button == 0 && state == 0)
   {
      held = true;
   }

   else if (button == 0 && state == 1)
   {
      held = false;
   }
   
   bool controlchange = editor->update(button, state, x, y, held);
   if (controlchange)
   {
      editor->regenerate_curve();
      preview->recreate();
   }

   preview->update();
}

int main(void)
{
   std::vector<Vector2> points;
   editor = new Editor(&points, screenHeight, screenWidth);
   preview = new Preview(&points, screenHeight, screenWidth);

   CV::init(&screenWidth, &screenHeight, "BIMP 3D");
   CV::run();
}
