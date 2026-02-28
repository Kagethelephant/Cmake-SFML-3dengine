#pragma once
// Project Libraries
#include "utils/data.hpp"
#include "utils/matrix.hpp"
#include <gpuRender/window.hpp>
// Standard Libraries
#include <vector>
#include <unordered_map>




// Constant max number of lights (this needs to match 3d frag shader constant for max lights)
constexpr unsigned int MAX_LIGHTS = 32;


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// VERTEX
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

/// @brief: Stores all potential vertex data so it can be transformed and moved as one object.
/// This the rendering code simpler so we dont have to align multiple vector arrays for position, clip position, UV, etc.
/// @param x: position vector x
/// @param y: position vector y
/// @param z: position vector z
struct vertex {

   vec4 pos;      // Screen position
   vec4 clipPos;  // Clip space Position
   vec4 fragPos;  // Frag Position (world space position)
   vec3 color;    // Color of vertice (not used)
   vec3 normal;   // Normal vector of vertice (not used)
   vec2 uv;       // Texture coordinates of vertice

   // Screen position references
   float& x;
   float& y;
   float& z;
   float& w;

   // Constructors (member initializer list)
   vertex() : pos{0,0,0,1}, x{pos.x}, y{pos.y}, z{pos.z}, w{pos.w}{}
   vertex(float _x, float _y, float _z) : pos{_x,_y,_z}, x{pos.x}, y{pos.y}, z{pos.z}, w{pos.w} {}

   // Copy constructor
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

   // Copy operator
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

   // Matrix multiplication operators (only for screen space vector)
   vertex operator * (const mat4x4& m) const{ vertex v = *this; v.pos *= m; return v;}
   void operator *= (const mat4x4& m) { this->pos *= m; this->pos *= m; this->pos *= m; }

   /// @brief: Linearly interpolate 2 vertices with scale t
   /// @param v2: second vertice to create line
   /// @param t: 0-1 scale for position on line
   /// @return: vertex with linear interpolated vertex (all attributes: pos, fragPos, uv, etc,)
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







//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// LIGHTS
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

struct light {
   light (const vec3& pos, const vec3& col = vec3(1,1,1)) : position{pos}, color{col} {}
   vec3 position;
   vec3 color;
};


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// CAMERA
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

class camera {

public:
   // Initialize the view matrice by calling move and rotate to default start position and orientation
   camera(window& _win) : gl_window(_win) { move(0,0,0); rotate(0,0,0);}

   // View matrix that is used for GPU and CPU rendering, this is the matrix that will move a vertex from its world
   // position to its position if the camera was the origin and its look direction was the z axis
   // (moves the world around the camera rather than camera itself)
   mat4x4 mat_view;

   // Reference to the window this camera will be rendering to (window controls resolution not camera)
   window& gl_window;

   /// @brief: Moves camera by given values (relative) movement is 
   /// based on the direction of the camera ( z moves forward/back, x moves sideways)
   /// @param x: Move sideways (normal to up and direction of camera)
   /// @param y: Move in the up direction ( normal to the forward and right of the camera)
   /// @param z: Move in the direction of camera pointDirection
   void move(const float& x, const float& y, const float& z);

   /// @brief: Rotates camera by given values (relative) movement is 
   /// based on the direction of the camera ( z moves forward/back, x moves sideways)
   /// @param u: Rotate around the right direction of the camera
   /// @param v: Rotate around the up direction of the camera
   /// @param w: Rotate around the pointDirection of the camera
   void rotate(const float& u, const float& v, const float& w);

   /// @brief: Returns const reference to camera position
   const vec3& getPosition() const { return (position);}
   /// @brief: Returns const reference to camera rotation
   const vec3& getRotation() const { return (rotation);}
   /// @brief: Returns const reference to camera direction vector
   const vec3& getDirection() const { return (direction);}

private:

   // Position and orientation of camera
   vec3 position = vec3(0,0,0);
   vec3 rotation = vec3(0,0,0);
   vec3 direction = vec3(0,0,-1); // Changed as result of rotation change in rotate functions
};






//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// MODEL OBJECTS
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX


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

   struct texture {
      int w;
      int h;
      int channels;
      unsigned char* data;
   };

   struct subMesh {

      std::vector<uint32_t> indices;
      bool textured = true;
      texture tex;
   };
   
   struct material {
      std::string name;
      texture diffuseTex;
   };

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


