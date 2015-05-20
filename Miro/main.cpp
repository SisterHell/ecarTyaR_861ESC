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
#include "Worley.h"

#include "assignment2.h"

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
/*
void
makeBunnyScene()
{
    g_camera = new Camera;
    g_scene = new Scene;
    g_image = new Image;

    g_image->resize(128, 128);
    
    // set up the camera
    g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.2f));
    g_camera->setEye(Vector3(-2, 3, 5)); //-2,3,5
    g_camera->setLookAt(Vector3(-0.5, 1, 0)); // -.5,1,0
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

	Material* red = new Lambert(Vector3(1.0f,0.0f,0.0f));
	((Lambert*)red)->setKd(Vector3((139.0/255.0), (69.0/255.0), (19.0/255.0)));
	((Lambert*)red)->setKs(Vector3(0.0f));
	((Lambert*)red)->setKt(Vector3(0.0f));
	((Lambert*)red)->setNoise(true);

    TriangleMesh * bunny = new TriangleMesh;
    bunny->load("Object/teapot.obj");
    
    // create all the triangles in the bunny mesh and add to the scene
	
	for (int i = 0; i < bunny->numTris(); ++i)
    {
        Triangle* t = new Triangle;
        t->setIndex(i);
        t->setMesh(bunny);
		t->setMaterial(red);
        g_scene->addObject(t);
    }
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
}*/
/*
void
makeTeapotScene(){
	g_camera = new Camera;
	g_scene = new Scene;
	g_image = new Image;

	g_image->resize(128, 128);

	// set up the camera
	g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.2f));
	g_camera->setEye(Vector3(-2, 3, 5));
	g_camera->setLookAt(Vector3(-.5, 1, 0));
	g_camera->setUp(Vector3(0, 1, 0));
	g_camera->setFOV(45);

	// create and place a point light source
	PointLight * light = new PointLight;
	light->setPosition(Vector3(-3, 15, 3));
	light->setColor(Vector3(1, 1, 1));
	light->setWattage(2000);
	g_scene->addLight(light);

	Material* mat = new Lambert(Vector3(1.0f));
	((Lambert*)mat)->setKd(Vector3(1.0f));
	((Lambert*)mat)->setKs(Vector3(0.0f));
	((Lambert*)mat)->setKt(Vector3(0.0f));

	Material* red = new Lambert(Vector3(1.0f, 0.0f, 0.0f));
	((Lambert*)red)->setKd(Vector3(1.0f, 0.0f, 0.0f));
	((Lambert*)red)->setKs(Vector3(0.0f));
	((Lambert*)red)->setKt(Vector3(0.0f));
	((Lambert*)red)->setNoise(false);

	TriangleMesh * teapot = new TriangleMesh;
	teapot->load("Object/teapot.obj");

	// create all the triangles in the bunny mesh and add to the scene
	for (int i = 0; i < teapot->numTris(); ++i)
	{
		Triangle* t = new Triangle;
		t->setIndex(i);
		t->setMesh(teapot);
		t->setMaterial(red);
		g_scene->addObject(t);
	}

	// create the floor triangle
	TriangleMesh * floor = new TriangleMesh;
	floor->createSingleTriangle();
	floor->setV1(Vector3(0, 0, 10));
	floor->setV2(Vector3(10, 0, -10));
	floor->setV3(Vector3(-10, 0, -10));
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
}*/

