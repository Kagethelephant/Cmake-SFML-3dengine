#include "object.hpp"

#include <math.h>
#include "utils/matrix.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>



// UPDATE POSITION
//---------------------------------------------------------------------------------------------
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




model::model(const std::string& filename, bool ccwWinding) {

   // Open the obj file
   std::ifstream obj(filename);
   if (!obj) {
      throw std::runtime_error("Failed to open OBJ file");
   }

   // Raw OBJ attribute streams
   std::vector<vec3> objPositions;
   std::vector<vec3> objNormals;
   std::vector<vec2> objTexcoords;

   // (v,t,n) → unified vertex index hashtable 
   std::unordered_map<vertexKey, uint32_t, vertexKeyHash> vertexCache;

   std::string line;
   while (std::getline(obj, line)) {

      std::stringstream stream(line);
      std::string type;
      stream >> type;

      // -------------------------------------------------
      // POSITION
      // -------------------------------------------------
      if (type == "v") {
         vec3 p;
         stream >> p.x >> p.y >> p.z;
         objPositions.push_back(p);
      }

      // -------------------------------------------------
      // TEXCOORD
      // -------------------------------------------------
      else if (type == "vt") {
         vec2 uv;
         stream >> uv.x >> uv.y;
         objTexcoords.push_back(uv);
      }

      // -------------------------------------------------
      // NORMAL
      // -------------------------------------------------
      else if (type == "vn") {
         vec3 n;
         stream >> n.x >> n.y >> n.z;
         objNormals.push_back(n);
      }

      // -------------------------------------------------
      // FACE (triangles, quads, ngons)
      // -------------------------------------------------
      else if (type == "f") {

         // Temporary storage for one polygon face
         std::vector<int> vIdx;
         std::vector<int> tIdx;
         std::vector<int> nIdx;

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
            // std::string::npos is what find() reterns when character is not found
            if (p1 != std::string::npos){
               // Get the texture value (adjusted to 0 based indexing)
               t = std::stoi(vert.substr(p1 + 1, p2 - p1 - 1)) - 1;
            }
            else{
               t = -1;
            }

            // checks that there is a second slash and grabs normal value after it
            if (p2 != std::string::npos)
               n = std::stoi(vert.substr(p2 + 1)) - 1;
            else
               n = -1;

            // Add values to the vertice buffers
            vIdx.push_back(v);
            tIdx.push_back(t);
            nIdx.push_back(n);
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
               vertexKey key{
                  vIdx[tri[k]],
                  tIdx[tri[k]],
                  nIdx[tri[k]]
               };

               auto it = vertexCache.find(key);
               // vertexCache.find() will return vertexCache.end() if it is not found
               // If it is found than add the already existing vertex index to indices
               if (it != vertexCache.end()) {
                  indices.push_back(it->second);
               } 
               else {
                  // If it does not exist create a new vertex
                  vertex vertOut;
                  vertOut.pos = objPositions[key.v];
                  // Some OBJ files omit texcoords or normals

                  vertOut.uv     = (key.t >= 0) ? objTexcoords[key.t] : vec2(0,0);
                  vertOut.normal = (key.n >= 0) ? objNormals[key.n]    : vec3(0,0,1);

                  uint32_t newIndex = static_cast<uint32_t>(vertices.size());
                  vertices.push_back(vertOut);
                  vertexCache[key] = newIndex;
                  indices.push_back(newIndex);
               }
            }
         }
      }
   }

   verticesRaw.reserve(vertices.size() * 8); // 3 pos + 3 normal + 2 uv = 8 floats per vertex

   for (const auto& vert : vertices) {
      // pos
      verticesRaw.push_back(vert.pos.c[0]);
      verticesRaw.push_back(vert.pos.c[1]);
      verticesRaw.push_back(vert.pos.c[2]);
      // normal
      verticesRaw.push_back(vert.normal.c[0]);
      verticesRaw.push_back(vert.normal.c[1]);
      verticesRaw.push_back(vert.normal.c[2]);
      // uv
      verticesRaw.push_back(vert.uv.c[0]);
      verticesRaw.push_back(vert.uv.c[1]);
   }
}
