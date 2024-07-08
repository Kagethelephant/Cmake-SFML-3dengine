#pragma once

//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include "data.hpp"




//////////////////////////////////////////////////////////////////
/// \brief Create a 3D object
/// \return SFML color object
//////////////////////////////////////////////////////////////////
class object3d
{
public:

    object3d();

    struct point{float x,y,z;};

    struct triangle{point v[3];};

    struct mat4x4 {float m[4][4] = {0};};


    std::vector<triangle> mesh;

    mat4x4 matProj;
    mat4x4 rotU;
    mat4x4 rotV;
    mat4x4 rotW;


    void drawSelf(sf:: RenderTexture& texture, float u = 0, float v = 0, float w = 0);



private:

    point matMultiply(point &pin, mat4x4 mat);

    void drawTriangle(sf::RenderTexture& texture, triangle tri, sf::Color col = c_color(White));

};