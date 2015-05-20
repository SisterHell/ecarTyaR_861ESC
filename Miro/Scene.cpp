#include "Miro.h"
#include "Scene.h"
#include "Camera.h"
#include "Image.h"
#include "Console.h"
#include <time.h>


Scene * g_scene = 0;

void Scene::addObject(Object* pObj)
{
	m_objects.push_back(pObj); 
}

void
Scene::openGL(Camera *cam)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cam->drawGL();

    // draw objects
    for (size_t i = 0; i < m_objects.size(); ++i)
        m_objects[i]->renderGL();

    glutSwapBuffers();
}

void
Scene::preCalc()
{
    Objects::iterator it;
    for (it = m_objects.begin(); it != m_objects.end(); it++)
    {
        Object* pObj = *it;
        pObj->preCalc();
		if (pObj->min.x < min.x){
			min.x = pObj->min.x;
		}
		if (pObj->min.y < min.y){
			min.y = pObj->min.y;
		}
		if (pObj->min.z < min.z){
			min.z = pObj->min.z;
		}
		if (pObj->max.x > max.x){
			max.x = pObj->max.x;
		}
		if (pObj->max.y > max.y){
			max.y = pObj->max.y;
		}
		if (pObj->max.z > max.z){
			max.z = pObj->max.z;
		}
    }
    Lights::iterator lit;
    for (lit = m_lights.begin(); lit != m_lights.end(); lit++)
    {
        PointLight* pLight = *lit;
        pLight->preCalc();
    }

	m_bvh.setRoot(min,max);
    m_bvh.build(&m_objects);
}

void
Scene::raytraceImage(Camera *cam, Image *img)
{
	clock_t start_time, end_time;

    Ray ray;
    HitInfo hitInfo;
    Vector3 shadeResult;
	m_bvh.ray_box_intersection = 0;
	m_bvh.ray_tri_intersection = 0;
    // loop over all pixels in the image
	start_time = clock();
	for (int j = 0; j < img->height(); ++j)
    {
        for (int i = 0; i < img->width(); ++i)
        {
			Material::reflectDepth = 0;
			Material::refractDepth = 0;

            ray = cam->eyeRay(i, j, img->width(), img->height());
			ray.n = 1;

            if (trace(hitInfo, ray))
            {
                shadeResult = hitInfo.material->shade(ray, hitInfo, *this);
                img->setPixel(i, j, shadeResult);
            }
        }
        img->drawScanline(j);
        glFinish();
        printf("Rendering Progress: %.3f%%\r", j/float(img->height())*100.0f);
        fflush(stdout);
    }
	printf("Rendering Progress: 100.000%\n");
	end_time = clock();
	std::cout << "trace time : " << ((double)(end_time - start_time)) / CLOCKS_PER_SEC << "\n";
    
	std::cout << "ray_box_intersection: " << m_bvh.ray_box_intersection << std::endl;
	std::cout << "ray_tri_intersection: " << m_bvh.ray_tri_intersection << std::endl;
    debug("done Raytracing!\n");
}

bool
Scene::trace(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
	return m_bvh.intersect(minHit, ray, tMin, tMax);
}


bool
Scene::trace(HitInfo& minHit, const Ray& ray, int& depth, float tMin, float tMax) const
{
	depth++;
	if (depth > MAX_DEPTH){
		return false;
	}
	return m_bvh.intersect(minHit, ray, tMin, tMax);
}
