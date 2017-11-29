//
//  Framework for a raytracer
//  File: cylinder.h
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

#ifndef CYLINDER_H
#define CYLINDER_H

#include "object.h"

class Cylinder : public Object
{
public:
    Cylinder(Point pa, Point pb, double r) : pa(pa), pb(pb), r(r) { }

    virtual Hit intersect(const Ray &ray);

    const Point pa, pb;
    const double r;
};

#endif