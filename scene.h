//
//  Framework for a raytracer
//  File: scene.h
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Authors:
//    Maarten Everts
//    Jasper van de Gronde
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#ifndef SCENE_H_KNBLQLP6
#define SCENE_H_KNBLQLP6

#include <vector>
#include "triple.h"
#include "light.h"
#include "object.h"
#include "image.h"

class Scene
{
private:
    std::vector<Object*> objects;
    std::vector<Light*> lights;
    Triple eye;
    Light recursiveReflection(Ray ray, unsigned int depth, unsigned int maxDepth, bool shadows);
    Color totalColor(const Ray &ray, Hit min_hit, std::vector<Light*> lights, Material *material, bool shadows, bool reflection, bool withDiff);
    void phong(Point hit, Point lightPosition, Vector N, Vector V, Material *mat, float &difftIntensity, float &specIntensity);

public:
    Color trace(const Ray &ray, unsigned int mode, bool shadows, bool reflection);
    void render(Image &img, bool shadows, bool reflection);
    void renderZBuffer(Image &img, bool shadows);
    void renderNBuffer(Image &img, bool shadows);
    void addObject(Object *o);
    void addLight(Light *l);
    void setEye(Triple e);
    unsigned int getNumObjects() { return objects.size(); }
    unsigned int getNumLights() { return lights.size(); }
};

#endif /* end of include guard: SCENE_H_KNBLQLP6 */
