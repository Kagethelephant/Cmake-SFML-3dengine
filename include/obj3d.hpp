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
/// \brief Triangle for rendering 3d objects
//////////////////////////////////////////////////////////////////
struct tri3d
{
    vec3 v[3];

    // Constructors
    tri3d(vec3 _x, vec3 _y, vec3 _z) : v{_x,_y,_z}{};
    tri3d() : v{vec3(0,0,0),vec3(0,0,0),vec3(0,0,0)}{};
    
    // Gets the normal vector of a triangle by getting the lines branching from the tris 0 point and getting the normal of the 2 lines
    vec3 norm() {return ((this-> v[1] - this-> v[0]).cross(this-> v[2] - this-> v[0])).norm(); }
    
};



//////////////////////////////////////////////////////////////////
/// \brief 3d camera
//////////////////////////////////////////////////////////////////
class camera
{
    public:

        float x = 0, y = 0, z = 0, u = 0, v = 0, w = 0;

        vec3 direction = vec3(0,0,1);
        vec3 position = vec3(0,0,0);
        vec3 up = vec3(0,1,0);
        vec3 target = vec3(0,0,0);

        mat4x4 point = identity_matrix();
        mat4x4 view = identity_matrix();

        mat4x4 update();
        void move(float _x, float _y, float _z);
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

        //////////////////////////////////////////////////////////////////
        /// \brief Holds trianles or points for 3D mesh or point cloud
        //////////////////////////////////////////////////////////////////
        std::vector<tri3d> mesh;

        //////////////////////////////////////////////////////////////////
        /// \brief Update the rotation matrix of the object
        //////////////////////////////////////////////////////////////////
        void update();

        //////////////////////////////////////////////////////////////////
        /// \brief Called externally to draw the triangles in the mesh 
        //////////////////////////////////////////////////////////////////
        void draw(sf:: RenderTexture& texture, sf::Vector2i res, camera c);

        //////////////////////////////////////////////////////////////////
        /// \brief Load an OBJ file
        //////////////////////////////////////////////////////////////////
        void load(std::string fileName);


    private:

        // Member variables: Matrices for rotating and projecting vertices/triangles
        mat4x4 m_matProj;
        mat4x4 m_matTransform;

        float m_aspectRatio;     

        /// Build the look at matrix
        mat4x4 mat_lookat(vec3& pos, vec3& target, vec3& up);

        /// Build view matrix
        mat4x4 mat_veiw(vec3& pos, vec3& target, vec3& up);

        /// simple draw 2d triangles
        void drawTriangle(sf::RenderTexture& texture, tri3d tri, sf::Color col = c_color(Blue));
};

