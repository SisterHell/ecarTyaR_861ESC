#ifndef CSE168_SCENE_H_INCLUDED
#define CSE168_SCENE_H_INCLUDED

#include "Miro.h"
#include "Object.h"
#include "PointLight.h"
#include "BVH.h"
#include "PhotonMap.h"

class Camera;
class Image;

class Scene
{
public:
	Scene() 
	{ 
		min.set(1000000, 1000000, 1000000); 
		max.set(-1000000, -1000000, -1000000);
	}
	void addObject(Object* pObj);        
    const Objects* objects() const      {return &m_objects;}

    void addLight(PointLight* pObj)     {m_lights.push_back(pObj);}
    const Lights* lights() const        {return &m_lights;}

    void preCalc();
    void openGL(Camera *cam);

    void raytraceImage(Camera *cam, Image *img);
	
	bool trace(HitInfo& minHit, const Ray& ray,
		float tMin = 0.0f, float tMax = MIRO_TMAX) const;

	bool trace(HitInfo& minHit, const Ray& ray, int& depth,
               float tMin = 0.0f, float tMax = MIRO_TMAX) const;
	bool pathTrace = false;
	bool photonTrace = true;

	PhotonMap phoMap;

	Vector3 bgColor;
protected:
    Objects m_objects;
    BVH m_bvh;
    Lights m_lights;
	Vector3 min,max;
};

extern Scene * g_scene;

#endif // CSE168_SCENE_H_INCLUDED
