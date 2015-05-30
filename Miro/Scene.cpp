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
	m_bvh.ray_num = 0;
	const int height = img->height();
	const int width = img->width();
    // loop over all pixels in the image
	start_time = clock();
	for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
			/*
			Material::reflectDepth = 0;
			Material::refractDepth = 0;
			ray = cam->eyeRay(i, j, width, height);
			ray.n = 1;

			if (trace(hitInfo, ray))
			{
				shadeResult = hitInfo.material->shade(ray, hitInfo, *this);
				img->setPixel(i, j, shadeResult);
			}
			*/
			// DOF
			int count = 0;
			shadeResult.x = 0; shadeResult.y = 0; shadeResult.z = 0;
			for (int k = 0; k < 200; k++){
				ray = cam->DOFeyeRay(i, j, width, height);
				ray.n = 1;
				Material::reflectDepth = 0;
				Material::refractDepth = 0;
				if (trace(hitInfo, ray))
				{
					shadeResult += hitInfo.material->shade(ray, hitInfo, *this);
					count++;
				}
			}
			shadeResult /= 200.0f;
			if (count != 0)
				img->setPixel(i, j, shadeResult);
			

			/* anti aliasing
			if (i == 0 || j == 0 || i == width - 1 || j == height - 1){

				ray = cam->eyeRay(i, j, width, height);
				ray.n = 1;

				if (trace(hitInfo, ray))
				{
					shadeResult = hitInfo.material->shade(ray, hitInfo, *this);
					img->setPixel(i, j, shadeResult);
				}
			}
			//3x supersampling
			else{
				int count = 0;
				shadeResult.x = 0; shadeResult.y = 0; shadeResult.z = 0;
				for (int k = -1; k < 2; k++){
					for (int l = -1; l < 2; l++){

						Material::reflectDepth = 0;
						Material::refractDepth = 0;

						ray = cam->eyeRay(i + 0.33*k, j + 0.33*l, width, height);
						ray.n = 1;


						if (trace(hitInfo, ray))
						{
							shadeResult += hitInfo.material->shade(ray, hitInfo, *this);

							count++;
						}
					}
				}
				//if (count == 0) std::cout << "nono" << std::endl;
				shadeResult /= (float)count;
				if (count != 0)
				img->setPixel(i, j, shadeResult);
			}
			*/
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
	std::cout << "ray_num: " << m_bvh.ray_num << std::endl;
    debug("done Raytracing!\n");
}

bool
Scene::trace(HitInfo& minHit, const Ray& ray, float tMin, float tMax) const
{
	m_bvh.ray_num++;
	return m_bvh.intersect(minHit, ray, tMin, tMax);
}


bool
Scene::trace(HitInfo& minHit, const Ray& ray, int& depth, float tMin, float tMax) const
{
	depth++;
	if (depth > MAX_DEPTH){
		return false;
	}
	m_bvh.ray_num++;
	return m_bvh.intersect(minHit, ray, tMin, tMax);
}
