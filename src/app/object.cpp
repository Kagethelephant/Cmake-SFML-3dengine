#include "object.hpp"
// OpenGL
#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
// Standard Libraries
#include <iostream>
#include <math.h>
#include "utils/matrix.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <unordered_map>
// STB_Image
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// MOVE AND ROTATE CAMERA
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void camera::move(const float& x, const float& y, const float& z) {
   
   vec3 up = vec3(0,1,0) * matrix_transform(0, 0, 0, rotation[0], rotation[1], rotation[2]);
   position += (direction.cross(up) * x);
   position += (direction * z);
   position[1] += y;
   mat_view = matrix_view(matrix_pointAt(position, direction, up));
}


void camera::rotate(const float& u, const float& v, const float& w) {
   
   rotation += vec3(u, v, w);
   vec3 up = vec3(0,1,0) * matrix_transform(0, 0, 0, rotation[0], rotation[1], rotation[2]);
   direction = (vec3(0,0,-1) * matrix_transform(0, 0, 0, rotation[0], rotation[1],rotation[2])).normal();
   mat_view = matrix_view(matrix_pointAt(position, direction, up));
}

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// MOVE AND ROTATE OBJECT
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
void object::scale(float sx, float sy, float sz){
   scales = vec3(sx,sy,sz);
   matScale = matrix_scale(scales[0], scales[1], scales[2]);
}


void object::move(float x, float y, float z){
   position = vec3(x,y,z);
   matTransform = matrix_transform(position[0], position[1], position[2], rotation[0], rotation[1], rotation[2]);
}


void object::rotate(float u, float v, float w){
   rotation = vec3(u,v,w);
   matTransform = matrix_transform(position[0], position[1], position[2], rotation[0], rotation[1], rotation[2]);
}




//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// LOAD OBJ MODEL
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
model::model(const std::string& filename, bool ccwWinding) {

   std::cout << filename << std::endl;
   // Open the obj file
   std::ifstream obj(filename);
   if (!obj) { throw std::runtime_error("Failed to open OBJ file");}

   std::string dir = getDirectory(filename);

   // Raw OBJ attribute streams
   std::vector<vec3> objPositions;
   std::vector<vec2> objTexcoords;

   // (v,t,n) → unified vertex index hashtable 
   // This is the key, the value, and the hash callback function to generate the hash key
   std::unordered_map<vertexKey, uint32_t, vertexKeyHash> vertexCache;

   subMesh* currentSubmesh;

   std::string line;
   while (std::getline(obj, line)) {

      std::stringstream stream(line);
      std::string type;
      stream >> type;

      // POSITION
      // -------------------------------------------------
      if (type == "v") {
         vec3 p;
         stream >> p.x >> p.y >> p.z;
         objPositions.push_back(p);
      }
      // TEXCOORD
      // -------------------------------------------------
      else if (type == "vt") {
         vec2 uv;
         stream >> uv.x >> uv.y;
         objTexcoords.push_back(uv);
      }
      // MTL FILE
      // -------------------------------------------------
      else if (type == "mtllib") {
         std::string mtlfile;
         stream >> mtlfile;
         loadMTL(dir + mtlfile, materialMap);
      }
      // MTL MATERIAL
      // -------------------------------------------------
      else if (type == "usemtl") {
         std::string mtlName;
         stream >> mtlName;
         material mtl = materialMap[mtlName];
         subMesh newSubmesh;
         newSubmesh.tex = mtl.diffuseTex;
         subMeshes.push_back(newSubmesh);
      }

      // FACE (triangles, quads, ngons)
      // -------------------------------------------------
      else if (type == "f") {
         
         if(subMeshes.size() == 0){
            subMesh newSubmesh;
            newSubmesh.textured = false;
            subMeshes.push_back(newSubmesh);
         }
         
         // Get the top submesh in the submesh array
         subMesh& currentMesh = subMeshes.back();

         // Temporary storage for one polygon face
         std::vector<int> vIdx;
         std::vector<int> tIdx;

         std::string vert;
         while (stream >> vert) {
            int v = 0, t = 0, n = 0;

            // Parse v/t/n or v//n or v/t
            // Get the position of the first slash and second slash
            size_t p1 = vert.find('/');
            size_t p2 = vert.find('/', p1 + 1);

            // First vertex is the position first slash (adjusted to 0 based indexing)
            v = std::stoi(vert.substr(0, p1)) - 1;

            // Checks that there is a second slash and there is something btwn first and second slash
            // Get the texture value (adjusted to 0 based indexing)
            if (p1 != std::string::npos) {t = std::stoi(vert.substr(p1 + 1, p2 - p1 - 1)) - 1; currentMesh.textured = true;}
            else currentMesh.textured = false;

            // Add values to the vertice buffers
            vIdx.push_back(v);
            tIdx.push_back(t);
         }

         // Need at least a triangle
         if (vIdx.size() < 3) continue;

         // -------------------------------------------------
         // FAN TRIANGULATION
         // (v0, v1, v2), (v0, v2, v3), ...
         // -------------------------------------------------
         for (size_t i = 1; i + 1 < vIdx.size(); ++i) {

            // Start with the first triangle (should be {0,1,2} then {0,2,3} for triangle fan)
            int tri[3] = {0, static_cast<int>(i), static_cast<int>(i + 1)};
            if (ccwWinding) std::swap(tri[1], tri[2]);

            for (int k = 0; k < 3; ++k) {
               // Create a key that ties all the attributes of the vertex together
               vertexKey key{vIdx[tri[k]],tIdx[tri[k]]};

               auto it = vertexCache.find(key);
               // vertexCache.find() will return vertexCache.end() if it is not found
               // If it is found than add the already existing vertex index to indices
               if (it != vertexCache.end()) currentMesh.indices.push_back(it->second);
               else {
                  // std::cout << "-------Create new vert---------" << std::endl;
                  // If it does not exist create a new vertex
                  int correctedKeyV, correctedKeyT;
                  if (key.v < 0) correctedKeyV = objPositions.size() + key.v + 1;
                  else correctedKeyV = key.v;
                  if (key.t < 0) correctedKeyT = objTexcoords.size() + key.t + 1;
                  else correctedKeyT = key.t;
                  // std::cout << correctedKeyV << std::endl;
                  // std::cout << correctedKeyT << std::endl;

                  vertex vertOut;
                  vertOut.pos = vec4(objPositions[correctedKeyV],1.0f);
                  // Some OBJ files omit texcoords or normals
                  if (currentMesh.textured){
                     vertOut.uv = objTexcoords[correctedKeyT];
                     currentMesh.textured = true;
                  }
                  else {
                     vertOut.uv = vec2(0,0);
                  }

                  uint32_t newIndex = static_cast<uint32_t>(vertices.size());
                  vertices.push_back(vertOut);
                  vertexCache[key] = newIndex;
                  currentMesh.indices.push_back(newIndex);
               }
            }
         }
      }
   }

   verticesRaw.reserve(vertices.size() * 8); // 3 pos + 3 normal + 2 uv = 8 floats per vertex

   // Push all vertice data back to a raw vertice array to send to GPU
   for (const auto& vert : vertices) {
      // pos
      verticesRaw.push_back(vert.pos.c[0]);
      verticesRaw.push_back(vert.pos.c[1]);
      verticesRaw.push_back(vert.pos.c[2]);
      // uv
      verticesRaw.push_back(vert.uv.c[0]);
      verticesRaw.push_back(vert.uv.c[1]);
   }
}


