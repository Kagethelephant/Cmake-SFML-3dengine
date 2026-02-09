#pragma once

#include <gpuRender/window.hpp>
#include <cstdint>
#include "utils/matrix.hpp"
#include "app/object.hpp"


enum class ClipPlane {
    Left,
    Right,
    Bottom,
    Top,
    Near,
    Far
};

/// @brief: Takes mesh data from 3D objects, projects the 3D polygons to a 2D view in the 
/// form of a pixel buffer to be drawn by and SFML window.
/// @param res: Reference to the window to be rendered to. Used to find resolution and SFML render target
class camera {

public:
   
   /// @brief: Position of the camera in 3D space
   vec3 camPosition = vec3(0,0,0);
   /// @brief: Direction that the camera is pointing in degrees about the rotational axis (updates direction)
   vec3 camRotation = vec3(0,0,0);
   /// @brief: Direction the camera is pointing in the form of a vector
   vec3 camDirection;

   /// @brief: Light position in world space (only supports one light)
   vec3 lightPos = vec3(0,0,0);
   vec3 lightPosView;
   /// @brief: Light color that is multiplied by the color of the object it is illuminating
   vec3 lightCol = vec3(1,1,1);

   /// @brief: Reference to the sfml window that we will render to
   gl_window& window;

   camera(gl_window& _window);

   /// @brief: Moves camera by given values (relative) movement is 
   /// based on the direction of the camera ( z moves forward/back, x moves sideways)
   /// @param x: Move sideways (normal to up and pointDirection of camera)
   /// @param y: Move in the up direction ( normal to the forward and right of the camera)
   /// @param x: Move in the direction of camera pointDirection
   void move(float x, float y, float z);

   /// @brief: Rotates camera by given values (relative) movement is 
   /// based on the direction of the camera ( z moves forward/back, x moves sideways)
   /// @param u: Rotate around the right direction of the camera
   /// @param v: Rotate around the up direction of the camera
   /// @param w: Rotate around the pointDirection of the camera
   void rotate(float u, float v, float w);

   /// @brief: Render 3D vertex data given information from 3D object. This is where most of the
   /// 3D graphics pipeline is excecuted: vertex shader, vertex post processing (triangle clipping)
   /// @param object: Object that provides position, orientation, scale, color and model index information
   void render(const object& object);

   /// @brief: Draw pixel buffer to the sfml window passed to this object
   void draw();

   // /// @brief: Model vertex (really triangles) data locations in the triangle buffer
   // struct model {
   //    /// @brief: location of first triangle in triangle buffer
   //    unsigned int start;
   //    /// @brief: size in triangles of the model
   //    unsigned int size;
   //    /// @brief: location of first triangle in triangle buffer
   //    unsigned int vertStart;
   //    /// @brief: size in triangles of the model
   //    unsigned int vertSize;
   // };

   /// @brief: Pixel array for 32 bit trucolor + alpha (8 bits for r,g,b and alpha) used to raster triangles
   std::vector<std::uint8_t> m_pixelBuffer;
   /// @brief: An array of the models stored in the triangle buffer
   std::vector<model> models;

   /// @brief: Loads vertex data into the triangle buffer from an OBJ file and saves the 
   /// location with a model object 
   /// @param filename: filepath to the OBJ file
   /// @param ccwWinding: changes the winding on the model so the triangle normal points outwards
   /// @return: uint location of the model in the models array
   unsigned int createModel (const std::string filename, const bool ccwWinding = false);

private:

   /// @brief: Aspect ratio of the window
   float m_aspectRatio;     
   /// @brief: Resolution of the window 
   vec2 m_resolution;

   //OPENGL MIRRORED ITEMS
   std::vector<float> vertices;
   std::vector<unsigned int> indices;
   //OPENGL MIRRORED ITEMS

   /// @brief: Makes an object rotate to point direction
   mat4x4 m_matPointAt; 
   /// @brief: Moves objects to position as if the origin was pointed a different direction
   mat4x4 m_matView; 
   /// @brief: Projects 3D points onto a 2D view
   mat4x4 m_matProject;

   /// @brief: virtual planes that represent the edge of the view frustum in 3d space
   vec4 m_planes[6];
   float far;
  

   struct triangleAttrib {

      triangleAttrib(tri3d pos, tri3d col, tri3d clip, tri3d frag) : triangle{pos}, color{col}, clipPos{clip}, fragPos{frag} {};
      tri3d triangle;
      tri3d color;
      tri3d clipPos;
      tri3d fragPos;
   };
   /// @brief: Stores triangle attributes (in this case just color, in openGL this would include normal, color, UV coords)
   std::vector<triangleAttrib> m_triangleAttribs;

   /// @brief: Stores triangles in 3D space from loaded objects to be drawn
   std::vector<unsigned int> m_indices;
   /// @brief: Stores triangles in 3D space from loaded objects to be drawn
   std::vector<float> m_vertices;

   // /// @brief: Texture to draw the pixelBuffer to so it can be drawn to an SFML window
   // sf::Texture m_pixelTexture;
   /// @brief: Buffer to clear the pixelBuffer with a background color
   std::vector<std::uint8_t> m_clearBuffer;
   /// @brief: Buffer to store the lowest z position of each pixel to decide whether we should draw over it
   std::vector<float> m_zBuffer;


   std::vector<vec4> viewVertices;

   std::vector<vec4> clipVertices;

   /// @brief: Scanline triangle fill algorithm. This is a common method of rasterization although it is very inefficient
   /// when ran on the CPU as we are doing here. steps in pipeline: Rasterization, Fragment Shader
   /// @param tri: Pixel array for 32 bit trucolor + alpha (8 bits for r,g,b and alpha)
   /// @param res: The resolution of the window
   void raster(const camera::triangleAttrib& attrib);
   
   /// @brief: In-place clipping of triangles in m_triangleAttribs against all 6 planes 
   /// planes in clip space (after projection, before perspective division)
   void clipTriangles();

   /// @brief: Checks the given triangle for winding in screen space
   /// @param tri: triangle to check for culling
   bool backFaceCulling(const tri3d& tri);

   /// @brief: Checks if a point is on one side of a plane
   /// @param point: Point in 3d space
   /// @param plain: Plain in 3d space represented by its normal vecor
   bool pointOutOfPlane(const vec4& p, const vec4& plane);

   /// @brief: Used to get the position on a line betweeen 2 3d points where 
   /// that line intersects the given plane
   /// @param p1: Point in 3d space
   /// @param p2: 2nd point in 3d space to create a theoretical line with the 1st point
   /// @param plane: Plain in 3d space that intersects the line
   /// @return: float t value representing percent of the line where intersectet[0 - 1]
   float planeIntersect(const vec4& a, const vec4& b, const vec4& plane);

   /// @brief: Checks if a point is on one side of a plane
   /// @param point: Point in 3d space
   /// @param plain: Plain in 3d space represented by its normal vecor
   float edgeFunction(const vec2& a, const vec2& b, const vec2& p);
};
