//
//  Framework for a raytracer
//  File: mesh.h
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Bernard Lupiac
//	  Tamy Boubekeur
//
//  Heavily inspired from one of my OpenGL projects, whose framework was
//  originally done by my 3D professor, Tamy Boubekeur
//	

#ifndef MESH_H
#define MESH_H

#include "object.h"
#include "triangle.h"
#include "meshtriangle.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>

class Mesh : public Object
{
public:
    Mesh(std::string meshPath);

    virtual Hit intersect(const Ray &ray);

    std::vector<Point> m_positions;
    std::vector<Vector> m_normals;
    std::vector<MeshTriangle> m_triangles;
    Triple position;
    float size;

private:
	void centerAndScaleToUnit ();
	void recomputeNormals ();
};

#endif /* end of include guard: MESH_H */