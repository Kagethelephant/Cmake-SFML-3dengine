#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include "data.hpp"
#include "matrix.hpp"
#include "polygon.hpp"
#include "obj3d.hpp"


/// @brief: Takes mesh data from 3D objects, projects the 3D polygons to a 2D view in the 
/// form of a pixel buffer to be drawn by and SFML window. This object is sf::drawable so it can be drawn
/// like other SFML objects. This will draw the projected 3D view generated with `update()`
/// @param res: Resolution of the desired output view 
/// @param fov: Field of view for camera in degrees
/// @param bgColor: Color to clear the pixel buffer with each frame
class camera : public sf::Drawable {

public:
   
   /// @brief: Position of the camera in 3D space
   vec3 position = vec3(0,0,-5);
   /// @brief: Direction that the camera is pointing in degrees about the rotational axis (updates direction)
   vec3 rotation = vec3(0,0,0);
   /// @brief: Direction the camera is pointing in the form of a vector
   vec3 pointDirection;

   camera(sf::Vector2u res, float fov = 90, sf::Color bgColor = sf::Color(ColorToHex(Color::Black)));

   /// @brief: Moves or rotates camera by given values (relative) movement is 
   /// based on the direction of the camera ( z moves forward/back, x moves sideways)
   /// @param x: Move sideways (normal to up and pointDirection of camera)
   /// @param y: Move in the up direction ( normal to the forward and right of the camera)
   /// @param x: Move in the direction of camera pointDirection
   /// @param u: Rotate around the right direction of the camera
   /// @param v: Rotate around the up direction of the camera
   /// @param w: Rotate around the pointDirection of the camera
   void move(float x, float y, float z, float u, float v, float w);

   /// @brief: Load the mesh of polygons from the 3D object and sort it with any other 
   /// in the triangle buffer by z value (camera z not world z)
   /// @param object: 3D object to load polygon mesh from
   void loadObject(object3d& object);

   /// @brief: Called externally to draw the triangles in the mesh 
   /// @param texture: std::vector of uint8_t representing a pixel array to be drawn to the screen
   /// @param tex: SFML texture to draw the pixel aray to 
   /// @param res: SFML 2d vector containing the resolution of the window
   /// @param camera: camera object that will be used to view the object to be drawn
   /// @param col: Color to draw the object (this will be the brightest color, 
   /// individual triangles will be shaded according to their orientation to the light)
   void update();

   /// @brief: Envoked by calling `sf::RenderTexture.draw(camera)` this draws the same as any
   void draw(sf::RenderTarget& target, sf::RenderStates states) const ;


private:

   /// @brief: Aspect ratio of the window
   float m_aspectRatio;     
   /// @brief: Resolution of the window 
   sf::Vector2u m_resolution;


   /// @brief: Makes an object rotate to point direction
   mat4x4 m_matPointAt; 
   /// @brief: Moves objects to position as if the origin was pointed a different direction
   mat4x4 m_matView; 
   /// @brief: Projects 3D points onto a 2D view
   mat4x4 m_matProject;

   /// @brief: virtual planes that represent the edge of the view frustum in 3d space
   vec4 m_planes[6];
  
   /// @brief: Stores triangles in 3D space from loaded objects to be drawn
   std::vector<tri3d> m_triangleBuffer;

   /// @brief: Texture to draw the pixelBuffer to so it can be drawn to an SFML window
   sf::Texture m_pixelTexture;
   /// @brief: Buffer to store the pixel data (place to draw triangles)
   std::vector<std::uint8_t> m_pixelBuffer;
   /// @brief: Buffer to clear the pixelBuffer with a background color
   std::vector<std::uint8_t> m_clearBuffer;
   /// @brief: Buffer to store the lowest z position to decide whether we should draw over it
   std::vector<float> m_zBuffer;
   
   std::vector<tri3d> clipTriangles(std::vector<tri3d> triangles);


   /// @brief: Clears the pixel buffer with the background color
   bool clearPixelBuffer();

   /// @brief: Checks if a point is on one side of a plane
   /// @param point: Point in 3d space
   /// @param plain: Plain in 3d space represented by its normal vecor
   bool pointOutOfPlane(vec3 point, vec4 plane);

   /// @brief: Used to get the position on a line betweeen 2 3d points where 
   /// that line intersects the given plane
   /// @param p1: Point in 3d space
   /// @param p2: 2nd point in 3d space to create a theoretical line with the 1st point
   /// @param plane: Plain in 3d space that intersects the theoretical line
   vec3 planeIntercect(vec3 p1, vec3 p2, vec4 plane);
};
