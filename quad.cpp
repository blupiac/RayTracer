//
//  Framework for a raytracer
//  File: quad.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Bernard Lupiac
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#include "quad.h"
#include "triangle.h"
#include <iostream>
#include <math.h>

/************************** Sphere **********************************/

Hit Quad::intersect(const Ray &ray)
{
    // http://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf
    // section 5.3.5

    Triangle t1(a, b, c);
    Triangle t2(a, c, d);

    Vector e1 = b - d;
    Vector e2 = a - c;
    Vector n = e1.cross(e2);

    const Hit h1 = t1.intersect(ray);
    const Hit h2 = t2.intersect(ray);

    if(!h1.no_hit)
    {
        return Hit(h1.t, n);
    }
    else if(!h2.no_hit)
    {
        return Hit(h2.t, n);
    }
    else
    { 
        return Hit::NO_HIT();
    }
}