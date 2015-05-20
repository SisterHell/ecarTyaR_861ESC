#include "Lambert.h"
#include "Ray.h"
#include "Scene.h"
#include <algorithm>
#include "Worley.h"
#include "Perlin.h"

Lambert::Lambert(const Vector3 & kd, const Vector3 & ka, const Vector3 & ks, const Vector3 & kt, const bool &NS) :
m_kd(kd), m_ka(ka), m_ks(ks), m_kt(kt), noise()
{

}

Lambert::~Lambert()
{
}

Vector3
Lambert::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
	bool isShadow = false;

	Vector3 L = Vector3(0.0f, 0.0f, 0.0f);

	const Vector3 viewDir = -ray.d; // d is a unit vector

	const Lights *lightlist = scene.lights();

	// loop over all of the lights
	Lights::const_iterator lightIter;

	float Pnoise;
	if (noise){
		Pnoise = PerlinNoise::noise(hit.P.x * 10, hit.P.y * 10, hit.P.z * 10);
		//std::cout << Pnoise << std::endl;
	}
	float* at = new float[3];
	//float at[3] = {hit.P.x, hit.P.y, hit.P.z};
	at[0] = hit.P.x * 10; at[1] = hit.P.y * 10; at[2] = hit.P.z * 10;
	float* F = new float[2];
	float(*delta)[3] = new float[3][3];

	unsigned long *ID = new unsigned long;
	if (noise)
		WorleyNoise::noise3D(at, 2, F, delta, ID);

	//std::cout << (F[1] - F[0]) << std::endl;


	//Lambert::setKd(Vector3(F[1] - F[0]));
	float factor;
	if (noise){
		factor = (F[1] - F[0]);
		//std::cout << factor << std::endl;
	}
	for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
	{
		PointLight* pLight = *lightIter;

		// light direction
		Vector3 l = pLight->position() - hit.P;

		// the inverse-squared falloff
		float falloff = l.length2();

		// normalize the light direction
		l /= sqrt(falloff);

		Vector3 rayToLight = pLight->position() - hit.P;
		float distToLight = rayToLight.length();
		rayToLight = rayToLight.normalize();
		if (shadowOn){
			HitInfo shadowInfo;
			Ray shadowRay(hit.P + rayToLight*epsilon, rayToLight);
			if (scene.trace(shadowInfo, shadowRay)){
				float dist = (shadowInfo.P - hit.P).length();
				if (dist < distToLight){
					isShadow = true;
					//std::cout << "distance to object: " << dist << std::endl;
					//std::cout << "distance to light: " << distToLight << std::endl;
				}
			}
		}


		// get the diffuse component
		float nDotL = dot(hit.N, l);
		Vector3 result = pLight->color();
		if (noise){
			//result *= Vector3(factor*(139.0 / 255.0), factor*(69.0 / 255.0), factor*(19.0 / 255.0));
			result *= m_kd * Vector3(factor);
			//if (factor > 0.3)
			Vector3 nV(Pnoise);
			if (nV.x < 0) nV.x = 0;
			if (nV.y < 0) nV.y = 0;
			if (nV.z < 0) nV.z = 0;
			//std::cout << result.x << ", " << result.y << ", " << result.z << std::endl;
			if (result.x > 0.2)
				result.x += nV.x;
			if (result.y > 0.2)
				result.y += nV.y;
			if (result.y > 0.2)
				result.y += nV.y;
		}
		else
			result *= m_kd;

		float E = nDotL / falloff * pLight->wattage() / (4 * PI);

		L += std::max(0.0f, E) * result;
		

		//std::cout << L.x << ", " << L.y << ", " << L.z << std::endl;


		Vector3 wr;

		
		//std::cout << "ray.d = " << ray.d.x << "," << ray.d.y << "," << ray.d.z << std::endl;
		//std::cout << "wr = " << wr.x << "," << wr.y << "," << wr.z << std::endl;
		//std::cout << "hit point = " << hit.P.x << "," << hit.P.y << "," << hit.P.z << std::endl;
		//std::cin.get();
		if (m_ks != 0){

			wr = 2 * dot(viewDir, hit.N) * hit.N + ray.d;
			L += m_ks * std::max(0.0f, E) * powf(dot(viewDir, wr), 80) / (nDotL);
		}

		
	}
	Vector3 wr;

	// reflection ray tracing
	//std::cout << "ray.d = " << ray.d.x << "," << ray.d.y << "," << ray.d.z << std::endl;
	//std::cout << "wr = " << wr.x << "," << wr.y << "," << wr.z << std::endl;
	//std::cout << "hit point = " << hit.P.x << "," << hit.P.y << "," << hit.P.z << std::endl;
	//std::cin.get();
	if (m_ks != 0){

		wr = 2 * dot(viewDir, hit.N) * hit.N + ray.d;
		HitInfo hitReflect;
		//Ray rayReflect(hit.P + hit.N * 0.0001, wr);
		Ray rayReflect(hit.P + wr * epsilon, wr);

		if (scene.trace(hitReflect, rayReflect, Material::reflectDepth)){
			L += m_ks * hitReflect.material->shade(rayReflect, hitReflect, scene);
		}
	}

	//refraction ray tracing
	if (m_kt != 0){
		//std::cout << "refract!" << std::endl;
		float n;
		float hitAngle = dot(viewDir, hit.N);

		// angle > 0 hit from outside
		// angle < 0 hit from inside
		if (hitAngle >= 0){
			//std::cout << "hit from outside hit angle = " << hitAngle << std::endl;
			n = 1.00029 / 1.33;
			if (1 - pow(n, 2)*(1 - pow(dot(viewDir, hit.N), 2)) < 0){
				return L;
			}
			wr = -1 * n * (viewDir - dot(viewDir, hit.N)*hit.N) - sqrtf(1 - pow(n, 2)*(1 - pow(dot(viewDir, hit.N), 2)))*hit.N;
			wr.normalize();
			HitInfo hitRefract;
			//Ray rayRefract(hit.P + dot(wr, hit.N)*0.0001 * hit.N, wr);
			Ray rayRefract(hit.P + epsilon * wr, wr);
			/*
			Vector3 temp = hit.P + dot(wr, hit.N)*0.0001 * hit.N;
			std::cout << "our: " << temp.x << ", " << temp.y << ", " << temp.z << std::endl;
			temp = hit.P - 0.0001 * hit.N;
			std::cout << "should be: " << temp.x << ", " << temp.y << ", " << temp.z << std::endl;
			std::cout << "wr: " << wr.x << ", " << wr.y << ", " << wr.z << std::endl;
			std::cout << "hit point = " << hit.P.x << "," << hit.P.y << "," << hit.P.z << std::endl;
			std::cout << "hit.N = " << hit.N.x << "," << hit.N.y << "," << hit.N.z << std::endl << std::endl;;
			*/
			//Ray rayRefract(hit.P - hit.N*0.00001, wr);

			/*
			std::cout << "hit.N = " << hit.N.x << "," << hit.N.y << "," << hit.N.z << std::endl;
			std::cout << "ray.o = " << ray.o.x << "," << ray.o.y << "," << ray.o.z << std::endl;
			std::cout << "ray.d = " << ray.d.x << "," << ray.d.y << "," << ray.d.z << std::endl;
			std::cout << "hit point = " << hit.P.x << "," << hit.P.y << "," << hit.P.z << std::endl;
			std::cout << "rayRefract = " << rayRefract.d.x << "," << rayRefract.d.y << "," << rayRefract.d.z << std::endl;
			std::cout << std::endl;
			*/

			if (scene.trace(hitRefract, rayRefract, Material::refractDepth)){
				//std::cout << "hit" << std::endl;
				L += m_kt * hitRefract.material->shade(rayRefract, hitRefract, scene);
			}
		}
		else{
			//std::cout << "hit from inside angle = " << hitAngle << std::endl;
			n = 1.33 / 1.00029;
			if (1 - pow(n, 2)*(1 - pow(dot(ray.d, hit.N), 2)) < 0){
				return L;
			}
			//wr = -1 * n * (viewDir - dot(viewDir, hit.N)*hit.N) - sqrtf(1 - pow(n, 2)*(1 - pow(dot(viewDir, hit.N), 2)))*hit.N;
			wr = n * (ray.d - dot(ray.d, hit.N)*hit.N) + sqrtf(1 - pow(n, 2)*(1 - pow(dot(ray.d, hit.N), 2)))*hit.N;

			wr.normalize();
			HitInfo hitRefract;
			//Ray rayRefract(hit.P + dot(wr, hit.N)*0.0001 * hit.N, wr);
			Ray rayRefract(hit.P + epsilon * wr, wr);
			/*
			Vector3 temp = hit.P + dot(wr, hit.N)*0.0001 * hit.N;
			std::cout << "our: " << temp.x << ", " << temp.y << ", " << temp.z << std::endl;
			temp = hit.P - 0.0001 * hit.N;
			std::cout << "should be: " << temp.x << ", " << temp.y << ", " << temp.z << std::endl;
			std::cout << "wr: " << wr.x << ", " << wr.y << ", " << wr.z << std::endl;
			std::cout << "hit point = " << hit.P.x << "," << hit.P.y << "," << hit.P.z << std::endl;
			std::cout << "hit.N = " << hit.N.x << "," << hit.N.y << "," << hit.N.z << std::endl << std::endl;;
			*/
			/*
			std::cout << "hit.N = " << hit.N.x << "," << hit.N.y << "," << hit.N.z << std::endl;
			std::cout << "ray.o = " << ray.o.x << "," << ray.o.y << "," << ray.o.z << std::endl;
			std::cout << "ray.d = " << ray.d.x << "," << ray.d.y << "," << ray.d.z << std::endl;
			std::cout << "hit point = " << hit.P.x << "," << hit.P.y << "," << hit.P.z << std::endl;
			std::cout << "rayRefract = " << rayRefract.d.x << "," << rayRefract.d.y << "," << rayRefract.d.z << std::endl;
			std::cout << std::endl;
			*/
			if (scene.trace(hitRefract, rayRefract, Material::refractDepth)){
				//std::cout << "hit" << std::endl;
				L += m_kt * hitRefract.material->shade(rayRefract, hitRefract, scene);
			}
		}
	}
    
    // add the ambient component
    L += m_ka;

	if (isShadow && shadowOn){
		//std::cout << "is In Shadow" << std::endl;
		return 0.7 * L;
	}
    
    return L;
}
