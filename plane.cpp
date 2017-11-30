//
//  Framework for a raytracer
//  File: box.cpp
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

#include "plane.h"
#include <iostream>
#include <math.h>

/************************** Sphere **********************************/

Hit Plane::intersect(const Ray &ray)
{
    // http://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf
    // section 5.3.1

    float t = (d - n.dot(ray.O)) / n.dot(ray.D);

    std::cout << t << " " << n.dot(ray.O) << " " << n.dot(ray.D) << std::endl;

    if(t >= 0)
    {
        std::cout << "plane hit" << std::endl;
        return Hit(t, n);
    }

    return Hit::NO_HIT();
}