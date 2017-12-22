//
//  Framework for a raytracer
//  File: quad.h
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

#ifndef QUAD_H
#define QUAD_H

#include "object.h"

class Quad : public Object
{
public:
    Quad(Point a, Point b, Point c, Point d) : a(a), b(b), c(c), d(d) { }

    virtual Hit intersect(const Ray &ray);

    const Point a, b, c, d;
};

#endif /* end of include guard: QUAD_H */