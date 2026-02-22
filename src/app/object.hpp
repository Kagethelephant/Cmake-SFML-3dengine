#pragma once

#include "utils/data.hpp"
#include "utils/matrix.hpp"
#include <gpuRender/window.hpp>
#include <vector>
#include <unordered_map>


constexpr unsigned int MAX_LIGHTS = 32;

struct vertex {

   vec4 pos;
   vec4 clipPos;
   vec4 fragPos;
   vec3 color;
   vec3 normal;
   vec2 uv;

   float& x;
   float& y;
   float& z;
   float& w;

   // Constructors (member initializer list)
   vertex() : pos{0,0,0,1}, x{pos.x}, y{pos.y}, z{pos.z}, w{pos.w}{}
   vertex(float _x, float _y, float _z) : pos{_x,_y,_z}, x{pos.x}, y{pos.y}, z{pos.z}, w{pos.w} {}


   // Copy constructor and operator
   vertex(const vertex& other) : 
      pos{other.pos}, 
      clipPos{other.clipPos}, 
      fragPos{other.fragPos}, 
      normal{other.normal}, 
      uv{other.uv}, 
      x{pos.x}, 
      y{pos.y}, 
      z{pos.x}, 
      w{pos.w} {}

   vertex& operator = (const vertex& other){
      if (this != &other){
         pos = other.pos;
         clipPos = other.clipPos;
         fragPos = other.fragPos;
         normal = other.normal;
         uv = other.uv;
      }  
      return *this;
   }
   vertex operator * (const mat4x4& m) const{ vertex v = *this; v.pos *= m; return v;}
   void operator *= (const mat4x4& m) { this->pos *= m; this->pos *= m; this->pos *= m; }

   // Member functions
   //---------------------------------------------------------------------------------------------
   /// @brief: returns the magnitude of the vector
   vertex lerp(const vertex& v2, const float& t) const { 
      vertex v1 = *this; 
      v1.pos = v1.pos + (v2.pos - v1.pos) * t;
      v1.clipPos = v1.clipPos + (v2.clipPos - v1.clipPos) * t;
      v1.fragPos = v1.fragPos + (v2.fragPos - v1.fragPos) * t;
      v1.normal = v1.normal + (v2.normal - v1.normal) * t;
      v1.uv = v1.uv + (v2.uv - v1.uv) * t;
      return v1;
   }
};


struct texture {
   int w;
   int h;
   int channels;
   unsigned char* data;
};

struct material {
   std::string name;
   texture diffuseTex;
};



struct subMesh {

   std::vector<uint32_t> indices;     // EBO
   bool textured = true;
   texture tex;
};

struct light {
   vec3 position;
   vec3 color;
};



class camera {
public:
   camera(window& _win) : gl_window(_win) {
      move(0,0,0);
      rotate(0,0,0);
   }

   vec3 position = vec3(0,0,0);
   vec3 rotation = vec3(0,0,0);
   vec3 direction = vec3(0,0,-1);

   mat4x4 mat_view;

   window& gl_window;

   // /// @brief: Moves camera by given values (relative) movement is 
   // /// based on the direction of the camera ( z moves forward/back, x moves sideways)
   // /// @param x: Move sideways (normal to up and pointDirection of camera)
   // /// @param y: Move in the up direction ( normal to the forward and right of the camera)
   // /// @param x: Move in the direction of camera pointDirection
   void move(float x, float y, float z);

   // /// @brief: Rotates camera by given values (relative) movement is 
   // /// based on the direction of the camera ( z moves forward/back, x moves sideways)
   // /// @param u: Rotate around the right direction of the camera
   // /// @param v: Rotate around the up direction of the camera
   // /// @param w: Rotate around the pointDirection of the camera
   void rotate(float u, float v, float w);

private:

};


light createLight(const vec3& pos, const vec3& col = vec3(255,255,255));



/// @brief: Loads vertex data into the triangle buffer from an OBJ file and saves the 
/// location with a model object 
/// @param filename: filepath to the OBJ file
/// @param ccwWinding: changes the winding on the model so the triangle normal points outwards
class model {
public:
   model(const std::string& filename, bool ccwWinding = false);



   bool hasTexture = false;
   bool hasNormal  = false;

   std::vector<float>   verticesRaw; // VBO
   std::vector<vertex>   vertices; // VBO
   

   texture loadTexture(const std::string& filename);

   std::unordered_map<std::string, material> materialMap;

   std::vector<subMesh> subMeshes;

private:

   struct vertexKey {
      int v;
      int t;
      int n;

      // Check if the another key is equal to this one
      bool operator==(const vertexKey& o) const {
         return v == o.v && t == o.t && n == o.n;
      }
   };

   struct vertexKeyHash {
      size_t operator()(const vertexKey& k) const {
         return std::hash<int>()(k.v)
         ^ (std::hash<int>()(k.t) << 1)
         ^ (std::hash<int>()(k.n) << 2);
      }
   };

   void loadMTL(const std::string& path, std::unordered_map<std::string, material>& materials);

   std::string getDirectory(const std::string& path);

};


/// @brief: 3D object with storing triangle mesh and the means to render it
/// @param filename: Relitive file path of the obj file to load
/// @param _position: Starting position of the object (default {0,0,0})
/// @param _scale: Scale to apply to the object (default {1,1,1})
/// @param _color: Color to draw the object (will be shaded by camera object) (default: white)
/// @param _lineColor: Color to draw the outline of the triangles (defualt: transperant)
class object {

public:
   
   /// @brief: Coordinates of the object origin in 3D space
   vec3 position = vec3(0,0,0);
   /// @brief: Rotation from original orientation in radians
   vec3 rotation = vec3(0,0,0);
   /// @brief: Scale of the object
   vec3 scales = vec3(1,1,1);
   /// @brief: Winding of triangle mesh. default clockwise
   bool ccwWinding = false;

   mat4x4 matTransform = matrix_transform(position[0], position[1], position[2], rotation[0], rotation[1], rotation[2]);

   mat4x4 matScale = matrix_scale(scales[0], scales[1], scales[2]);

   model& mod;

   /// @brief: Base color to draw the object (this will be shaded by the camera)
   Color color = Color::White;


   object(model& _mod) : mod{_mod} {};


   void scale(float sx, float sy, float sz);

   void move(float x, float y, float z);

   void rotate(float u, float v, float w);

private:


};


