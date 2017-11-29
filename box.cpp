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

#include "box.h"
#include <iostream>
#include <math.h>

/************************** Sphere **********************************/

Hit Box::intersect(const Ray &ray)
{
    // http://www.r-5.org/files/books/computers/algo-list/realtime-3d/Christer_Ericson-Real-Time_Collision_Detection-EN.pdf
    // section 5.3.3

    float tmin = 0;
    float tmax = 1000000000000;

    for(int i = 0; i < 3; i++)
    {
        if(abs(ray.D.data[i]) < 0.0001)
        {
            if(ray.O.data[i] < ll.data[i] || ray.O.data[i] > ur.data[i]) return Hit::NO_HIT();
        }
        else
        {
            float ood = 1.0 / ray.D.data[i];
            float t1 = (ll.data[i] - ray.O.data[i]) * ood;
            float t2 = (ur.data[i] - ray.O.data[i]) * ood;

            if(t1 > t2)
            {
                float temp = t1;
                t1 = t2;
                t2 = temp;
            }

            if(t1 > tmin)   tmin = t1;
            if(t2 > tmax)   tmax = t2;

            if(tmin > tmax) return Hit::NO_HIT();
        }
    }

    Point i = ray.O + ray.D * tmin;
    Vector N;

    if(i.x == ll.x)
    {
        N = Vector(-1, 0, 0);
    }
    else if(i.y == ll.y)
    {
        N = Vector(0, -1, 0);
    }
    else if(i.z == ll.z)
    {
        N = Vector(0, 0, -1);
    }
    else if(i.x == ur.x)
    {
        N = Vector(1, 0, 0);
    }
    else if(i.y == ur.y)
    {
        N = Vector(0, 1, 0);
    }
    else if(i.z == ur.z)
    {
        N = Vector(0, 0, 1);
    }
    else
    {
        std::cerr << "Wrong intersection point on box collision." << std::endl;
    }

    return Hit(tmin,N);
}