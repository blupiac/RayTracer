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

Color Scene::trace(const Ray &ray, unsigned int mode, bool shadows, bool reflection)
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

    return totalColor(ray, min_hit, lights, obj->material, shadows, reflection, true);
}

Color Scene::totalColor(const Ray &ray, Hit min_hit, std::vector<Light*> lights, Material *material, bool shadows, bool reflection, bool withDiff)
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

        // https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector
        if(reflection)
        {
            Vector reflDir = ray.D - 2 * (ray.D.dot(min_hit.N) * min_hit.N);
            Ray reflectRay(hit + reflDir * 0.1, reflDir);
            
            Light reflLight = recursiveReflection(reflectRay, 1, 2, shadows);
            std::vector<Light*> reflLights;
            reflLights.push_back(&reflLight);

            Color resCol = totalColor(ray, min_hit, reflLights, material, shadows, false, false);
            lightIntensity = light->color * (difftIntensity + specIntensity) + resCol;
        }
        else
        {
            if(withDiff)
                lightIntensity = light->color * (difftIntensity + specIntensity);
            else
                lightIntensity = light->color * specIntensity;
        }

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

Light Scene::recursiveReflection(Ray ray, unsigned int depth, unsigned int maxDepth, bool shadows)
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
    if (!obj) return Light(Point(0.0, 0.0, 0.0), Color(0.0, 0.0, 0.0));

    Point intersectpoint = ray.at(min_hit.t);

    if(depth == maxDepth)
    {
        Color col = totalColor(ray, min_hit, lights, obj->material, shadows, false, true);
        return Light(intersectpoint, col);
    }
    else
    {
        // https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector        
        Vector reflDir = ray.D - 2 * (ray.D.dot(min_hit.N) * min_hit.N);
        Ray reflectRay(intersectpoint + reflDir * 0.1 , reflDir);
        
        Light reflLight = recursiveReflection(reflectRay, depth + 1, maxDepth, shadows);
        std::vector<Light*> reflLights;
        reflLights.push_back(&reflLight);
        
        Color resCol = totalColor(ray, min_hit, reflLights, obj->material, shadows, false, false);
        return Light(intersectpoint, resCol);
    }
}

void Scene::render(Image &img, bool shadows, bool reflection)
{
    int w = img.width();
    int h = img.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x+0.5, h-1-y+0.5, 0);
            Ray ray(eye, (pixel-eye).normalized());
            Color col = trace(ray, 0, shadows, reflection);
            col.clamp();
            img(x,y) = col;
        }
    }
}

void Scene::renderZBuffer(Image &img, bool shadows)
{
    int w = img.width();
    int h = img.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x+0.5, h-1-y+0.5, 0);
            Ray ray(eye, (pixel-eye).normalized());
            Color col = trace(ray, 1, shadows, false);
            col.clamp();
            img(x,y) = col;
        }
    }
}

void Scene::renderNBuffer(Image &img, bool shadows)
{
    int w = img.width();
    int h = img.height();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Point pixel(x+0.5, h-1-y+0.5, 0);
            Ray ray(eye, (pixel-eye).normalized());
            Color col = trace(ray, 2, shadows, false);
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
