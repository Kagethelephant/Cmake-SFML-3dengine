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

    struct vertex
    {
        float x,y,z;
    };

    struct triangle
    {
        vertex v[3];
    };

    struct mat4x4 
    {
        float m[4][4] = {0};
    };

    std::vector<triangle> mesh;

    mat4x4 matProj;
    mat4x4 rotU;
    mat4x4 rotV;
    mat4x4 rotW;


    void drawSelf(sf:: RenderWindow& window, float u = 0, float v = 0, float w = 0, float scale = 1);



private:

    object3d::vertex projectVertex(vertex &vin, mat4x4 mat);

    void drawTriangle(sf::RenderWindow& window, int x1, int y1, int x2, int y2, int x3, int y3, sf::Color col = c_color(White));

};