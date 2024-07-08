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



void object3d::drawSelf(sf::RenderTexture& texture, float u, float v, float w)
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

        triRotU.v[0] = matMultiply(i.v[0], rotU);
        triRotU.v[1] = matMultiply(i.v[1], rotU);
        triRotU.v[2] = matMultiply(i.v[2], rotU);

        triRotV.v[0] = matMultiply(triRotU.v[0], rotV);
        triRotV.v[1] = matMultiply(triRotU.v[1], rotV);
        triRotV.v[2] = matMultiply(triRotU.v[2], rotV);

        triRotW.v[0] = matMultiply(triRotV.v[0], rotW);
        triRotW.v[1] = matMultiply(triRotV.v[1], rotW);
        triRotW.v[2] = matMultiply(triRotV.v[2], rotW);



        triTrans = triRotW;

        triTrans.v[0].z = triRotW.v[0].z + 3.0f;
        triTrans.v[1].z = triRotW.v[1].z + 3.0f;
        triTrans.v[2].z = triRotW.v[2].z + 3.0f;

        

        // Project
        triProj.v[0] = matMultiply(triTrans.v[0], matProj);
        triProj.v[1] = matMultiply(triTrans.v[1], matProj);
        triProj.v[2] = matMultiply(triTrans.v[2], matProj);

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


        drawTriangle(texture, triProj);

    }
}


object3d::point object3d::matMultiply(point &pin, mat4x4 mat)
{
    point pout;

    pout.x = pin.x * mat.m[0][0] + pin.y * mat.m[1][0] + pin.z * mat.m[2][0] + mat.m[3][0];
    pout.y = pin.x * mat.m[0][1] + pin.y * mat.m[1][1] + pin.z * mat.m[2][1] + mat.m[3][1];
    pout.z = pin.x * mat.m[0][2] + pin.y * mat.m[1][2] + pin.z * mat.m[2][2] + mat.m[3][2];
    float w = pin.x * mat.m[0][3] + pin.y * mat.m[1][3] + pin.z * mat.m[2][3] + mat.m[3][3];

    if ( w != 0.0f)
    {
        pout.x /= w;
        pout.y /= w;
        pout.z /= w;
    }
    return pout;
}


void object3d::drawTriangle(sf::RenderTexture& texture, triangle tri, sf::Color col) 
{
    sf::Vertex line1[2], line2[2], line3[2];
    line1[0].color = col; line1[1].color = col;
    line2[0].color = col; line2[1].color = col;
    line3[0].color = col; line3[1].color = col;

    line1[0].position = sf::Vector2f(tri.v[0].x, tri.v[0].y); line1[1].position = sf::Vector2f(tri.v[1].x, tri.v[1].y);
    line2[0].position = sf::Vector2f(tri.v[1].x, tri.v[1].y); line2[1].position = sf::Vector2f(tri.v[2].x, tri.v[2].y);
    line3[0].position = sf::Vector2f(tri.v[2].x, tri.v[2].y); line3[1].position = sf::Vector2f(tri.v[0].x, tri.v[0].y);

    texture.draw(line1,2,sf::Lines);
    texture.draw(line2,2,sf::Lines);
    texture.draw(line3,2,sf::Lines);

}
