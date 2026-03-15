#pragma once

#include <gpuRender/window.hpp>
#include <cstdint>
#include "utils/matrix.hpp"
#include "app/object.hpp"




/// @brief: Takes mesh data from 3D objects, projects the 3D polygons to a 2D view in the 
/// form of a pixel buffer to be drawn to an OpenGL FBO.
class cpuRenderObject {

public:


   /// @brief: Create new CPU rendering engine
   /// @param cam: Reference to the window to be rendered to. Used to find resolution and SFML render target
   cpuRenderObject(camera& cam);

   void bindObject(const object& obj);

   void addLight(const light& newLight){ m_lights.push_back(newLight);}

   /// @brief: Render 3D vertex data given information from 3D object. This is where most of the
   /// 3D graphics pipeline is excecuted: vertex shader, vertex post processing (triangle clipping)
   /// @param object: Object that provides position, orientation, scale, color and model index information
   void render();


private:

   /// @brief: Bundles 3 vertices together. This is created at the primative assembly stage of rendering
   struct triangle3d {

      vertex v[3];

      triangle3d() : v{vertex(),vertex(),vertex()}{};
      triangle3d(vertex v0, vertex v1, vertex v2) : v{v0,v1,v2}{};

      // @brief: Perform perspective divide for all vertices. Divides by respective w value to make distant objects apear smaller
      void perspectiveDivide() {v[0].screenPos.perspectiveDivide(); v[1].screenPos.perspectiveDivide(); v[2].screenPos.perspectiveDivide();}

      // Operator overloads for multiplying a whole triagle by a matrix (just multiplies the underlying vectors)
      triangle3d operator * (const mat4x4& m) const {return triangle3d(this->v[0] * m, this->v[1] * m, this->v[2] * m);}
      void operator *= (const mat4x4& m) { this->v[0] *= m; this->v[1] *= m; this->v[2] *= m; }
   };

   /// @brief: Reference to the sfml window that we will render to
   const window& m_window;
   camera& m_camera;

   /// @brief: Pixel array for 32 bit trucolor + alpha (8 bits for r,g,b and alpha) used to raster triangles
   std::vector<std::uint8_t> m_pixelBuffer;
   /// @brief: Vector array of light objects to use for rendering
   std::vector<light> m_lights;
   /// @brief: Vector array of 3d objects to render (added with bindObject)
   std::vector<object> m_objects;

   /// @brief: Resolution of the window 
   vec2 m_resolution;


   /// @brief: virtual planes that represent the edge of the view frustum in 3d space
   vec4 m_planes[6];

   /// @brief: Buffer to clear the pixelBuffer with a background color
   std::vector<std::uint8_t> m_clearBuffer;
   /// @brief: Buffer to store the lowest z position of each pixel to decide whether we should draw over it
   std::vector<float> m_zBuffer;


   std::vector<vertex> m_vertAttribs;

   std::vector<triangle3d> m_primatives;

   /// @brief: Scanline triangle fill algorithm. This is a common method of rasterization although it is very inefficient
   /// when ran on the CPU as we are doing here. steps in pipeline: Rasterization, Fragment Shader
   /// @param tri: Pixel array for 32 bit trucolor + alpha (8 bits for r,g,b and alpha)
   /// @param res: The resolution of the window
   void raster(const triangle3d& p, const object& obj, const model::subMesh& mesh);
   
   /// @brief: In-place clipping of triangles in m_triangleAttribs against all 6 planes 
   /// planes in clip space (after projection, before perspective division)
   void clipTriangles();

   /// @brief: Checks the given triangle for winding in screen space
   /// @param tri: triangle to check for culling
   bool backFaceCulling(const triangle3d& tri);

   /// @brief: Used to get the position on a line betweeen 2 3d points where 
   /// that line intersects the given plane
   /// @param p1: Point in 3d space
   /// @param p2: 2nd point in 3d space to create a theoretical line with the 1st point
   /// @param plane: Plain in 3d space that intersects the line
   /// @return: float t value representing percent of the line where intersectet[0 - 1]
   float planeIntersect(const vec4& a, const vec4& b, const vec4& plane);

   int wrap(int n, int max);
};
