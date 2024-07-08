#include "obj3d.hpp"



object3d::object3d()
{
    float fAspectRatio = ((float)sf::VideoMode::getDesktopMode().width) / ((float)sf::VideoMode::getDesktopMode().height);
    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = 90.0f;
    float fFovRad = 1.0f / std::tanf(fFov * 0.5f/ 180.0f * 3.14159f);

    matProj.m[0][0] = fAspectRatio * fFovRad;
    matProj.m[1][1] = fFovRad;
    matProj.m[2][2] = fFar / (fFar- fNear);
    matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matProj.m[2][3] = 1.0f;
    matProj.m[3][3] = 0.0f;

}



void object3d::drawSelf(sf::RenderWindow& window, float u, float v, float w, float scale)
{
    float AspectRatio = ((float)sf::VideoMode::getDesktopMode().width) / ((float)sf::VideoMode::getDesktopMode().height);

    rotU.m[0][0] = 1;               //    1    0    0    0
    rotU.m[1][1] = std::cosf(u);    //    0   cos -sin   0
    rotU.m[1][2] = std::sinf(u);    //    0   sin  cos   0
    rotU.m[2][1] = -std::sinf(u);   //    0    0    0    1
    rotU.m[2][2] = std::cosf(u);
    rotU.m[3][3] = 1;

    rotV.m[0][0] = std::cosf(v);    //   cos   0   sin   0
    rotV.m[1][1] = 1;               //    0    1    0    0
    rotV.m[2][0] = std::sinf(v);    //  -sin   0   cos   0
    rotV.m[0][2] = -std::sinf(v);   //    0    0    0    1
    rotV.m[2][2] = std::cosf(v);
    rotV.m[3][3] = 1;

    rotW.m[0][0] = std::cosf(w);    //   cos -sin   0    0
    rotW.m[0][1] = std::sinf(w);    //   sin  cos   0    0
    rotW.m[1][0] = -std::sinf(w);   //    0    0    0    0
    rotW.m[1][1] = std::cosf(w);    //    0    0    0    1
    rotW.m[2][2] = 1;
    rotW.m[3][3] = 1;



    for(auto i: mesh)
    {
        triangle triTrans, triProj, triRotU, triRotV, triRotW;

        triRotU.v[0] = projectVertex(i.v[0], rotU);
        triRotU.v[1] = projectVertex(i.v[1], rotU);
        triRotU.v[2] = projectVertex(i.v[2], rotU);

        triRotV.v[0] = projectVertex(triRotU.v[0], rotV);
        triRotV.v[1] = projectVertex(triRotU.v[1], rotV);
        triRotV.v[2] = projectVertex(triRotU.v[2], rotV);

        triRotW.v[0] = projectVertex(triRotV.v[0], rotW);
        triRotW.v[1] = projectVertex(triRotV.v[1], rotW);
        triRotW.v[2] = projectVertex(triRotV.v[2], rotW);



        triTrans = triRotW;

        triTrans.v[0].z = triRotW.v[0].z + 3.0f;
        triTrans.v[1].z = triRotW.v[1].z + 3.0f;
        triTrans.v[2].z = triRotW.v[2].z + 3.0f;

        

        // Project
        triProj.v[0] = projectVertex(triTrans.v[0], matProj);
        triProj.v[1] = projectVertex(triTrans.v[1], matProj);
        triProj.v[2] = projectVertex(triTrans.v[2], matProj);

        // Scale
        triProj.v[0].x += 1; triProj.v[0].y += 1; 
        triProj.v[1].x += 1; triProj.v[1].y += 1; 
        triProj.v[2].x += 1; triProj.v[2].y += 1; 

        triProj.v[0].x *= 0.5f * 500;
        triProj.v[0].y *= 0.5f * 500;
        triProj.v[1].x *= 0.5f * 500;
        triProj.v[1].y *= 0.5f * 500;
        triProj.v[2].x *= 0.5f * 500;
        triProj.v[2].y *= 0.5f * 500;


        drawTriangle(window, triProj.v[0].x,triProj.v[0].y, triProj.v[1].x,triProj.v[1].y, triProj.v[2].x,triProj.v[2].y );

    }
}


object3d::vertex object3d::projectVertex(vertex &vin, mat4x4 mat)
{
    vertex vout;

    vout.x = vin.x * mat.m[0][0] + vin.y * mat.m[1][0] + vin.z * mat.m[2][0] + mat.m[3][0];
    vout.y = vin.x * mat.m[0][1] + vin.y * mat.m[1][1] + vin.z * mat.m[2][1] + mat.m[3][1];
    vout.z = vin.x * mat.m[0][2] + vin.y * mat.m[1][2] + vin.z * mat.m[2][2] + mat.m[3][2];
    float w = vin.x * mat.m[0][3] + vin.y * mat.m[1][3] + vin.z * mat.m[2][3] + mat.m[3][3];

    if ( w != 0.0f)
    {
        vout.x /= w;
        vout.y /= w;
        vout.z /= w;
    }

    return vout;
}


void object3d::drawTriangle(sf::RenderWindow& window, int x1, int y1, int x2, int y2, int x3, int y3, sf::Color col) 
{
    sf::Vertex line1[2], line2[2], line3[2];
    line1[0].color = col; line1[1].color = col;
    line2[0].color = col; line2[1].color = col;
    line3[0].color = col; line3[1].color = col;

    line1[0].position = sf::Vector2f(x1, y1); line1[1].position = sf::Vector2f(x2,y2);
    line2[0].position = sf::Vector2f(x2, y2); line2[1].position = sf::Vector2f(x3,y3);
    line3[0].position = sf::Vector2f(x3, y3); line3[1].position = sf::Vector2f(x2,y2);

    window.draw(line1,2,sf::Lines);
    window.draw(line2,2,sf::Lines);
    window.draw(line3,2,sf::Lines);

}
