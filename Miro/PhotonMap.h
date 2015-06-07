#pragma once
#include <vector>
#include "Photon.h"
#include "PointLight.h"
#include "BVH.h"
#include "Sampler.h"
#include "Ray.h"
#include "Lambert.h"

class Scene;
class PhotonMap
{
public:
	PhotonMap();
	virtual ~PhotonMap();

	void emitPhoton(Scene* scene, PointLight* pobj, int n);
	void drawPhoton();
	void buildKD();
	Vector3 getL(HitInfo&);

	std::vector<Photon*> photonList;
private:
	Sampler* s;
};
