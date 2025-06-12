/**
 * @file preview.h
 * @brief Declarations for the Preview class and related types for 3D model visualization.
 *
 * Provides the Preview class interface, which manages rendering, user interaction, and visualization modes for 3D models. Also defines the Triangle struct and related constants.
 */
#ifndef PREVIEW_H
#define PREVIEW_H

#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "canvas_pro.h"
#include "color.h"

/**
 * @brief Wireframe rendering mode constant.
 */
#define WIREFRAME_MODE 0
/**
 * @brief Pixel lighting rendering mode constant.
 */
#define PIXEL_LIGHTING_MODE 1

/**
 * @brief Default color used for painting.
 */
#define DEFAULT_COLOR Color(241, 120, 38, 255)

/**
 * @brief Represents a triangle in 3D space.
 *
 * Holds three vertices (va, vb, vc) as Vector3 objects.
 */
struct Triangle
{
    Vector3 va, vb, vc;
    /**
     * @brief Constructs a Triangle with three vertices.
     * @param va First vertex.
     * @param vb Second vertex.
     * @param vc Third vertex.
     */
    Triangle(Vector3 va, Vector3 vb, Vector3 vc)
    {
        this->va = va;
        this->vb = vb;
        this->vc = vc;
    }
};

/**
 * @brief Handles 3D model preview rendering, interaction, and visualization modes.
 *
 * Manages the rendering pipeline, user input, and various display modes for 3D models generated from Bezier curves.
 */
class Preview
{
    private:
        int mode = WIREFRAME_MODE; /**< Current rendering mode. */

        CVpro::image *result = NULL; /**< Pointer to the result image. */

        std::vector<Vector2> *R2_points; /**< Pointer to 2D points vector. */
        std::vector<Triangle> R2_projections; /**< 2D projections of the triangles. */
        std::vector<Triangle> R3_triangles; /**< 3D triangles for the model. */

        float anchorX, anchorY; /**< Anchor points for manipulation. */
        bool held = false; /**< Indicates if the model is being manipulated. */
        float grabX, grabY; /**< Grab coordinates for manipulation. */
        float width, height; /**< Dimensions of the preview area. */
        float padding; /**< Padding around the preview area. */

        float dist = 3000.0; /**< Distance for perspective projection. */
        float object_pos = dist; /**< Object position in the scene. */
        float scale = 1.0; /**< Scale factor for the model. */

        int slices = 32; /**< Number of slices for the model. */
        bool grow_y = false; /**< Indicates if the model grows in the Y direction. */
        bool perspective = true; /**< Enables or disables perspective projection. */
        bool show_normal = false; /**< Indicates if normals are displayed. */

        std::vector<float> zbuffer; /**< Z-buffer for depth testing. */
        Color paint_color = DEFAULT_COLOR; /**< Color used for painting. */

        std::vector<CVpro::image *> icons; /**< UI icons for interaction. */
        float button_anchorX, button_anchorY, button_size; /**< Button UI properties. */

        /**
         * @brief Projects a 3D point to 2D.
         * @param point 3D point to project.
         * @param d Distance from the camera.
         * @return Projected 2D point.
         */
        Vector3 project(Vector3 point, float d);
        /**
         * @brief Rotates Bezier points to form a surface.
         * @return A vector of rotated 3D points.
         */
        std::vector<std::vector<Vector3>> rotate_bezier();
        /**
         * @brief Generates triangles from the surface.
         */
        void triangularize();
        /**
         * @brief Flattens the 3D model for display.
         */
        void flatten();
        /**
         * @brief Paints the wireframe result.
         */
        void paint_result();
        /**
         * @brief Draws a line in the result image.
         * @param x0 X coordinate of the start point.
         * @param y0 Y coordinate of the start point.
         * @param x1 X coordinate of the end point.
         * @param y1 Y coordinate of the end point.
         */
        void line_to_bmp(int x0, int y0, int x1, int y1);
        /**
         * @brief Sets a pixel in the result image.
         * @param x X coordinate of the pixel.
         * @param y Y coordinate of the pixel.
         */
        void set_pixel(int x, int y);
        /**
         * @brief Projects triangles to wireframe.
         */
        void triangle_to_wireframe();
        /**
         * @brief (Unused) Vertex lighting rendering.
         */
        void triangle_to_vertex_lighting();
        /**
         * @brief Renders triangles with per-pixel lighting.
         */
        void triangle_to_pixel_lighting();
        /**
         * @brief Computes lighting at a vertex.
         * @param normal Vertex normal.
         * @param light_dir Direction of the light source.
         * @return Computed color at the vertex.
         */
        Color compute_vertex_lighting(Vector3 normal, Vector3 light_dir);
        /**
         * @brief Draws the preview background.
         */
        void draw_background();
        /**
         * @brief Rotates the model.
         * @param roll Rotation around the X axis.
         * @param pitch Rotation around the Y axis.
         * @param yaw Rotation around the Z axis.
         */
        void rotate(float roll, float pitch, float yaw);
        /**
         * @brief Gets rotation vector from mouse movement.
         * @param x X coordinate of the mouse.
         * @param y Y coordinate of the mouse.
         * @return Rotation vector.
         */
        Vector3 obtain_rotation(int x, int y);
        /**
         * @brief Draws UI buttons.
         */
        void draw_buttons();
        /**
         * @brief Checks if a button was clicked.
         * @param button Mouse button index.
         * @param state Mouse button state.
         * @param x X coordinate of the mouse.
         * @param y Y coordinate of the mouse.
         * @return Button ID if clicked, -1 otherwise.
         */
        int check_buttons(int button, int state, int x, int y);
        /**
         * @brief Handles model manipulation input.
         * @param button Mouse button index.
         * @param state Mouse button state.
         * @param direction Scroll direction for zoom.
         * @param x X coordinate of the mouse.
         * @param y Y coordinate of the mouse.
         */
        void handle_model_manipulation(int button, int state, int direction, int x, int y);
        /**
         * @brief Handles UI input.
         * @param button Mouse button index.
         * @param state Mouse button state.
         * @param x X coordinate of the mouse.
         * @param y Y coordinate of the mouse.
         */
        void handle_ui_input(int button, int state, int x, int y);
        /**
         * @brief Draws normals for triangles.
         */
        void calculate_normals();
        /**
         * @brief Draws info text.
         */
        void draw_info();
        
    public:
        /**
         * @brief Constructs a Preview object.
         * @param points Pointer to a vector of Vector2 points.
         * @param screen_height Height of the screen.
         * @param screen_width Width of the screen.
         */
        Preview(std::vector<Vector2> *points, float screen_height, float screen_width);
        /**
         * @brief Destructor for the Preview class.
         */
        ~Preview();

        /**
         * @brief Draws the preview area and its contents.
         */
        void draw();
        /**
         * @brief Updates the preview based on user input.
         * @param button Mouse button index.
         * @param state Mouse button state.
         * @param direction Scroll direction for zoom.
         * @param x X coordinate of the mouse.
         * @param y Y coordinate of the mouse.
         */
        void update(int button, int state, int direction, int x, int y);
        /**
         * @brief Recreates the model and redraws the preview.
         */
        void recreate();
        /**
         * @brief Recreates the model with rotation applied.
         * @param roll Rotation around the X axis.
         * @param pitch Rotation around the Y axis.
         * @param yaw Rotation around the Z axis.
         */
        void recreate(float roll, float pitch, float yaw);
};

#endif