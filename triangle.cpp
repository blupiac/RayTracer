//
//  Framework for a raytracer
//  File: triangle.cpp
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

#include "triangle.h"
#include <iostream>
#include <math.h>

/************************** Triangle **********************************/

Hit Triangle::intersect(const Ray &ray)
{
    // http://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf
    // section 5.3.4

    Vector pq = ray.D;
    Vector pa = a - ray.O;
    Vector pb = b - ray.O;
    Vector pc = c - ray.O;

    float u = pq.dot(pc.cross(pb));
    if(u < 0.0f) return Hit::NO_HIT();
    float v = pq.dot(pa.cross(pc));
    if(v < 0.0f) return Hit::NO_HIT();
    float w = pq.dot(pb.cross(pa));
    if(w < 0.0f) return Hit::NO_HIT();

    float denom = 1.0f / (u + v + w);
    u *= denom;
    v *= denom;
    w *= denom;

    Point i = u*a + v*b + w*c;
    Vector tv = i - ray.O;
    float t = tv.length();

    // https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal

    Vector ba = b - a;
    Vector ca = c - a;
    Vector N;
    N.x = ba.y * ca.z - ba.z * ca.y;
    N.y = ba.z * ca.x - ba.x * ca.z;
    N.z = ba.x * ca.y - ba.y * ca.x;

    return Hit(t,N);
}