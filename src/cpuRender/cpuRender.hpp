#pragma once

#include <gpuRender/window.hpp>
#include <cstdint>
#include "utils/matrix.hpp"
#include "app/object.hpp"



struct prim {

public:

   vertex v[3];


   prim() : v{vertex(),vertex(),vertex()}{};
   prim(vertex v0, vertex v1, vertex v2) : v{v0,v1,v2}{};

   // @brief: Generates a vector normal to the triangles face starting from the triangles 0 point
   vec4 normal() const {return ((v[1].fragPos - v[0].fragPos).cross(v[2].fragPos - v[0].fragPos)).normal();; }
   // @brief: Devide by the w value (viewspace z value) after projection to give perspective, making far away objects look smaller
   void perspectiveDivide() {v[0].pos.perspectiveDivide(); v[1].pos.perspectiveDivide(); v[2].pos.perspectiveDivide();}
   // @brief: Print the vector parameters
   void print() const {v[0].pos.print(); v[1].pos.print(); v[2].pos.print();}

   // Operator overloads for multiplying a whole triagle by a matrix (just multiplies the underlying vectors)
   prim operator * (const mat4x4& m) const {return prim(this->v[0] * m, this->v[1] * m, this->v[2] * m);}
   void operator *= (const mat4x4& m) { this->v[0] *= m; this->v[1] *= m; this->v[2] *= m; }

private:

};




/// @brief: Takes mesh data from 3D objects, projects the 3D polygons to a 2D view in the 
/// form of a pixel buffer to be drawn by and SFML window.
/// @param res: Reference to the window to be rendered to. Used to find resolution and SFML render target
class cpuRenderObject {

public:
   

   std::vector<light> lights;

   void addLight(const light& newLight){
      lights.push_back(newLight);
   }

   /// @brief: Reference to the sfml window that we will render to
   window& gl_window;
   camera& cam;

   cpuRenderObject(camera& _cam);


   /// @brief: Render 3D vertex data given information from 3D object. This is where most of the
   /// 3D graphics pipeline is excecuted: vertex shader, vertex post processing (triangle clipping)
   /// @param object: Object that provides position, orientation, scale, color and model index information
   void render(const object& object);

   /// @brief: Draw pixel buffer to the sfml window passed to this object
   void draw();


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

   int texW;
   int texH;
   int texC;
   unsigned char* texD;

   /// @brief: Aspect ratio of the window
   float m_aspectRatio;     
   /// @brief: Resolution of the window 
   vec2 m_resolution;


   // /// @brief: Makes an object rotate to point direction
   // mat4x4 m_matPointAt; 
   // /// @brief: Moves objects to position as if the origin was pointed a different direction
   // mat4x4 m_matView; 
   /// @brief: Projects 3D points onto a 2D view
   mat4x4 m_matProject;

   /// @brief: virtual planes that represent the edge of the view frustum in 3d space
   vec4 m_planes[6];
   float far;

   /// @brief: Buffer to clear the pixelBuffer with a background color
   std::vector<std::uint8_t> m_clearBuffer;
   /// @brief: Buffer to store the lowest z position of each pixel to decide whether we should draw over it
   std::vector<float> m_zBuffer;


   std::vector<vertex> vertAttribs;

   std::vector<prim> primatives;

   /// @brief: Scanline triangle fill algorithm. This is a common method of rasterization although it is very inefficient
   /// when ran on the CPU as we are doing here. steps in pipeline: Rasterization, Fragment Shader
   /// @param tri: Pixel array for 32 bit trucolor + alpha (8 bits for r,g,b and alpha)
   /// @param res: The resolution of the window
   void raster(const prim& p);
   
   /// @brief: In-place clipping of triangles in m_triangleAttribs against all 6 planes 
   /// planes in clip space (after projection, before perspective division)
   void clipTriangles();

   /// @brief: Checks the given triangle for winding in screen space
   /// @param tri: triangle to check for culling
   bool backFaceCulling(const prim& tri);

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
