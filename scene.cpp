//
//  Framework for a raytracer
//  File: scene.cpp
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

#include "scene.h"
#include "material.h"

Color Scene::trace(const Ray &ray, unsigned int mode)
{
    // Find hit object and distance
    Hit min_hit(std::numeric_limits<double>::infinity(),Vector());
    Object *obj = NULL;
    for (unsigned int i = 0; i < objects.size(); ++i) {
        Hit hit(objects[i]->intersect(ray));
        if (hit.t<min_hit.t) {
            min_hit = hit;
            obj = objects[i];
        }
    }

    // No hit? Return background color.
    if (!obj) return Color(0.0, 0.0, 0.0);

    if(mode == 1) // zbuffer
    {
        // simple normalization using min dist = 100, max dist = 10000
        float dist = (10 - min_hit.t) / (1000 - 10);
        return Vector(dist, dist, dist);
    }
    else if(mode == 2) // nbuffer
    {
        // simple normalization again
        return Vector((min_hit.N.x+1)/2, (min_hit.N.y+1)/2, (min_hit.N.z+1)/2);
    }

    Material *material = obj->material;            //the hit objects material
    Point hit = ray.at(min_hit.t);                 //the hit point
    Vector N = min_hit.N;                          //the normal at hit point
    Vector V = -ray.D;                             //the view vector

    // phong
    // https://en.wikipedia.org/wiki/Phong_reflection_model

    Vector intensity = Vector(0 , 0 , 0);

    for (std::vector<Light*>::iterator it = lights.begin() ; it != lights.end(); ++it)
    {
        Light* light = *it;
        
        Vector lm = light->position - hit;
        lm = lm.normalized();

        float lmDotN = lm.dot(N);
        Vector rm = 2 * lmDotN * N - lm;
        rm = rm.normalized();

        float difftIntensity = material->kd * lmDotN;

        // otherwise, negatives will be turned into positive with pair powers
        if(rm.dot(V) < 0)  rm = Vector(0 , 0 , 0);

        float specIntensity = material->ks * pow(rm.dot(V) , material->n);

        if(difftIntensity < 0)  difftIntensity = 0;
        if(specIntensity < 0)  specIntensity = 0;

        float lightIntensity = difftIntensity + specIntensity;

        intensity += lightIntensity;
    }
    
    Color color = material->color * intensity;

    return color;
}

void Scene::render(Image &img)
{
    int w = img.width();
    int h = img.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x+0.5, h-1-y+0.5, 0);
            Ray ray(eye, (pixel-eye).normalized());
            Color col = trace(ray, 0);
            col.clamp();
            img(x,y) = col;
        }
    }
}

void Scene::renderZBuffer(Image &img)
{
    int w = img.width();
    int h = img.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x+0.5, h-1-y+0.5, 0);
            Ray ray(eye, (pixel-eye).normalized());
            Color col = trace(ray, 1);
            col.clamp();
            img(x,y) = col;
        }
    }
}

void Scene::renderNBuffer(Image &img)
{
    int w = img.width();
    int h = img.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x+0.5, h-1-y+0.5, 0);
            Ray ray(eye, (pixel-eye).normalized());
            Color col = trace(ray, 2);
            col.clamp();
            img(x,y) = col;
        }
    }
}

void Scene::addObject(Object *o)
{
    objects.push_back(o);
}

void Scene::addLight(Light *l)
{
    lights.push_back(l);
}

void Scene::setEye(Triple e)
{
    eye = e;
}