void
refractionScene(){
	g_camera = new Camera;
	g_scene = new Scene;
	g_image = new Image;

	g_image->resize(128, 128);

	// set up the camera
	g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.2f));
	g_camera->setEye(Vector3(-4, 7, 5));
	g_camera->setLookAt(Vector3(-.5, 1, 0));
	g_camera->setUp(Vector3(0, 1, 0));
	g_camera->setFOV(45);

	// create and place a point light source
	PointLight * light = new PointLight;
	light->setPosition(Vector3(-3, 15, 3));
	light->setColor(Vector3(1, 1, 1));
	light->setWattage(2000);
	g_scene->addLight(light);

	Material* mat = new Lambert(Vector3(1.0f));
	((Lambert*)mat)->setKd(Vector3(1.0f));
	((Lambert*)mat)->setKs(Vector3(0.0f));
	((Lambert*)mat)->setKt(Vector3(0.0f));

	Material* green = new Lambert(Vector3(1.0f, 0.0f, 0.0f));
	((Lambert*)green)->setKd(Vector3(0.0f, 1.0f, 0.0f));
	((Lambert*)green)->setKs(Vector3(0.0f));
	((Lambert*)green)->setKt(Vector3(0.0f));
	((Lambert*)green)->setNoise(false);

	Material* red = new Lambert(Vector3(1.0f, 0.0f, 0.0f));
	((Lambert*)red)->setKd(Vector3(1.0f, 0.0f, 0.0f));
	((Lambert*)red)->setKs(Vector3(0.0f));
	((Lambert*)red)->setKt(Vector3(0.0f));
	((Lambert*)red)->setNoise(false);

	Material* stone = new Lambert(Vector3(1.0f));
	((Lambert*)stone)->setKd(Vector3(1.0f));
	((Lambert*)stone)->setKs(Vector3(0.0f));
	((Lambert*)stone)->setKt(Vector3(0.0f));
	((Lambert*)stone)->setNoise(true);


	Material* glass = new Lambert(Vector3(1.0f));
	((Lambert*)glass)->setKd(Vector3(0.0f));
	((Lambert*)glass)->setKs(Vector3(0.0f));
	((Lambert*)glass)->setKt(Vector3(1.0f));

	Material* shinyFloor = new Lambert(Vector3(1.0f));
	((Lambert*)shinyFloor)->setKd(Vector3(0.4f));
	((Lambert*)shinyFloor)->setKs(Vector3(0.6f));
	((Lambert*)shinyFloor)->setKt(Vector3(0.0f));

	TriangleMesh * teapot = new TriangleMesh;
	teapot->load("Object/teapot.obj");

	// create all the triangles in the bunny mesh and add to the scene
	for (int i = 0; i < teapot->numTris(); ++i)
	{
		Triangle* t = new Triangle;
		t->setIndex(i);
		t->setMesh(teapot);
		t->setMaterial(stone);
		g_scene->addObject(t);
	}

	Sphere * sphere = new Sphere;
	sphere->setCenter(Vector3(-3.2, 5.7, 4));
	sphere->setRadius(0.3);
	sphere->setMaterial(glass);
	g_scene->addObject(sphere);

	Sphere * sphere2 = new Sphere;
	sphere2->setCenter(Vector3(1, 2, 1));
	sphere2->setRadius(0.4);
	sphere2->setMaterial(green);
	g_scene->addObject(sphere2);

	// create the floor triangle
	TriangleMesh * floor = new TriangleMesh;
	floor->createSingleTriangle();
	floor->setV1(Vector3(0, 0, 10));
	floor->setV2(Vector3(10, 0, -10));
	floor->setV3(Vector3(-10, 0, -10));
	floor->setN1(Vector3(0, 1, 0));
	floor->setN2(Vector3(0, 1, 0));
	floor->setN3(Vector3(0, 1, 0));

	Triangle* t = new Triangle;
	t->setIndex(0);
	t->setMesh(floor);
	t->setMaterial(shinyFloor);
	g_scene->addObject(t);

	// let objects do pre-calculations if needed
	g_scene->preCalc();
}

