#pragma once

//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include "math.hpp"




//////////////////////////////////////////////////////////////////
/// \brief Projects points on a 2D surface on to a sphere
//////////////////////////////////////////////////////////////////
class stereograph
{
private:
    /* data */
public:
    stereograph(/* args */);
    ~stereograph();

    struct vec3d{float x,y,z;};
    struct point{float x,y;};

    std::vector<vec3d> sphere;
    std::vector<point> map;
};

