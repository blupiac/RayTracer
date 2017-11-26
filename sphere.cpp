//
//  Framework for a raytracer
//  File: sphere.cpp
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

#include "sphere.h"
#include <iostream>
#include <math.h>

/************************** Sphere **********************************/

Hit Sphere::intersect(const Ray &ray)
{
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    * 
    * Insert calculation of ray/sphere intersection here. 
    *
    * You have the sphere's center (C) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return Hit::NO_HIT().
    * Otherwise, return an instance of Hit() with the distance of the
    * intersection point from the ray origin as t and the normal ad N (see example).
    ****************************************************/

    // http://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf
    // section 5.3.2

    Vector m = ray.O - position; 
    float b = m.dot(ray.D); 
    float c = m.dot(m) - r * r; 

    // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0) 
    if (c > 0.0f && b > 0.0f) return Hit::NO_HIT(); 
    float discr = b*b - c; 

    // A negative discriminant corresponds to ray missing sphere 
    if (discr < 0.0f) return Hit::NO_HIT(); 

    // Ray now found to intersect sphere, compute smallest t value of intersection
    float t = -b - sqrt(discr); 

    // If t is negative, ray started inside sphere so clamp t to zero 
    if (t < 0.0f) t = 0.0f; 

    Vector i = ray.O + t * ray.D;

    Vector N = i - position;
    N = N.normalized();

    return Hit(t,N);
}