void
glassBallsScene(){
	g_camera = new Camera;
	g_scene = new Scene;
	g_image = new Image;

	g_image->resize(128, 128);

	// set up the camera
	g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.2f));
	g_camera->setEye(Vector3(0, 2, 7));
	g_camera->setLookAt(Vector3(-.5, 1, 0));
	g_camera->setUp(Vector3(0, 1, 0));
	g_camera->setFOV(45);

	// create and place a point light source
	PointLight * light = new PointLight;
	light->setPosition(Vector3(-3, 15, 3));
	light->setColor(Vector3(1, 1, 1));
	light->setWattage(2000);
	g_scene->addLight(light);

	Material* mat = new Lambert(Vector3(1.0f));
	((Lambert*)mat)->setKd(Vector3(1.0f));
	((Lambert*)mat)->setKs(Vector3(0.0f));
	((Lambert*)mat)->setKt(Vector3(0.0f));

	Material* green = new Lambert(Vector3(1.0f, 0.0f, 0.0f));
	((Lambert*)green)->setKd(Vector3(0.0f, 1.0f, 0.0f));
	((Lambert*)green)->setKs(Vector3(0.0f));
	((Lambert*)green)->setKt(Vector3(0.0f));
	((Lambert*)green)->setNoise(false);

	Material* red = new Lambert(Vector3(1.0f, 0.0f, 0.0f));
	((Lambert*)red)->setKd(Vector3(1.0f, 0.0f, 0.0f));
	((Lambert*)red)->setKs(Vector3(0.0f));
	((Lambert*)red)->setKt(Vector3(0.0f));
	((Lambert*)red)->setNoise(false);

	Material* stone = new Lambert(Vector3(1.0f));
	((Lambert*)stone)->setKd(Vector3(1.0f));
	((Lambert*)stone)->setKs(Vector3(0.0f));
	((Lambert*)stone)->setKt(Vector3(0.0f));
	((Lambert*)stone)->setNoise(true);


	Material* glass = new Lambert(Vector3(1.0f));
	((Lambert*)glass)->setKd(Vector3(0.0f));
	((Lambert*)glass)->setKs(Vector3(0.0f));
	((Lambert*)glass)->setKt(Vector3(1.0f));

	Material* shinyFloor = new Lambert(Vector3(1.0f));
	((Lambert*)shinyFloor)->setKd(Vector3(0.4f));
	((Lambert*)shinyFloor)->setKs(Vector3(0.6f));
	((Lambert*)shinyFloor)->setKt(Vector3(0.0f));

	Material* glassBowl = new Lambert(Vector3(1.0f));
	((Lambert*)glassBowl)->setKd(Vector3(0.2f));
	((Lambert*)glassBowl)->setKs(Vector3(0.2f));
	((Lambert*)glassBowl)->setKt(Vector3(0.6f));


	Sphere * sphere = new Sphere;
	sphere->setCenter(Vector3(0,2,2));
	sphere->setRadius(0.5);
	sphere->setMaterial(glass);
	g_scene->addObject(sphere);

	Sphere * sphere2 = new Sphere;
	sphere2->setCenter(Vector3(-2, 2, -2));
	sphere2->setRadius(0.8);
	sphere2->setMaterial(glassBowl);
	g_scene->addObject(sphere2);

	Sphere * sphere3 = new Sphere;
	sphere3->setCenter(Vector3(2, 2, -2));
	sphere3->setRadius(0.8);
	sphere3->setMaterial(stone);
	g_scene->addObject(sphere3);

	// create the floor triangle
	TriangleMesh * floor = new TriangleMesh;
	floor->createSingleTriangle();
	floor->setV1(Vector3(0, 0, 10));
	floor->setV2(Vector3(10, 0, -10));
	floor->setV3(Vector3(-10, 0, -10));
	floor->setN1(Vector3(0, 1, 0));
	floor->setN2(Vector3(0, 1, 0));
	floor->setN3(Vector3(0, 1, 0));

	Triangle* t = new Triangle;
	t->setIndex(0);
	t->setMesh(floor);
	t->setMaterial(shinyFloor);
	g_scene->addObject(t);

	// let objects do pre-calculations if needed
	g_scene->preCalc();
}