//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// HELPERS
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
// ------------------------- Get File Directory Path -------------------------
std::string model::getDirectory(const std::string& path) {
   size_t pos = path.find_last_of("/\\");
   if (pos == std::string::npos)
      return "";
   return path.substr(0, pos + 1); // include trailing slash
}


// ------------------------- Load Texture File -------------------------
model::texture model::loadTexture(const std::string& filepath) {
   std::string correctedPath = filepath;
   std::replace(correctedPath.begin(), correctedPath.end(), '\\', '/');
   texture tex;
   stbi_set_flip_vertically_on_load(true);

   tex.data = stbi_load(correctedPath.c_str(), &tex.w, &tex.h, &tex.channels, 0);
   if (!tex.data)
      throw std::runtime_error("Failed to load texture: " + correctedPath);

   GLenum format;
   if (tex.channels == 1) format = GL_RED;
   else if (tex.channels == 3) format = GL_RGB;
   else if (tex.channels == 4) format = GL_RGBA;
   else {
      stbi_image_free(tex.data);
      throw std::runtime_error("Unsupported image channel count: " + correctedPath);
   }

   return tex;
}

// ------------------------- Load MTL File -------------------------
void model::loadMTL(const std::string& path, std::unordered_map<std::string, material>& materials) {
   std::ifstream file(path);
   if (!file) throw std::runtime_error("Failed to open MTL: " + path);

   // Get the directory of the file to find the textures
   std::string dir = getDirectory(path);
   material* current = nullptr;
   std::string line;

   // itterate through the entire mtl file
   while (std::getline(file, line)) {
      std::stringstream ss(line);
      std::string type;
      ss >> type;

      // Load the name of the material into the map
      // This works because the "newmtl" will come before the "map_Kd" so current will be set before "map_Kd"
      if (type == "newmtl") {
         std::string name;
         ss >> name;
         materials[name] = {}; // Creates another element in the map if it does not exist
         materials[name].name = name; // Load name into the name po
         current = &materials[name];
      } 
      // Load the texture into the material map
      else if (type == "map_Kd" && current) {
         std::string texPath;
         ss >> texPath;
         // Load texture from the path
         current->diffuseTex = loadTexture(dir + texPath);
      }
   }
}
