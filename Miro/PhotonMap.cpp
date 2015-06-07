#include "PhotonMap.h"
#include "Scene.h"

PhotonMap::PhotonMap()
{
	s = new Sampler();
}


PhotonMap::~PhotonMap()
{
	delete s;
}

void PhotonMap::emitPhoton(Scene* scene, PointLight* pobj, int n)
{
	for (int i = 0; i < n; i++){
		float random_num = 0;
		Vector3 direction = s->generate_vector_sample();
		Ray ray(pobj->position(), direction);
		HitInfo hit;
		float pw = pobj->wattage() / n;
		Vector3 pwv(pw, pw, pw);
		bool isRed = false;
		//printf("shooting photon!\n");
		do{
			random_num = 0;
			if (scene->trace(hit, ray)){
				if (isRed && hit.material->getKD().x > 0 && hit.material->getKD().y > 0 && hit.material->getKD().z > 0){
					//printf("red hits white!\n");
					//photonList[photonList.size() - 1]->power.print();
					//std::cin.get();
				}
				Photon* pho = new Photon();
				pho->power = pwv * hit.material->getKD();
				pwv = pho->power;
				//printf("color = ");
				//pho->power.print();
				pho->position = hit.P;
				pho->direction = ray.d;
				photonList.push_back(pho);

				ray.o = hit.P + 0.000001*hit.N;
				Vector3 w = hit.N;
				Vector3 t = s->generate_vector_sample();
				Vector3 u = cross(w, t);
				Vector3 v = cross(w, u);
				w.normalize();
				v.normalize();
				u.normalize();
				ray.d = s->generate_sphere_sample(5, u, v, w);
				//ray.d.print();

				pwv *= 0.31830988618;
				random_num = ((double)rand() / (RAND_MAX));
				if (hit.material->getKD().y == 0 && hit.material->getKD().x > 0){
					isRed = true;
				}
				else{
					isRed = false;
				}
			}
		} while (random_num > 0.1);
	}
	printf("done photon!\n");
}

void PhotonMap::drawPhoton()
{

}