#include "obj3d.hpp"


//////////////////////////////////////////////////////////////////
rend3d::object3d::object3d()
{
    rotateObject(0,0,0);
    m_aspectRatio = ((float)sf::VideoMode::getDesktopMode().width) / ((float)sf::VideoMode::getDesktopMode().height);

    // Temporary variables to initiat the projection matrix
    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = 90.0f;
    float fFovRad = 1.0f / mat_tan(fFov * 0.5f/ 180.0f * 3.14159f);

    // This is the projection matrix
    m_matProj.m[0][0] = m_aspectRatio * fFovRad;            // Need to multiply the x by the aspect ratio, and FOV = 1/tan(theta/2)
    m_matProj.m[1][1] = fFovRad;                            // FOV = 1/tan(theta/2)
    m_matProj.m[2][2] = fFar / (fFar- fNear);               // Ratio for the near and far plane
    m_matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);   // Need to subtract out the near plane
    m_matProj.m[2][3] = 1.0f;
    m_matProj.m[3][3] = 0.0f;
}


//////////////////////////////////////////////////////////////////
void rend3d::object3d::rotateObject(float u, float v, float w)
{
    // Update the rotation matrices
    m_matRotU.m[0][0] = 1;             //    1    0    0    0
    m_matRotU.m[1][1] = mat_cos(u);    //    0   cos -sin   0
    m_matRotU.m[1][2] = mat_sin(u);    //    0   sin  cos   0     for dividing by z when
    m_matRotU.m[2][1] = -mat_sin(u);   //    0    0    0    1 <<  scaling distant objects
    m_matRotU.m[2][2] = mat_cos(u);
    m_matRotU.m[3][3] = 1;

    m_matRotV.m[0][0] = mat_cos(v);    //   cos   0   sin   0
    m_matRotV.m[1][1] = 1;             //    0    1    0    0
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
void rend3d::object3d::drawMesh(sf::RenderTexture& texture, sf::Vector2i res, float zoom)
{

    // Create a vector for the camera and light direction (not position)
    vector3 cam(0,0,0);
    vector3 light(1,1,1);

    light = normalize(light);

    std::vector<triangle> buffer;

    for(triangle i: mesh)
    {
        triangle triIn;

        // Rotate the view
        triIn.v[0] = matMultiply(matMultiply(matMultiply(i.v[0], m_matRotU), m_matRotV), m_matRotW);
        triIn.v[1] = matMultiply(matMultiply(matMultiply(i.v[1], m_matRotU), m_matRotV), m_matRotW);
        triIn.v[2] = matMultiply(matMultiply(matMultiply(i.v[2], m_matRotU), m_matRotV), m_matRotW);

        // Push farther into screen so we can see it
        triIn.v[0].z += zoom;
        triIn.v[1].z += zoom;
        triIn.v[2].z += zoom;

        // Get the normal vector to the triangle
        vector3 norm = triNorm(triIn);

        // Find the shading of the triangle by its likeness with the light vector
        //float color = dotProd(norm,light);
        
        // Only draw if the triangle is visable
        // position of any point on the triangle - cam position will give the vector to check against the normal
        if(dotProd(norm,triIn.v[0],cam) > 0)
        {
            buffer.push_back(triIn);
            // drawTriangle(texture, triIn, sf::Color(65+(color*50),95+(color*100),120+(color*70)));
        }
    }
    
    // sort by z midpoint
    std::sort(buffer.begin(),buffer.end(), [](triangle &t1, triangle &t2)
    {
            float z1 = (t1.v[0].z + t1.v[1].z + t1.v[2].z)/3.0f;
            float z2 = (t2.v[0].z + t2.v[1].z + t2.v[2].z)/3.0f;
            return z1 > z2;
    });


    //project and draw
    for(triangle tri : buffer)
    {
        vector3 norm = triNorm(tri);
        float color = dotProd(norm,light);

            for (int i = 0; i < 3; i++)
            {
                // Project 2D
                tri.v[i] = matMultiply(tri.v[i], m_matProj);

                // Why i have to multiply this by the aspect ratio i have no idea
                // Center on screen
                tri.v[i].x += 1*m_aspectRatio;
                tri.v[i].y += 1/m_aspectRatio;

                // Why these are swapped i have no idea
                // Scale to size
                tri.v[i].x *= 0.5f * res.y;
                tri.v[i].y *= 0.5f * res.x;
            }

        drawTriangle(texture, tri, sf::Color(65+(color*50),95+(color*75),120+(color*75)));
    }
}


//////////////////////////////////////////////////////////////////
void rend3d::object3d::loadObj(std::string fileName)
{
    std::ifstream obj(fileName);
    if (!obj.is_open())
    {
        std::cout << "error: cannot load 3d object" << std::endl;
        return;
    }
    
        std::vector<vector3> verts;

        while(!obj.eof())
        {
            char line[128];
            obj.getline(line,128);

            std::strstream stream;
            stream << line;

            char junk;
            if(line[0] == 'v')
            {
                vector3 v;
                stream >> junk >> v.x >> v.y >> v.z;
                verts.push_back(v);
            }
            if(line[0] == 'f')
            {
                int f[3];
                stream >> junk >> f[0] >> f[1] >> f[2];
                mesh.push_back(rend3d::triangle(verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]));
            }

        }
}