void
reflectionScene2(){
	g_camera = new Camera;
	g_scene = new Scene;
	g_image = new Image;

	g_image->resize(128, 128);

	// set up the camera
	g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.2f));
	g_camera->setEye(Vector3(-2, 5, 11));
	g_camera->setLookAt(Vector3(-.5, 1, 0));
	g_camera->setUp(Vector3(0, 1, 0));
	g_camera->setFOV(45);

	// create and place a point light source
	PointLight * light = new PointLight;
	light->setPosition(Vector3(-3, 15, 3));
	light->setColor(Vector3(1, 1, 1));
	light->setWattage(2000);
	g_scene->addLight(light);

	Material* mat = new Lambert(Vector3(1.0f));
	((Lambert*)mat)->setKd(Vector3(1.0f));
	((Lambert*)mat)->setKs(Vector3(0.0f));
	((Lambert*)mat)->setKt(Vector3(0.0f));

	Material* green = new Lambert(Vector3(1.0f, 0.0f, 0.0f));
	((Lambert*)green)->setKd(Vector3(0.0f, 1.0f, 0.0f));
	((Lambert*)green)->setKs(Vector3(0.0f));
	((Lambert*)green)->setKt(Vector3(0.0f));
	((Lambert*)green)->setNoise(false);

	Material* red = new Lambert(Vector3(1.0f, 0.0f, 0.0f));
	((Lambert*)red)->setKd(Vector3(1.0f, 0.0f, 0.0f));
	((Lambert*)red)->setKs(Vector3(0.0f));
	((Lambert*)red)->setKt(Vector3(0.0f));
	((Lambert*)red)->setNoise(false);

	Material* stone = new Lambert(Vector3(1.0f));
	((Lambert*)stone)->setKd(Vector3(1.0f));
	((Lambert*)stone)->setKs(Vector3(0.0f));
	((Lambert*)stone)->setKt(Vector3(0.0f));
	((Lambert*)stone)->setNoise(true);


	Material* mirror = new Lambert(Vector3(1.0f));
	((Lambert*)mirror)->setKd(Vector3(0.0f));
	((Lambert*)mirror)->setKs(Vector3(1.0f));
	((Lambert*)mirror)->setKt(Vector3(0.0f));

	Material* shinyFloor = new Lambert(Vector3(1.0f));
	((Lambert*)shinyFloor)->setKd(Vector3(0.4f));
	((Lambert*)shinyFloor)->setKs(Vector3(0.6f));
	((Lambert*)shinyFloor)->setKt(Vector3(0.0f));

	TriangleMesh * teapot = new TriangleMesh;
	teapot->load("Object/teapot.obj");

	// create all the triangles in the bunny mesh and add to the scene
	for (int i = 0; i < teapot->numTris(); ++i)
	{
		Triangle* t = new Triangle;
		t->setIndex(i);
		t->setMesh(teapot);
		t->setMaterial(stone);
		g_scene->addObject(t);
	}

	Sphere * sphere = new Sphere;
	sphere->setCenter(Vector3(-2, 4, -5));
	sphere->setRadius(2);
	sphere->setMaterial(mirror);
	g_scene->addObject(sphere);

	// create the floor triangle
	TriangleMesh * floor = new TriangleMesh;
	floor->createSingleTriangle();
	floor->setV1(Vector3(0, 0, 10));
	floor->setV2(Vector3(10, 0, -10));
	floor->setV3(Vector3(-10, 0, -10));
	floor->setN1(Vector3(0, 1, 0));
	floor->setN2(Vector3(0, 1, 0));
	floor->setN3(Vector3(0, 1, 0));

	Triangle* t = new Triangle;
	t->setIndex(0);
	t->setMesh(floor);
	t->setMaterial(shinyFloor);
	g_scene->addObject(t);

	// let objects do pre-calculations if needed
	g_scene->preCalc();
}

void
reflectionScene3(){
	g_camera = new Camera;
	g_scene = new Scene;
	g_image = new Image;

	g_image->resize(128, 128);

	// set up the camera
	g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.2f));
	g_camera->setEye(Vector3(-2, 5, 11));
	g_camera->setLookAt(Vector3(-.5, 1, 0));
	g_camera->setUp(Vector3(0, 1, 0));
	g_camera->setFOV(45);

	// create and place a point light source
	PointLight * light = new PointLight;
	light->setPosition(Vector3(-3, 15, 3));
	light->setColor(Vector3(1, 1, 1));
	light->setWattage(2000);
	g_scene->addLight(light);

	Material* mat = new Lambert(Vector3(1.0f));
	((Lambert*)mat)->setKd(Vector3(1.0f));
	((Lambert*)mat)->setKs(Vector3(0.0f));
	((Lambert*)mat)->setKt(Vector3(0.0f));

	Material* green = new Lambert(Vector3(1.0f, 0.0f, 0.0f));
	((Lambert*)green)->setKd(Vector3(0.0f, 1.0f, 0.0f));
	((Lambert*)green)->setKs(Vector3(0.0f));
	((Lambert*)green)->setKt(Vector3(0.0f));
	((Lambert*)green)->setNoise(false);

	Material* red = new Lambert(Vector3(1.0f, 0.0f, 0.0f));
	((Lambert*)red)->setKd(Vector3(1.0f, 0.0f, 0.0f));
	((Lambert*)red)->setKs(Vector3(0.0f));
	((Lambert*)red)->setKt(Vector3(0.0f));
	((Lambert*)red)->setNoise(false);

	Material* stone = new Lambert(Vector3(1.0f));
	((Lambert*)stone)->setKd(Vector3(1.0f));
	((Lambert*)stone)->setKs(Vector3(0.0f));
	((Lambert*)stone)->setKt(Vector3(0.0f));
	((Lambert*)stone)->setNoise(true);


	Material* mirror = new Lambert(Vector3(1.0f));
	((Lambert*)mirror)->setKd(Vector3(0.0f));
	((Lambert*)mirror)->setKs(Vector3(1.0f));
	((Lambert*)mirror)->setKt(Vector3(0.0f));

	Material* shinyFloor = new Lambert(Vector3(1.0f));
	((Lambert*)shinyFloor)->setKd(Vector3(0.4f));
	((Lambert*)shinyFloor)->setKs(Vector3(0.6f));
	((Lambert*)shinyFloor)->setKt(Vector3(0.0f));

	TriangleMesh * bunny = new TriangleMesh;
	bunny->load("Object/bunny.obj");

	// create all the triangles in the bunny mesh and add to the scene
	for (int i = 0; i < bunny->numTris(); ++i)
	{
		Triangle* t = new Triangle;
		t->setIndex(i);
		t->setMesh(bunny);
		t->setMaterial(stone);
		g_scene->addObject(t);
	}

	Sphere * sphere = new Sphere;
	sphere->setCenter(Vector3(-2, 4, -5));
	sphere->setRadius(2);
	sphere->setMaterial(mirror);
	g_scene->addObject(sphere);

	// create the floor triangle
	TriangleMesh * floor = new TriangleMesh;
	floor->createSingleTriangle();
	floor->setV1(Vector3(0, 0, 10));
	floor->setV2(Vector3(10, 0, -10));
	floor->setV3(Vector3(-10, 0, -10));
	floor->setN1(Vector3(0, 1, 0));
	floor->setN2(Vector3(0, 1, 0));
	floor->setN3(Vector3(0, 1, 0));

	Triangle* t = new Triangle;
	t->setIndex(0);
	t->setMesh(floor);
	t->setMaterial(shinyFloor);
	g_scene->addObject(t);

	// let objects do pre-calculations if needed
	g_scene->preCalc();
}

