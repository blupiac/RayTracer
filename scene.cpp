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

Color Scene::trace(const Ray &ray, unsigned int mode, bool shadows, bool reflection, unsigned int depth, unsigned int maxDepth)
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

    if(reflection && (depth < maxDepth))
    {
        Vector reflDir = ray.D - 2 * (ray.D.dot(min_hit.N) * min_hit.N);
        Ray reflectRay(ray.at(min_hit.t) + reflDir * 0.1, reflDir);
        
        Color reflCol = trace(reflectRay, mode, shadows, reflection, depth + 1, maxDepth);

        Color normalCol = totalColor(ray, min_hit, lights, obj->material, shadows, false);
        return normalCol + reflCol * obj->material->ks;
    }
    else
    {
        return totalColor(ray, min_hit, lights, obj->material, shadows, reflection);
    }
}

Color Scene::totalColor(const Ray &ray, Hit min_hit, std::vector<Light*> lights, Material *material, bool shadows, bool reflection)
{

    Point hit = ray.at(min_hit.t);                 //the hit point
    Vector N = min_hit.N;                          //the normal at hit point
    Vector V = -ray.D;                             //the view vector

    Vector intensity = Vector(0 , 0 , 0);

    for (std::vector<Light*>::iterator it = lights.begin() ; it != lights.end(); ++it)
    {
        Light* light = *it;

        float difftIntensity, specIntensity;
        phong(hit, light->position, N, V, material, difftIntensity, specIntensity);

        Triple lightIntensity;

        lightIntensity = light->color * (difftIntensity + specIntensity);

        if(shadows)
        {
            bool shadowed = false;
            unsigned int i = 0;
            Vector dir = (light->position - hit).normalized();
            Ray lightRay(hit + dir * 0.1, dir);
            while (!shadowed && i < objects.size()) 
            {
                Hit hit(objects[i]->intersect(lightRay));
                if (hit.no_hit == false) 
                {
                    shadowed = true;
                }
                i++;
            }
            if(shadowed == true)
            {
                lightIntensity *= 0.2;
            }
        }

        intensity += lightIntensity;
    }
    
    Color color = material->color * intensity;

    return color;
}

// phong
// https://en.wikipedia.org/wiki/Phong_reflection_model
void Scene::phong(Point hit, Point lightPosition, Vector N, Vector V, Material *mat, float &difftIntensity, float &specIntensity)
{
    Vector lm = lightPosition - hit;
    lm = lm.normalized();

    float lmDotN = lm.dot(N);
    Vector rm = 2 * lmDotN * N - lm;
    rm = rm.normalized();

    difftIntensity = mat->kd * lmDotN;

    // otherwise, negatives will be turned into positive with pair exponents
    if(rm.dot(V) < 0)  rm = Vector(0 , 0 , 0);

    specIntensity = mat->ks * pow(rm.dot(V) , mat->n);

    if(difftIntensity < 0)  difftIntensity = 0;
    if(specIntensity < 0)  specIntensity = 0;
}

void Scene::render(Image &img, Camera *cam, bool shadows, bool reflection, unsigned int renderType, unsigned int aaFactor)
{
    int w = cam->xSize;
    int h = cam->ySize;
    float startX = cam->center.x - w / 2.0;
    float startY = cam->center.y - h / 2.0;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Color totalCol(0.0, 0.0, 0.0);
            for(unsigned int i = 1; i < (aaFactor + 1); i++)
            {
                for(unsigned int j = 1; j < (aaFactor + 1); j++)
                {
                    Point pixel(startX + x+(i / (float) (aaFactor + 1.0)), startY + h-1-y+(j / (float) (aaFactor + 1.0)), 0);
                    Ray ray(cam->eye, (pixel-cam->eye).normalized());
                    Color col = trace(ray, renderType, shadows, reflection, 0, 2);
                    col.clamp();
                    totalCol += col;
                }
            }
            img(x,y) = totalCol / (float) (aaFactor * aaFactor);
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
