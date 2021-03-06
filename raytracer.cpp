//
//  Framework for a raytracer
//  File: raytracer.cpp
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Author: Maarten Everts
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#include "raytracer.h"
#include "object.h"
#include "sphere.h"
#include "triangle.h"
#include "plane.h"
#include "quad.h"
#include "mesh.h"
#include "material.h"
#include "light.h"
#include "camera.h"
#include "image.h"
#include "yaml/yaml.h"
#include <ctype.h>
#include <fstream>
#include <assert.h>

// Functions to ease reading from YAML input
void operator >> (const YAML::Node& node, Triple& t);
Triple parseTriple(const YAML::Node& node);

void operator >> (const YAML::Node& node, Triple& t)
{
    assert(node.size()==3);
    node[0] >> t.x;
    node[1] >> t.y;
    node[2] >> t.z;
}

Triple parseTriple(const YAML::Node& node)
{
    Triple t;
    node[0] >> t.x;
    node[1] >> t.y;
    node[2] >> t.z;	
    return t;
}

Material* Raytracer::parseMaterial(const YAML::Node& node)
{
    Material *m = new Material();

    if (node.FindValue("texture"))
    {
        std::string texturePath;
        node["texture"] >> texturePath;
        Image* tex = new Image(texturePath.c_str());

        if(tex->width() == 0 && tex->height() == 0)
        {
            std::cerr << "Error reading texture " << texturePath << " : width and height equals 0." << std::endl;
        }

        m->texture = tex;
    }
    else
    {
        node["color"] >> m->color;  
    }
    node["ka"] >> m->ka;
    node["kd"] >> m->kd;
    node["ks"] >> m->ks;
    node["n"] >> m->n;
    return m;
}

Object* Raytracer::parseObject(const YAML::Node& node)
{
    Object *returnObject = NULL;
    std::string objectType;
    node["type"] >> objectType;

    if (objectType == "sphere") {
        Point pos;
        node["position"] >> pos;
        double r;
        node["radius"] >> r;
        Sphere *sphere = new Sphere(pos,r);		
        returnObject = sphere;
    }
    else if(objectType == "triangle")
    {
        Point a, b, c;
        node["a"] >> a;
        node["b"] >> b;
        node["c"] >> c;
        Triangle *triangle = new Triangle(a, b, c);
        returnObject = triangle;
    }
    else if(objectType == "plane")
    {
        float d;
        Vector n;
        node["d"] >> d;
        node["n"] >> n;
        Plane *plane = new Plane(d, n);
        returnObject = plane;
    }
    else if(objectType == "quad")
    {
        Point a, b, c, d;
        a = parseTriple(node["a"]);
        b = parseTriple(node["b"]);
        c = parseTriple(node["c"]);
        d = parseTriple(node["d"]);
        Quad *quad = new Quad(a, b, c, d);
        returnObject = quad;
    }
    else if(objectType == "mesh")
    {
        std::string meshPath;
        node["path"] >> meshPath;
        Mesh *mesh = new Mesh(meshPath);
        mesh->position = parseTriple(node["position"]);
        node["size"] >> mesh->size;
        //mesh->recomputeNormals();
        mesh->scaleTranslate();
        returnObject = mesh;
    }

    if (returnObject) {
        // read the material and attach to object
        returnObject->material = parseMaterial(node["material"]);
    }

    if (node.FindValue("angle"))
    {
        node["angle"] >> returnObject->angle;
        returnObject->angle = returnObject->angle * 2 * M_PI / 360.0;
    }

    return returnObject;
}

Light* Raytracer::parseLight(const YAML::Node& node)
{
    Point position;
    node["position"] >> position;
    Color color;
    node["color"] >> color;
    return new Light(position,color);
}

Camera* Raytracer::parseCamera(const YAML::Node& node)
{
    Triple eye, center, up;
    unsigned int xSize, ySize;
    eye = parseTriple(node["eye"]);
    center = parseTriple(node["center"]);
    up = parseTriple(node["up"]);

    const YAML::Node& viewSizeNode = node["viewSize"];
    xSize = viewSizeNode[0];
    ySize = viewSizeNode[1];

    return new Camera(eye, center, up, xSize, ySize);
}

/*
* Read a scene from file
*/

bool Raytracer::readScene(const std::string& inputFilename)
{
    // Initialize a new scene
    scene = new Scene();

    // Open file stream for reading and have the YAML module parse it
    std::ifstream fin(inputFilename.c_str());
    if (!fin) {
        cerr << "Error: unable to open " << inputFilename << " for reading." << endl;;
        return false;
    }
    try {
        YAML::Parser parser(fin);
        if (parser) {
            YAML::Node doc;
            parser.GetNextDocument(doc);

            doc["RenderMode"] >> mode;
            if(mode == "gooch")
            {
                const YAML::Node& gooch = doc["GoochParameters"];
                gooch["b"] >> gp.b;
                gooch["y"] >> gp.y;
                gooch["alpha"] >> gp.alpha;
                gooch["beta"] >> gp.beta;
            }

            doc["AA"] >> aaFactor;
            if(doc["Shadows"] == "true")    shadows = true;
            else                            shadows = false;

            if(doc["Reflections"] == "true")     reflections = true;
            else                                reflections = false;

            // Read scene configuration options
            const YAML::Node& cam = doc["Camera"];
            scene->setEye(parseTriple(cam["eye"]));
            camera = parseCamera(cam);

            // Read and parse the scene objects
            const YAML::Node& sceneObjects = doc["Objects"];
            if (sceneObjects.GetType() != YAML::CT_SEQUENCE) {
                cerr << "Error: expected a sequence of objects." << endl;
                return false;
            }
            for(YAML::Iterator it=sceneObjects.begin();it!=sceneObjects.end();++it) {
                Object *obj = parseObject(*it);
                // Only add object if it is recognized
                if (obj) {
                    scene->addObject(obj);
                } else {
                    cerr << "Warning: found object of unknown type, ignored." << endl;
                }
            }

            // Read and parse light definitions
            const YAML::Node& sceneLights = doc["Lights"];
            if (sceneObjects.GetType() != YAML::CT_SEQUENCE) {
                cerr << "Error: expected a sequence of lights." << endl;
                return false;
            }
            for(YAML::Iterator it=sceneLights.begin();it!=sceneLights.end();++it) {
                scene->addLight(parseLight(*it));
            }
        }
        if (parser) {
            cerr << "Warning: unexpected YAML document, ignored." << endl;
        }
    } catch(YAML::ParserException& e) {
        std::cerr << "Error at line " << e.mark.line + 1 << ", col " << e.mark.column + 1 << ": " << e.msg << std::endl;
        return false;
    }

    cout << "YAML parsing results: " << scene->getNumObjects() << " objects read." << endl;
    return true;
}

void Raytracer::renderToFile(const std::string& outputFilename)
{
    Image img(camera->xSize, camera->ySize);
    cout << "Tracing..." << endl;
    if(mode == "phong")
    {
    	scene->render(img, camera, shadows, reflections, 0, aaFactor, gp);
    }
    else if(mode == "zbuffer")
    {
    	scene->render(img, camera, shadows, false, 1, 1, gp);
    }
    else if(mode == "normal")
    {
    	scene->render(img, camera, shadows, false, 2, 1, gp);
    }
    else if(mode == "gooch")
    {
        scene->render(img, camera, shadows, reflections, 3, aaFactor, gp);
    }
    cout << "Writing image to " << outputFilename << "..." << endl;
    img.write_png(outputFilename.c_str());
    cout << "Done." << endl;
}
