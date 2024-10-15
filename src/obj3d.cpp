#include "obj3d.hpp"


//////////////////////////////////////////////////////////////////
rend3d::object3d::object3d()
{
    u = 0, v = 0, w = 0 , x = 0 , y = 0 , z = 0;
    update();
    m_aspectRatio = ((float)sf::VideoMode::getDesktopMode().width) / ((float)sf::VideoMode::getDesktopMode().height);

    m_matProj = project_matrix(90.0f,m_aspectRatio,0.1f,1000.0f);
    
}


//////////////////////////////////////////////////////////////////
void rend3d::object3d::update()
{ 
    m_matLook = translate_matrix(0,0,6);
    m_matPos = translate_matrix(x,y,z);
    m_matRot = rotate_y_matrix(v);
}


//////////////////////////////////////////////////////////////////
void rend3d::object3d::draw(sf::RenderTexture& texture, sf::Vector2i res)
{
    // Create a vector for the camera and light direction (not position)
    vec3 cam(0,0,0);
    vec3 light(1,1,1);
    light = light.norm();


    vec3 look = vec3(0,0,1) * m_matRot;
    vec3 move = vec3(0,0,0) * m_matPos;
    
    pointTarget = move + look;

    mat4x4 pointMat = point_matrix(move,pointTarget,pointUp);
    mat4x4 viewMat = view_matrix(pointMat);

    std::vector<triangle> buffer;

    for(triangle i: mesh)
    {
        triangle triIn;

        triIn.v[0] = i.v[0] * m_matLook;
        triIn.v[1] = i.v[1] * m_matLook;
        triIn.v[2] = i.v[2] * m_matLook;

        triIn.v[0] = triIn.v[0] * viewMat;
        triIn.v[1] = triIn.v[1] * viewMat;
        triIn.v[2] = triIn.v[2] * viewMat;

        // Get the normal vector to the triangle
        vec3 norm = triIn.norm();
        
        // Only draw if the triangle is visable
        // position of any point on the triangle - cam position will give the vector to check against the normal
        if(norm.dot(triIn.v[0] - cam) > 0) buffer.push_back(triIn);
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
        vec3 norm = tri.norm();
        float color = norm.dot(light);

            for (int i = 0; i < 3; i++)
            {
                // Project 2D
                tri.v[i] = tri.v[i] * m_matProj;

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
void rend3d::object3d::load(std::string fileName)
{
    std::ifstream obj(fileName);
    if (!obj.is_open())
    {
        std::cout << "error: cannot load 3d object" << std::endl;
        return;
    }
    
    std::vector<vec3> verts;

    while(!obj.eof())
    {
        char line[128];
        obj.getline(line,128);

        std::strstream stream;
        stream << line;

        char junk;
        if(line[0] == 'v')
        {
            vec3 v;
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
void rend3d::object3d::drawTriangle(sf::RenderTexture& texture, triangle tri, sf::Color col) 
{

    // for cliping count points outside of view,
    // 2 points in view means need to create another triangle
    // for tris that clip 2 or more edges itterate through all edges and clip it multiple times
    sf::VertexArray triangle(sf::Triangles, 3);

    triangle[0].color = col;
    triangle[1].color = col; 
    triangle[2].color = col; 

    triangle[0].position = sf::Vector2f(tri.v[0].x, tri.v[0].y);
    triangle[1].position = sf::Vector2f(tri.v[1].x, tri.v[1].y);
    triangle[2].position = sf::Vector2f(tri.v[2].x, tri.v[2].y);

    texture.draw(triangle);


    // // Draw the lines
    // sf::Color col2 = c_color(White);

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
