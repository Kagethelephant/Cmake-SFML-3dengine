#include "obj3d.hpp"


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
object3d::object3d()
{
    m_aspectRatio = ((float)sf::VideoMode::getDesktopMode().width) / ((float)sf::VideoMode::getDesktopMode().height);

    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = 90.0f;
    float fFovRad = 1.0f / mat_tan(fFov * 0.5f/ 180.0f * 3.14159f);

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
    m_matRotU.m[1][1] = mat_cos(u);    //    0   cos -sin   0
    m_matRotU.m[1][2] = mat_sin(u);    //    0   sin  cos   0     for dividing by z when
    m_matRotU.m[2][1] = -mat_sin(u);   //    0    0    0    1 <<  scaling distant objects
    m_matRotU.m[2][2] = mat_cos(u);
    m_matRotU.m[3][3] = 1;

    m_matRotV.m[0][0] = mat_cos(v);    //   cos   0   sin   0
    m_matRotV.m[1][1] = 1;               //    0    1    0    0
    m_matRotV.m[2][0] = mat_sin(v);    //  -sin   0   cos   0
    m_matRotV.m[0][2] = -mat_sin(v);   //    0    0    0    1
    m_matRotV.m[2][2] = mat_cos(v);
    m_matRotV.m[3][3] = 1;

    m_matRotW.m[0][0] = mat_cos(w);    //   cos -sin   0    0
    m_matRotW.m[0][1] = mat_sin(w);    //   sin  cos   0    0
    m_matRotW.m[1][0] = -mat_sin(w);   //    0    0    0    0
    m_matRotW.m[1][1] = mat_cos(w);    //    0    0    0    1
    m_matRotW.m[2][2] = 1;
    m_matRotW.m[3][3] = 1;
}



//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
object3d::vec3d object3d::projectPoint(vec3d pin, float zoom)
{
    vec3d pout = pin;
    
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
object3d::vec3d object3d::rotatePoint(vec3d pin)
{
    vec3d pout;
     pout = matMultiply(matMultiply(matMultiply(pin, m_matRotU), m_matRotV), m_matRotW);
    return pout;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
float object3d::dotProd(vec3d pin1, vec3d pin2)
{
    return ((pin1.x * pin2.x) + (pin1.y * pin2.y) + (pin1.z * pin2.z));
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
object3d::vec3d object3d::normalize(vec3d pin)
{
    vec3d pout = pin;

    float l = std::sqrtf(pin.x*pin.x + pin.y*pin.y + pin.z*pin.z);
    pout.x /= l; pout.y /= l; pout.z /= l;

    return pout;
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
void object3d::drawMesh(sf::RenderTexture& texture, float u, float v, float w, float zoom)
{
    rotate(u,v,w);

    vec3d cam;
    vec3d light;

    cam.x = 0; cam.y = 0, cam.z = 0;
    light.x = 1; light.y = 1, light.z = 1;

    light = normalize(light);

    for(triangle i: mesh)
    {
        triangle triIn;

        // Rotate the view
        triIn.v[0] = matMultiply(matMultiply(matMultiply(i.v[0], m_matRotU), m_matRotV), m_matRotW);
        triIn.v[1] = matMultiply(matMultiply(matMultiply(i.v[1], m_matRotU), m_matRotV), m_matRotW);
        triIn.v[2] = matMultiply(matMultiply(matMultiply(i.v[2], m_matRotU), m_matRotV), m_matRotW);

        object3d::vec3d norm, line1, line2;

        line1.x = triIn.v[1].x - triIn.v[0].x;
        line1.y = triIn.v[1].y - triIn.v[0].y;
        line1.z = triIn.v[1].z - triIn.v[0].z;

        line2.x = triIn.v[2].x - triIn.v[0].x;
        line2.y = triIn.v[2].y - triIn.v[0].y;
        line2.z = triIn.v[2].z - triIn.v[0].z;

        norm.x = line1.y * line2.z - line1.z *line2.y;
        norm.y = line1.z * line2.x - line1.x *line2.z;
        norm.z = line1.x * line2.y - line1.y *line2.x;

        // Push farther into screen so we can see it
        triIn.v[0].z += zoom;
        triIn.v[1].z += zoom;
        triIn.v[2].z += zoom;

        norm = normalize(norm);

        float color = dotProd(norm,light);
        

        if(dotProd(norm,triIn.v[1]) > 0)
        {

            // Project 2D
            triIn.v[0] = matMultiply(triIn.v[0], m_matProj);
            triIn.v[1] = matMultiply(triIn.v[1], m_matProj);
            triIn.v[2] = matMultiply(triIn.v[2], m_matProj);

            // Center on screen
            triIn.v[0].x += 1;
            triIn.v[0].y += 1;

            triIn.v[1].x += 1;
            triIn.v[1].y += 1;

            triIn.v[2].x += 1;
            triIn.v[2].y += 1;


            // Scale to size
            triIn.v[0].x *= 0.5f * 500/ m_aspectRatio;
            triIn.v[0].y *= 0.5f * 500;

            triIn.v[1].x *= 0.5f * 500/ m_aspectRatio;
            triIn.v[1].y *= 0.5f * 500;

            triIn.v[2].x *= 0.5f * 500/ m_aspectRatio;
            triIn.v[2].y *= 0.5f * 500;

            
            drawTriangle(texture, triIn, sf::Color(65,95,120,155+(color*100)));
        }
    }
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
void object3d::drawVecCloud(sf::RenderTexture& texture, float u, float v, float w, float zoom)
{
    rotate(u,v,w);
    vec3d p;

    for(auto i: vecCloud)
    {   
        p = i;
    
        p = rotatePoint(p);

        p = projectPoint(p,zoom);

        sf::Vertex pout(sf::Vector2f(p.x, p.y), c_color(Blue));
        texture.draw(&pout, 1, sf::Points);
    }
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
object3d::vec3d object3d::matMultiply(vec3d pin, mat4x4 mat)
{
    vec3d pout;

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

    sf::VertexArray triangle(sf::Triangles, 3);

    triangle[0].color = col;
    triangle[1].color = col; 
    triangle[2].color = col; 

    triangle[0].position = sf::Vector2f(tri.v[0].x, tri.v[0].y);
    triangle[1].position = sf::Vector2f(tri.v[1].x, tri.v[1].y);
    triangle[2].position = sf::Vector2f(tri.v[2].x, tri.v[2].y);

    texture.draw(triangle);

    sf::Color col2 = c_color(Light_Blue);

    sf::Vertex line1[2], line2[2], line3[2];
    
    line1[0].color = col2; line1[1].color = col2;
    line2[0].color = col2; line2[1].color = col2;
    line3[0].color = col2; line3[1].color = col2;

    line1[0].position = sf::Vector2f(tri.v[0].x, tri.v[0].y); line1[1].position = sf::Vector2f(tri.v[1].x, tri.v[1].y);
    line2[0].position = sf::Vector2f(tri.v[1].x, tri.v[1].y); line2[1].position = sf::Vector2f(tri.v[2].x, tri.v[2].y);
    line3[0].position = sf::Vector2f(tri.v[2].x, tri.v[2].y); line3[1].position = sf::Vector2f(tri.v[0].x, tri.v[0].y);

    texture.draw(line1,2,sf::Lines);
    texture.draw(line2,2,sf::Lines);
    texture.draw(line3,2,sf::Lines);
}
