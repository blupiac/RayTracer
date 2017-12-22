//
//  Framework for a raytracer
//  File: mesh.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Bernard Lupiac
//    Tamy Boubekeur
//
//  Heavily inspired from one of my OpenGL projects, whose framework was
//  originally done by my 3D professor, Tamy Boubekeur
//  


#include "mesh.h"

/************************** Mesh **********************************/

Mesh::Mesh(std::string meshPath)
{
    ifstream in (meshPath.c_str ());
    if (!in) 
    {
        exit (1);
    }
    string offString;
    unsigned int sizeV, sizeT, tmp;
    in >> offString >> sizeV >> sizeT >> tmp;
    m_positions.resize (sizeV);
    m_triangles.resize (sizeT);
    for (unsigned int i = 0; i < sizeV; i++)
    {
        in >> m_positions[i].x;
        in >> m_positions[i].y;
        in >> m_positions[i].z;
    }
    int s;
    for (unsigned int i = 0; i < sizeT; i++) {
        in >> s;
        for (unsigned int j = 0; j < 3; j++)
        {
            in >> m_triangles[i][j];
        }
    }
    in.close ();

    std::cout << "Read: " << meshPath << " Points read: " << sizeV << " Triangles read: " << sizeT << std::endl;

    recomputeNormals ();
}

Hit Mesh::intersect(const Ray &ray)
{
    for (unsigned int i = 0 ; i < m_triangles.size(); ++i)
    {

        Triangle t(m_positions[m_triangles[i][0]],
                    m_positions[m_triangles[i][1]],
                    m_positions[m_triangles[i][2]]);

        const Hit h = t.intersect(ray);

        if(!h.no_hit)
        {
            return Hit(h.t, m_normals[i]);
        }
    }
    return Hit::NO_HIT();
}

// -------------- Helpers -------------------

void Mesh::recomputeNormals () {
    m_normals.clear ();
    std::vector<Vector> m_normalsPerPoint;
    m_normalsPerPoint.resize (m_positions.size (), Vector (0.f, 0.f, 0.f));
    m_normals.resize (m_triangles.size (), Vector (0.f, 0.f, 0.f));
    for (unsigned int i = 0; i < m_triangles.size (); i++) {
        Vector e01 = m_positions[m_triangles[i][1]] -  m_positions[m_triangles[i][0]];
        Vector e02 = m_positions[m_triangles[i][2]] -  m_positions[m_triangles[i][0]];
        Vector n = e01.cross(e02);
        Vector cross = n;
        n.normalize ();
        for (unsigned int j = 0; j < 3; j++)
        {
            // avec poids
            m_normalsPerPoint[m_triangles[i][j]] += n * std::asin(cross.length());
        }
    }
    for (unsigned int i = 0; i < m_normalsPerPoint.size (); i++)
        m_normalsPerPoint[i].normalize ();

    for (unsigned int i = 0; i < m_normals.size (); i++)
    {
        m_normals[i] = m_normalsPerPoint[m_triangles[i][0]] +
                        m_normalsPerPoint[m_triangles[i][1]] + 
                        m_normalsPerPoint[m_triangles[i][2]];
        m_normals[i] /= 3.0;
        m_normals[i] = m_normals[i].normalized();
    }

}

void Mesh::scaleTranslate()
{
    for (unsigned int i = 0; i < m_positions.size(); i++)
    {
        m_positions[i] *= size;
        m_positions[i] += position;
    }
}