void
reflectionScene(){
	g_camera = new Camera;
	g_scene = new Scene;
	g_image = new Image;

	g_image->resize(128, 128);

	// set up the camera
	g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.2f));
	g_camera->setEye(Vector3(-2, 3, 5));
	g_camera->setLookAt(Vector3(-.5, 1, 0));
	g_camera->setUp(Vector3(0, 1, 0));
	g_camera->setFOV(45);

	// create and place a point light source
	PointLight * light = new PointLight;
	light->setPosition(Vector3(-3, 15, 3));
	light->setColor(Vector3(1, 1, 1));
	light->setWattage(2000);
	g_scene->addLight(light);

	Material* mat = new Lambert(Vector3(1.0f));
	((Lambert*)mat)->setKd(Vector3(1.0f));
	((Lambert*)mat)->setKs(Vector3(0.0f));
	((Lambert*)mat)->setKt(Vector3(0.0f));

	Material* green = new Lambert(Vector3(1.0f, 0.0f, 0.0f));
	((Lambert*)green)->setKd(Vector3(0.0f, 1.0f, 0.0f));
	((Lambert*)green)->setKs(Vector3(0.0f));
	((Lambert*)green)->setKt(Vector3(0.0f));
	((Lambert*)green)->setNoise(false);

	Material* red = new Lambert(Vector3(1.0f, 0.0f, 0.0f));
	((Lambert*)red)->setKd(Vector3(1.0f, 0.0f, 0.0f));
	((Lambert*)red)->setKs(Vector3(0.0f));
	((Lambert*)red)->setKt(Vector3(0.0f));
	((Lambert*)red)->setNoise(false);


	Material* mirror = new Lambert(Vector3(1.0f));
	((Lambert*)mirror)->setKd(Vector3(0.0f));
	((Lambert*)mirror)->setKs(Vector3(1.0f));
	((Lambert*)mirror)->setKt(Vector3(0.0f));

	TriangleMesh * teapot = new TriangleMesh;
	teapot->load("Object/teapot.obj");

	// create all the triangles in the bunny mesh and add to the scene
	for (int i = 0; i < teapot->numTris(); ++i)
	{
		Triangle* t = new Triangle;
		t->setIndex(i);
		t->setMesh(teapot);
		t->setMaterial(mirror);
		g_scene->addObject(t);
	}

	Sphere * sphere = new Sphere;
	sphere->setCenter(Vector3(-2, 2, 1));
	sphere->setRadius(0.3);
	sphere->setMaterial(red);
	g_scene->addObject(sphere);

	Sphere * sphere2 = new Sphere;
	sphere2->setCenter(Vector3(2, 2, -1));
	sphere2->setRadius(0.3);
	sphere2->setMaterial(green);
	g_scene->addObject(sphere2);

	// create the floor triangle
	TriangleMesh * floor = new TriangleMesh;
	floor->createSingleTriangle();
	floor->setV1(Vector3(0, 0, 10));
	floor->setV2(Vector3(10, 0, -10));
	floor->setV3(Vector3(-10, 0, -10));
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

