//
//  Framework for a raytracer
//  File: box.h
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Maarten Everts
//    Jasper van de Gronde
//	  Bernard Lupiac
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#ifndef PLANE_H
#define PLANE_H

#include "object.h"

class Plane : public Object
{
public:
    Plane(float d, Vector n) : d(d), n(n) { }

    virtual Hit intersect(const Ray &ray);

    const float d;
    const Vector n;
};

#endif /* end of include guard: TRIANGLE_H */
