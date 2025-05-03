#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <iostream>
#include <string.h>

/*
    Constantes de teclado para uso fácil da Canvas
*/

namespace CVkeyboard
{
    void decode(char *buffer, int key)
    {
        int size = strlen(buffer);
        if (key >= 45 && key <= 122)
        {
            buffer[size] = (char)key;
            buffer[size+1] = '\0';
        }

        else if (key == 8)
        {
            if (size > 0)
            {
                buffer[size-1] = '\0';            
            }
        }

        else
        {
            std::cout << "Keyboard log: Key pressed unavaliable with code " << key << std::endl;
        }
    } 
}

#endif