void
makeSphereScene(){
	g_camera = new Camera;
	g_scene = new Scene;
	g_image = new Image;

	g_image->resize(128, 128);

	// set up the camera
	g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.2f));
	g_camera->setEye(Vector3(-2, 3, 5));
	g_camera->setLookAt(Vector3(-.5, 1, 0));
	g_camera->setUp(Vector3(0, 1, 0));
	g_camera->setFOV(45);

	// create and place a point light source
	PointLight * light = new PointLight;
	light->setPosition(Vector3(-3, 15, 3));
	light->setColor(Vector3(1, 1, 1));
	light->setWattage(2000);
	g_scene->addLight(light);

	Material* mat = new Lambert(Vector3(1.0f));
	((Lambert*)mat)->setKd(Vector3(1.0f));
	((Lambert*)mat)->setKs(Vector3(0.0f));
	((Lambert*)mat)->setKt(Vector3(0.0f));

	Material* green = new Lambert(Vector3(1.0f, 0.0f, 0.0f));
	((Lambert*)green)->setKd(Vector3(0.0f, 1.0f, 0.0f));
	((Lambert*)green)->setKs(Vector3(0.0f));
	((Lambert*)green)->setKt(Vector3(0.0f));
	((Lambert*)green)->setNoise(false);

	Material* stone = new Lambert(Vector3(1.0f));
	((Lambert*)stone)->setKd(Vector3(1.0f,0.0f,0.0f));
	((Lambert*)stone)->setKs(Vector3(0.0f));
	((Lambert*)stone)->setKt(Vector3(0.0f));
	((Lambert*)stone)->setNoise(true);

	TriangleMesh * sphere = new TriangleMesh;
	sphere->load("Object/sphere.obj");

	// create all the triangles in the bunny mesh and add to the scene
	for (int i = 0; i < sphere->numTris(); ++i)
	{
		Triangle* t = new Triangle;
		t->setIndex(i);
		t->setMesh(sphere);
		t->setMaterial(stone);
		g_scene->addObject(t);
	}

	// create the floor triangle
	TriangleMesh * floor = new TriangleMesh;
	floor->createSingleTriangle();
	floor->setV1(Vector3(0, 0, 10));
	floor->setV2(Vector3(10, 0, -10));
	floor->setV3(Vector3(-10, 0, -10));
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

void
makeBunnyScene()
{
	g_camera = new Camera;
	g_scene = new Scene;
	g_image = new Image;

	g_image->resize(128, 128);

	// set up the camera
	g_camera->setBGColor(Vector3(0.0f, 0.0f, 0.2f));
	g_camera->setEye(Vector3(-2, 3, 5));
	g_camera->setLookAt(Vector3(-.5, 1, 0));
	g_camera->setUp(Vector3(0, 1, 0));
	g_camera->setFOV(45);

	// create and place a point light source
	PointLight * light = new PointLight;
	light->setPosition(Vector3(-3, 15, 3));
	light->setColor(Vector3(1, 1, 1));
	light->setWattage(2000);
	g_scene->addLight(light);

	Material* mat = new Lambert(Vector3(1.0f));

	TriangleMesh * bunny = new TriangleMesh;
	bunny->load("Object/bunny.obj");

	// create all the triangles in the bunny mesh and add to the scene
	for (int i = 0; i < bunny->numTris(); ++i)
	{
		Triangle* t = new Triangle;
		t->setIndex(i);
		t->setMesh(bunny);
		t->setMaterial(mat);
		g_scene->addObject(t);
	}

	// create the floor triangle
	TriangleMesh * floor = new TriangleMesh;
	floor->createSingleTriangle();
	floor->setV1(Vector3(0, 0, 10));
	floor->setV2(Vector3(10, 0, -10));
	floor->setV3(Vector3(-10, 0, -10));
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
	//makeBunnyScene();
	//makeTeapotScene();
	//makeSphereScene();
	//reflectionScene();
	//reflectionScene2();
	//reflectionScene3();
	//glassBallsScene();
	//refractionScene();
	//makeTeapotScene();
	//makeBunny1Scene();
	//makeBunny20Scene();
	makeSponzaScene();
    MiroWindow miro(&argc, argv);
    miro.mainLoop();

    return 0; // never executed
}

