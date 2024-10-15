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
        vec3 v[3];

        // Constructors
        triangle(vec3 _x, vec3 _y, vec3 _z) : v{_x,_y,_z}{};
        triangle() : v{vec3(0,0,0),vec3(0,0,0),vec3(0,0,0)}{};
        
        // Gets the normal vector of a triangle by getting the lines branching from the tris 0 point and getting the normal of the 2 lines
        vec3 norm() {return ((this-> v[2] - this-> v[0]).cross(this-> v[1] - this-> v[0])).norm(); }
        
        };

    //////////////////////////////////////////////////////////////////
    /// \brief 3D object with storing triangle mesh and the means to render it
    //////////////////////////////////////////////////////////////////
    class object3d
        {
        public:

            object3d();
            
            //////////////////////////////////////////////////////////////////
            /// @brief Object position and orientation variables
            //////////////////////////////////////////////////////////////////
            float u,v,w,x,y,z;

            vec3 pointDir = vec3(.2,0,.8);
            vec3 pointUp = vec3(0,1,0);
            vec3 pointPos = vec3(0,0,0);
            vec3 pointTarget;

            //////////////////////////////////////////////////////////////////
            /// \brief Holds trianles or points for 3D mesh or point cloud
            //////////////////////////////////////////////////////////////////
            std::vector<rend3d::triangle> mesh;

            //////////////////////////////////////////////////////////////////
            /// \brief Update the rotation matrix of the object
            //////////////////////////////////////////////////////////////////
            void update();

            //////////////////////////////////////////////////////////////////
            /// \brief Called externally to draw the triangles in the mesh 
            //////////////////////////////////////////////////////////////////
            void draw(sf:: RenderTexture& texture, sf::Vector2i res);

            //////////////////////////////////////////////////////////////////
            /// \brief Load an OBJ file
            //////////////////////////////////////////////////////////////////
            void load(std::string fileName);


        private:

        // Member variables: Matrices for rotating and projecting vertices/triangles
        mat4x4 m_matRot;
        mat4x4 m_matPos;
        mat4x4 m_matProj;
        mat4x4 m_matLook;
        float m_aspectRatio;     

        /// Build the look at matrix
        mat4x4 mat_lookat(vec3& pos, vec3& target, vec3& up);

        /// Build view matrix
        mat4x4 mat_veiw(vec3& pos, vec3& target, vec3& up);

        /// simple draw 2d triangles
        void drawTriangle(sf::RenderTexture& texture, triangle tri, sf::Color col = c_color(Blue));

        };
}
