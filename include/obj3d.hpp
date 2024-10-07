#pragma once

//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <strstream>
#include <algorithm>
#include "data.hpp"
#include "math.hpp"


//////////////////////////////////////////////////////////////////
/// \brief Namespace for 3d objects and algorithms
//////////////////////////////////////////////////////////////////
namespace rend3d
{
    /// Triangles for constructing 3D mesh
    struct triangle
        {
        triangle(vector3 _x = vector3(0,0,0), vector3 _y = vector3(0,0,0), vector3 _z = vector3(0,0,0)){v[0] =_x; v[1] = _y; v[2] = _z;};
        vector3 v[3];
        };

    //////////////////////////////////////////////////////////////////
    /// \brief 3D object with storing triangle mesh and the means to render it
    //////////////////////////////////////////////////////////////////
    class object3d
        {
        public:

            object3d();

            //////////////////////////////////////////////////////////////////
            /// \brief Holds triangles or points for 3D mesh or point cloud
            //////////////////////////////////////////////////////////////////
            std::vector<rend3d::triangle> mesh;

            //////////////////////////////////////////////////////////////////
            /// \brief Update the rotation matrix of the object
            //////////////////////////////////////////////////////////////////
            void rotateObject(float u = 0.0f, float v = 0.0f, float w = 0.0f);

            //////////////////////////////////////////////////////////////////
            /// \brief Called externally to draw the triangles in the mesh 
            //////////////////////////////////////////////////////////////////
            void drawMesh(sf:: RenderTexture& texture, sf::Vector2i res, float zoom = 3.0f);

            //////////////////////////////////////////////////////////////////
            /// \brief Load an OBJ file
            //////////////////////////////////////////////////////////////////
            void loadObj(std::string fileName);


        private:

        // Member variables: Matrices for rotating and projecting vertices/triangles
        mat4x4 m_matProj;
        mat4x4 m_matRotU;
        mat4x4 m_matRotV;
        mat4x4 m_matRotW;

        float m_aspectRatio;     

        // Get the normal vector to a triangle
        vector3 triNorm(triangle tri);

        /// Apply progection matrix to the given triangle
        //triangle projectTriangle(triangle tri, float zoom = 3.0f);

        /// Apply the rotation matrix to the given triangle
        //triangle orientTriangle(triangle tri);


        /// Private function for performing matrice transformations on vertices
        vector3 matMultiply(vector3 vin, mat4x4 mat);

        /// simple draw 2d triangles
        void drawTriangle(sf::RenderTexture& texture, triangle tri, sf::Color col = c_color(Blue));

        };
}
