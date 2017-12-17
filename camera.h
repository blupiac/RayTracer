//
//  Framework for a raytracer
//  File: camera.h
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

#ifndef CAMERA_H
#define CAMERA_H

#include "triple.h"

class Camera
{
public:
    Camera(Triple eye, Triple center, Triple up, unsigned int xSize, unsigned int ySize) :
            eye(eye), center(center), up(up), xSize(xSize), ySize(ySize)
    { }

    Triple eye, center, up;
    unsigned int xSize, ySize;
};

#endif