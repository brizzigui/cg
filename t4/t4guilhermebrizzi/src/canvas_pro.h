/**
 * @file canvas_pro.h
 * @brief Declarations for the CVpro utility namespace for enhanced Canvas2D operations.
 *
 * Provides the CVpro namespace, which includes the image class and utility functions for drawing text, handling colors, and loading bitmaps. Designed to simplify and extend Canvas2D usage for graphics projects.
 */

// CVpro - t4

#ifndef CANVAS_PRO_H
#define CANVAS_PRO_H

#include "gl_canvas2d.h"
#include <stdarg.h>

typedef unsigned char subpixel;

namespace CVpro {
    /**
     * @brief Represents an RGBA image for drawing and displaying in Canvas2D.
     *
     * Provides methods for displaying and clearing the image buffer.
     */
    class image
    {
        public:
            subpixel *matrix; /**< Pointer to the image pixel data. */
            int width, height, bytes = 4; /**< Image dimensions and bytes per pixel. */
            /**
             * @brief Constructs an image with the given width and height.
             * @param width Width of the image.
             * @param height Height of the image.
             */
            image(int width, int height);
            /**
             * @brief Destructor for the image class.
             */
            ~image();

            /**
             * @brief Displays the image at the default position.
             */
            void display();
            /**
             * @brief Displays the image at the specified (x, y) position.
             * @param x X coordinate for the image position.
             * @param y Y coordinate for the image position.
             */
            void display(int x, int y);
            /**
             * @brief Clears the image buffer.
             */
            void clear();
    };
    
    /**
     * @brief Draws formatted text at the specified position.
     * @param x X coordinate.
     * @param y Y coordinate.
     * @param text Format string for the text.
     * @return None.
     */
    void text(float x, float y, const char *text, ...);
    /**
     * @brief Draws formatted text with alignment at the specified position.
     * @param x X coordinate.
     * @param y Y coordinate.
     * @param align Alignment character ('l', 'c', 'r').
     * @param text Format string for the text.
     * @return None.
     */
    void text_align(float x, float y, char align, const char *text, ...);
    /**
     * @brief Draws formatted text with alignment and spacing at the specified position.
     * @param x X coordinate.
     * @param y Y coordinate.
     * @param align Alignment character.
     * @param spacing Spacing between characters.
     * @param text Format string for the text.
     * @return None.
     */
    void autotext(float x, float y, char align, float spacing, const char *text, ...);
    /**
     * @brief Sets the current drawing color (RGB).
     * @param r Red component (0-255).
     * @param g Green component (0-255).
     * @param b Blue component (0-255).
     */
    void color(int r, int g, int b);
    /**
     * @brief Sets the current drawing color (RGBA).
     * @param r Red component (0-255).
     * @param g Green component (0-255).
     * @param b Blue component (0-255).
     * @param a Alpha component (0-255).
     */
    void color(int r, int g, int b, int a);
    /**
     * @brief Loads a bitmap image from the specified file path.
     * @param path Path to the .bmp file.
     * @return Pointer to the loaded image object.
     */
    CVpro::image *load_bitmap(const char *path);

}

#endif
