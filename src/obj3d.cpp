#include "obj3d.hpp"


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
object3d::object3d()
{
    m_aspectRatio = ((float)sf::VideoMode::getDesktopMode().width) / ((float)sf::VideoMode::getDesktopMode().height);

    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = 90.0f;
    float fFovRad = 1.0f / std::tanf(fFov * 0.5f/ 180.0f * 3.14159f);

    m_matProj.m[0][0] = m_aspectRatio * fFovRad;
    m_matProj.m[1][1] = fFovRad;
    m_matProj.m[2][2] = fFar / (fFar- fNear);
    m_matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    m_matProj.m[2][3] = 1.0f;
    m_matProj.m[3][3] = 0.0f;
}




//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
void object3d::rotate(float u, float v, float w)
{
    m_matRotU.m[0][0] = 1;               //    1    0    0    0
    m_matRotU.m[1][1] = std::cosf(u);    //    0   cos -sin   0
    m_matRotU.m[1][2] = std::sinf(u);    //    0   sin  cos   0     for dividing by z when
    m_matRotU.m[2][1] = -std::sinf(u);   //    0    0    0    1 <<  scaling distant objects
    m_matRotU.m[2][2] = std::cosf(u);
    m_matRotU.m[3][3] = 1;

    m_matRotV.m[0][0] = std::cosf(v);    //   cos   0   sin   0
    m_matRotV.m[1][1] = 1;               //    0    1    0    0
    m_matRotV.m[2][0] = std::sinf(v);    //  -sin   0   cos   0
    m_matRotV.m[0][2] = -std::sinf(v);   //    0    0    0    1
    m_matRotV.m[2][2] = std::cosf(v);
    m_matRotV.m[3][3] = 1;

    m_matRotW.m[0][0] = std::cosf(w);    //   cos -sin   0    0
    m_matRotW.m[0][1] = std::sinf(w);    //   sin  cos   0    0
    m_matRotW.m[1][0] = -std::sinf(w);   //    0    0    0    0
    m_matRotW.m[1][1] = std::cosf(w);    //    0    0    0    1
    m_matRotW.m[2][2] = 1;
    m_matRotW.m[3][3] = 1;
}



//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
object3d::point object3d::projectPoint(point pin, float zoom)
{
            point pout;
            // Rotate the view
            pout = matMultiply(matMultiply(matMultiply(pin, m_matRotU), m_matRotV), m_matRotW);
            
            // Push farther into screen so we can see it
            pout.z += zoom;

            // Project 2D
            pout = matMultiply(pout, m_matProj);

            // Center on screen
            pout.x += 2.2; 
            pout.y += 1; 

            // Scale to size
            pout.x *= 0.5f * 500/ m_aspectRatio;
            pout.y *= 0.5f * 500;

            return pout;
}




//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
void object3d::drawMesh(sf::RenderTexture& texture, float u, float v, float w, float zoom)
{
    rotate(u,v,w);
    triangle tri;
    for(auto i: mesh)
    {
        tri = i;
        
        for (int z = 0; z<3; z++)
        {  
            // Rotate the view
            tri.v[z] = projectPoint(tri.v[z],zoom);
        }
        drawTriangle(texture, tri);
    }
}





//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
void object3d::drawPointCloud(sf::RenderTexture& texture, float u, float v, float w, float zoom)
{
    rotate(u,v,w);
    point p;

    for(auto i: pointCloud)
    {   
        p = i;
        p = projectPoint(p,zoom);

        sf::Vertex pout(sf::Vector2f(p.x, p.y), c_color(Blue));
        texture.draw(&pout, 1, sf::Points);
    }
}




//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
object3d::point object3d::matMultiply(point &pin, mat4x4 mat)
{
    point pout;

    pout.x = pin.x * mat.m[0][0] + pin.y * mat.m[1][0] + pin.z * mat.m[2][0] + mat.m[3][0];
    pout.y = pin.x * mat.m[0][1] + pin.y * mat.m[1][1] + pin.z * mat.m[2][1] + mat.m[3][1];
    pout.z = pin.x * mat.m[0][2] + pin.y * mat.m[1][2] + pin.z * mat.m[2][2] + mat.m[3][2];
    float w = pin.x * mat.m[0][3] + pin.y * mat.m[1][3] + pin.z * mat.m[2][3] + mat.m[3][3];

    if ( w != 0.0f) {pout.x /= w; pout.y /= w; pout.z /= w;}
    
    return pout;
}




//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
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
