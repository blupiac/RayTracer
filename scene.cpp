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

Color Scene::trace(const Ray &ray, unsigned int mode, bool shadows, bool reflection, unsigned int depth, unsigned int maxDepth, GoochParams gp)
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
        
        Color reflCol = trace(reflectRay, mode, shadows, reflection, depth + 1, maxDepth, gp);

        Color normalCol = totalColor(ray, min_hit, lights, obj->angle, obj->material, shadows, false, mode, gp);
        return normalCol + reflCol * obj->material->ks;
    }
    else
    {
        return totalColor(ray, min_hit, lights, obj->angle, obj->material, shadows, reflection, mode, gp);
    }

}

Color Scene::totalColor(const Ray &ray, Hit min_hit, std::vector<Light*> lights, float angle, Material *material, bool shadows, bool reflection, unsigned int mode, GoochParams gp)
{

    Point hit = ray.at(min_hit.t);                 //the hit point
    Vector N = min_hit.N;                          //the normal at hit point
    Vector V = -ray.D;                             //the view vector

    Vector intensity = Vector(0 , 0 , 0);

    if(mode == 3 && N.dot(V) < 0.2)
    {
        return Color(0.0, 0.0, 0.0);
    }

    for (std::vector<Light*>::iterator it = lights.begin() ; it != lights.end(); ++it)
    {
        Light* light = *it;

        Triple lightIntensity;

        float difftIntensity, specIntensity;
        phong(hit, light->position, N, V, material, difftIntensity, specIntensity);

        if(mode == 0) // phong
        {
            lightIntensity = light->color * (difftIntensity + specIntensity);
        }
        if(mode == 3) // gooch
        {
            Vector L = light->position - hit;
            L = L.normalized();
            Triple kd = light->color * material->color * material->kd;

            Triple kBlue = Triple(0, 0, gp.b);
            Triple kYellow = Triple(gp.y, gp.y, 0);

            Triple kCool = kBlue + gp.alpha * kd;
            Triple kWarm = kYellow + gp.beta * kd;

            lightIntensity = kCool * (1 - N.dot(L)) / 2.0 + kWarm * (1 + N.dot(L)) / 2.0;
            lightIntensity += specIntensity;
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
    
    Color color;

    if(material->texture != NULL)
    {
        Image* tex = material->texture;
        color = getTexColor(tex, N, angle) * intensity;
    }
    else if(mode == 0)
    {
        color = material->color * intensity;
    }
    else if(mode == 3)
    {
        color = intensity;
    }

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

void Scene::render(Image &img, Camera *cam, bool shadows, bool reflection, unsigned int renderType, unsigned int aaFactor, GoochParams gp)
{
    std::clock_t tInit = std::clock();
    std::cout << "Rendering begins." << std::endl;

    int w = img.width();
    int h = img.height();
    float pixSize = cam->up.length();
    Vector lookDir = cam->center - cam->eye;
    Vector xDir = lookDir.cross(cam->up);
    Vector yDir = xDir.cross(lookDir);
    xDir = xDir.normalized();
    yDir = yDir.normalized();
    Vector start = cam->center - (pixSize * w / 2.0) * xDir - (pixSize * h / 2.0) * yDir;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Color totalCol(0.0, 0.0, 0.0);
            for(unsigned int i = 1; i < (aaFactor + 1); i++)
            {
                for(unsigned int j = 1; j < (aaFactor + 1); j++)
                {
                    float aaX = i * (xDir.x + yDir.x) / (float) (aaFactor + 1.0);
                    float aaY = j * (xDir.y + yDir.y) / (float) (aaFactor + 1.0);
                    Point pixel = start + pixSize * ((x + aaX) * xDir + (h - y + aaY) * yDir);
                    Ray ray(cam->eye, (pixel-cam->eye).normalized());
                    Color col = trace(ray, renderType, shadows, reflection, 0, 2, gp);
                    col.clamp();
                    totalCol += col;
                }
            }
            img(x,y) = totalCol / (float) (aaFactor * aaFactor);
        }
    }

    std::cout << "Rendering ended: " << (std::clock() - tInit) / (double)CLOCKS_PER_SEC << " seconds" << std::endl;
}

Color Scene::getTexColor(const Image *tex, Vector N, float angle)
{
    float u = 1 - (0.5 + (atan2(N.z, N.x) + angle) / (2 * M_PI));
    float v = 0.5 - asin(N.y) / M_PI;

    return tex->colorAt(u, v);
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