//////////////////////////////////////////////////////////////////
vector3 rend3d::object3d::triNorm(triangle tri)
{
    // Get the vectors representing the lines of the triangle branching from 0
    vector3 line1(tri.v[1].x - tri.v[0].x, tri.v[1].y - tri.v[0].y, tri.v[1].z - tri.v[0].z);
    vector3 line2(tri.v[2].x - tri.v[0].x, tri.v[2].y - tri.v[0].y, tri.v[2].z - tri.v[0].z);

    // Take the cross product of those two lines to get the normal vector
    vector3 norm = crossProd(line2,line1);

    return normalize(norm);
}


//////////////////////////////////////////////////////////////////
vector3 rend3d::object3d::matMultiply(vector3 vin, mat4x4 mat)
{
    vector3 vout;

    vout.x = vin.x * mat.m[0][0] + vin.y * mat.m[1][0] + vin.z * mat.m[2][0] + mat.m[3][0];
    vout.y = vin.x * mat.m[0][1] + vin.y * mat.m[1][1] + vin.z * mat.m[2][1] + mat.m[3][1];
    vout.z = vin.x * mat.m[0][2] + vin.y * mat.m[1][2] + vin.z * mat.m[2][2] + mat.m[3][2];
    float w = vin.x * mat.m[0][3] + vin.y * mat.m[1][3] + vin.z * mat.m[2][3] + mat.m[3][3];

    if ( w != 0.0f) {vout.x /= w; vout.y /= w; vout.z /= w;}
    
    return vout;
}



//////////////////////////////////////////////////////////////////
void rend3d::object3d::drawTriangle(sf::RenderTexture& texture, triangle tri, sf::Color col) 
{

    sf::VertexArray triangle(sf::Triangles, 3);

    triangle[0].color = col;
    triangle[1].color = col; 
    triangle[2].color = col; 

    triangle[0].position = sf::Vector2f(tri.v[0].x, tri.v[0].y);
    triangle[1].position = sf::Vector2f(tri.v[1].x, tri.v[1].y);
    triangle[2].position = sf::Vector2f(tri.v[2].x, tri.v[2].y);

    texture.draw(triangle);

    // Draw the lines
    // sf::Color col2 = c_color(Light_Blue);

    // sf::Vertex line1[2], line2[2], line3[2];
    
    // line1[0].color = col2; line1[1].color = col2;
    // line2[0].color = col2; line2[1].color = col2;
    // line3[0].color = col2; line3[1].color = col2;

    // line1[0].position = sf::Vector2f(tri.v[0].x, tri.v[0].y); line1[1].position = sf::Vector2f(tri.v[1].x, tri.v[1].y);
    // line2[0].position = sf::Vector2f(tri.v[1].x, tri.v[1].y); line2[1].position = sf::Vector2f(tri.v[2].x, tri.v[2].y);
    // line3[0].position = sf::Vector2f(tri.v[2].x, tri.v[2].y); line3[1].position = sf::Vector2f(tri.v[0].x, tri.v[0].y);

    // texture.draw(line1,2,sf::Lines);
    // texture.draw(line2,2,sf::Lines);
    // texture.draw(line3,2,sf::Lines);
}
