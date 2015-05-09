#include <math.h>
#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"

#include "PointLight.h"
#include "Sphere.h"
#include "TriangleMesh.h"
#include "Triangle.h"
#include "Lambert.h"
#include "MiroWindow.h"

void
makeSpiralScene()
{
    g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;

    g_image->resize(512, 512);
    
    // set up the camera
    g_camera->setBGColor(Vector3(1.0f, 1.0f, 1.0f));
    g_camera->setEye(Vector3(-5, 2, 3));
    g_camera->setLookAt(Vector3(0, 0, 0));
    g_camera->setUp(Vector3(0, 1, 0));
    g_camera->setFOV(45);

    // create and place a point light source
    PointLight * light = new PointLight;
    light->setPosition(Vector3(-3, 15, 3));
    light->setColor(Vector3(1, 1, 1));
    light->setWattage(1000);
    g_scene->addLight(light);

    // create a spiral of spheres
    //Material* mat = new Lambert(Vector3(1.0f, 0.0f, 0.0f));
    const int maxI = 200;
    const float a = 0.15f;
    for (int i = 1; i < maxI; ++i)
    {
		float rgb = (float)i / (float)maxI;
        float t = i/float(maxI);
        float theta = 4*PI*t;
        float r = a*theta;
        float x = r*cos(theta);
        float y = r*sin(theta);
        float z = 2*(2*PI*a - r);
		Material* mat = new Lambert(Vector3(rgb,1-rgb,0));
        Sphere * sphere = new Sphere;
        sphere->setCenter(Vector3(x,y,z));
        sphere->setRadius(r/10);
        sphere->setMaterial(mat);
        g_scene->addObject(sphere);
    }
    
    // let objects do pre-calculations if needed
    g_scene->preCalc();
}

void
makeBunnyScene()
{
    g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;

    g_image->resize(128, 128);
    
    // set up the camera
    g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.2f));
    g_camera->setEye(Vector3(0, 3, 3)); //-2,3,5
    g_camera->setLookAt(Vector3(0, 0, 0)); // -.5,1,0
    g_camera->setUp(Vector3(0, 1, 0));
    g_camera->setFOV(45);

    // create and place a point light source
    PointLight * light = new PointLight;
    light->setPosition(Vector3(-3, 15, 3)); // default -3,15,3
    light->setColor(Vector3(1, 1, 1));
    light->setWattage(2000);
    g_scene->addLight(light);

    Material* mat = new Lambert(Vector3(1.0f));
	((Lambert*)mat)->setKd(Vector3(1.0f));
	((Lambert*)mat)->setKs(Vector3(0.0f));
	((Lambert*)mat)->setKt(Vector3(0.0f));
	Material* mirror = new Lambert(Vector3(1.0f));
	((Lambert*)mirror)->setKd(Vector3(0.0f));
	((Lambert*)mirror)->setKs(Vector3(0.0f));
	((Lambert*)mirror)->setKt(Vector3(1.f));

    TriangleMesh * bunny = new TriangleMesh;
    bunny->load("Object/teapot.obj");
    
    // create all the triangles in the bunny mesh and add to the scene
	
	for (int i = 0; i < bunny->numTris(); ++i)
    {
        Triangle* t = new Triangle;
        t->setIndex(i);
        t->setMesh(bunny);
        t->setMaterial(mirror); 
        g_scene->addObject(t);
    }
	
	/*
	Sphere * sphere = new Sphere;
	float x = 0;
	float y = 1;
	float z = 0;
	float r = 1;
	sphere->setCenter(Vector3(x, y, z));
	sphere->setRadius(r);
	sphere->setMaterial(mirror);
	g_scene->addObject(sphere);
	*/
    // create the floor triangle
    TriangleMesh * floor = new TriangleMesh;
    floor->createSingleTriangle();
    floor->setV1(Vector3(  0, -1,  -10));
    floor->setV2(Vector3( 2.5, -1, 5));
    floor->setV3(Vector3(-2.5, -1, 5));
    floor->setN1(Vector3(0, 1, 0));
    floor->setN2(Vector3(0, 1, 0));
    floor->setN3(Vector3(0, 1, 0));
    
    Triangle* t = new Triangle;
    t->setIndex(0);
    t->setMesh(floor);
    t->setMaterial(mat); 
	g_scene->addObject(t);
    
    // let objects do pre-calculations if needed
    g_scene->preCalc();
}


int
main(int argc, char*argv[])
{
    // create a scene
    //makeSpiralScene();
	//makeLorenzScene();
	makeBunnyScene();

    MiroWindow miro(&argc, argv);
    miro.mainLoop();

    return 0; // never executed
}

