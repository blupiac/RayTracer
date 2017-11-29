//
//  Framework for a raytracer
//  File: cylinder.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Maarten Everts
//    Jasper van de Gronde
//    Bernard Lupiac
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#include "cylinder.h"
#include <iostream>
#include <math.h>

/************************** Sphere **********************************/

Hit Cylinder::intersect(const Ray &ray)
{
    // http://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf
    // section 5.3.7

    float t;

    Vector d = pa - pb;
    Vector m = ray.O - pb;
    Vector n = ray.D;
    
    float md = m.dot(d);
    float nd = n.dot(d);
    float dd = d.dot(d);

    // ray outside endcaps?
    if(md < 0.0f && md + nd < 0.0f) return Hit::NO_HIT();
    if(md > dd && md + nd > dd)     return Hit::NO_HIT();

    float nn = n.dot(n);
    float mn = m.dot(n);

    float a = dd * nn - nd * nd;
    float k = m.dot(m) - r * r;
    float c = dd * k - md * md;

    // ray parallel to axis?
    if(Abs(a) < 0.001) // epsilon
    {
        if(c > 0.0f)    return Hit::NO_HIT();
        if(md > dd)     t = (nd - mn) / nn;
        else            t = 0.0f;

        Vector i = ray.O + t * ray.D;

        Vector N = i - position;
        N = N.normalized();

        return Hit(t,N);
    }

    float b = dd * mn - nd * md;
    float discr = b * b - a * c;

    if(discr < 0.0f)    return Hit::NO_HIT();

    t = (-b -sqrt(discr)) / a;

    if(t < 0.0f || t > 1.0f)    return Hit::NO_HIT();

    if(md + t * nd < 0.0f)
    {
        if(nd <= 0.0f) return Hit::NO_HIT();
        t = (dd - md) / nd;

        if(k + 2 * t * (mn * t * nn) > 0.0f)
        {
            return Hit::NO_HIT();
        } 
    }
    else if(md + t * nd > dd)
    {
        if(nd >= 0.0f)  return 0;
        t = (dd - md) / nd;

        if(kk + dd – 2 * md + t * (2 * (mn – nd) + t * nn) > 0.0f)
        {
            return Hit::NO_HIT();
        } 
    }

    Vector i = ray.O + t * ray.D;

    Vector N = i - position;
    N = N.normalized();

    return Hit(t,N);
}