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

#ifndef BOX_H
#define BOX_H

#include "object.h"

class Box : public Object
{
public:
    Box(Point ll, Point ur) : ll(ll), ur(ur) { }

    virtual Hit intersect(const Ray &ray);

    // lower left front, upper right back
    const Point ll, ur;
};

#endif /* end of include guard: TRIANGLE_H */
