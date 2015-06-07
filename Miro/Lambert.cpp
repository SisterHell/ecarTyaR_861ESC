#include "Lambert.h"
#include "Ray.h"
#include "Scene.h"
#include <algorithm>
#include "Worley.h"
#include "Perlin.h"
#include "Sampler.h"

Lambert::Lambert(const Vector3 & kd, const Vector3 & ka, const Vector3 & ks, const Vector3 & kt, const bool &NS) :
m_kd(kd), m_ka(ka), m_ks(ks), m_kt(kt), noise()
{

}

Lambert::~Lambert()
{
}

Vector3
Lambert::shade(const Ray& ray, const HitInfo& hit, const Scene& scene, const int depth) const
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
	}
	float* at = new float[3];
	at[0] = hit.P.x * 10; at[1] = hit.P.y * 10; at[2] = hit.P.z * 10;
	float* F = new float[2];
	float(*delta)[3] = new float[3][3];

	unsigned long *ID = new unsigned long;
	if (noise)
		WorleyNoise::noise3D(at, 2, F, delta, ID);

	float factor;
	if (noise){
		factor = (F[1] - F[0]);
	}
	if (scene.photonTrace){
		int num = 0;
		for (int i = 0; i < scene.phoMap.photonList.size(); i++){
			if ((scene.phoMap.photonList[i]->position - hit.P).length() <= 0.1){
				num++;
				L += scene.phoMap.photonList[i]->power;// *dot(hit.N, scene.phoMap.photonList[i]->direction.negate());
			}
		}
		//L.print();
	}
	else if (!scene.pathTrace){
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
					}
				}
			}


			// get the diffuse component
			float nDotL = dot(hit.N, l);
			Vector3 result = pLight->color();
			if (noise){
				//setKd(Vector3(factor));
				//factor
				result *= Vector3(factor);
				Vector3 nV(Pnoise);
				if (nV.x < 0) nV.x = 0;
				if (nV.y < 0) nV.y = 0;
				if (nV.z < 0) nV.z = 0;
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

			Vector3 wr;

			if (m_ks != 0){

				wr = 2 * dot(viewDir, hit.N) * hit.N + ray.d;
				L += m_ks * std::max(0.0f, E) * powf(dot(viewDir, wr), 10) / (nDotL);
			}


		}
		Vector3 wr;

		// reflection ray tracing
		if (m_ks != 0){

			wr = 2 * dot(viewDir, hit.N) * hit.N + ray.d;

			HitInfo hitReflect;
			Ray rayReflect(hit.P + wr * epsilon, wr);

			if (scene.trace(hitReflect, rayReflect, Material::reflectDepth)){
				L += m_ks * hitReflect.material->shade(rayReflect, hitReflect, scene);
			}
			else{
				return scene.bgColor;
			}
		}

		//refraction ray tracing
		if (m_kt != 0){
			float n;
			float hitAngle = dot(viewDir, hit.N);

			if (hitAngle >= 0){
				n = 1.00029 / 1.33;
				if (1 - pow(n, 2)*(1 - pow(dot(viewDir, hit.N), 2)) < 0){
					delete at;
					delete F;
					delete delta;
					delete ID;
					return L;
				}
				wr = -1 * n * (viewDir - dot(viewDir, hit.N)*hit.N) - sqrtf(1 - pow(n, 2)*(1 - pow(dot(viewDir, hit.N), 2)))*hit.N;
				wr.normalize();
				HitInfo hitRefract;

				Ray rayRefract(hit.P + epsilon * wr, wr);

				if (scene.trace(hitRefract, rayRefract, Material::refractDepth)){
					L += m_kt * hitRefract.material->shade(rayRefract, hitRefract, scene);
				}
			}
			else{
				n = 1.33 / 1.00029;
				if (1 - pow(n, 2)*(1 - pow(dot(ray.d, hit.N), 2)) < 0){
					delete at;
					delete F;
					delete delta;
					delete ID;
					return L;
				}
				wr = n * (ray.d - dot(ray.d, hit.N)*hit.N) + sqrtf(1 - pow(n, 2)*(1 - pow(dot(ray.d, hit.N), 2)))*hit.N;

				wr.normalize();
				HitInfo hitRefract;
				Ray rayRefract(hit.P + epsilon * wr, wr);

				if (scene.trace(hitRefract, rayRefract, Material::refractDepth)){
					L += m_kt * hitRefract.material->shade(rayRefract, hitRefract, scene);
				}
			}
		}

		// add the ambient component
		L += m_ka;

		if (isShadow && shadowOn){
			L *= 0.7;
		}
	}

	else{ // path trace
		if (depth > MAX_DEPTH){
			delete at;
			delete F;
			delete delta;
			delete ID;
			return L;
		}
		else if (depth > 0){

			Vector3 reflectedColor;
			if (hit.material->light){
				//std::cout << "hit light" << std::endl;

				//std::cout << "light color: " << hit.material->lightColor.x << ", " << hit.material->lightColor.y << ", " << hit.material->lightColor.z << std::endl;
				delete at;
				delete F;
				delete delta;
				delete ID;
				return hit.material->lightColor;
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
						}
					}
				}


				// get the diffuse component
				float nDotL = dot(hit.N, l);
				Vector3 result = pLight->color();
				if (noise){
					result *= m_kd * Vector3(factor);
					Vector3 nV(Pnoise);
					if (nV.x < 0) nV.x = 0;
					if (nV.y < 0) nV.y = 0;
					if (nV.z < 0) nV.z = 0;
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

				Vector3 wr;

				if (m_ks != 0){

					wr = 2 * dot(viewDir, hit.N) * hit.N + ray.d;
					L += m_ks * std::max(0.0f, E) * powf(dot(viewDir, wr), 80) / (nDotL);
				}


			}
			Vector3 wr;

			// reflection ray tracing
			if (m_ks != 0){

				wr = 2 * dot(viewDir, hit.N) * hit.N + ray.d;
				HitInfo hitReflect;
				Ray rayReflect(hit.P + wr * epsilon, wr);

				if (scene.trace(hitReflect, rayReflect, Material::reflectDepth)){
					L += m_ks * hitReflect.material->shade(rayReflect, hitReflect, scene);
				}
			}

			//refraction ray tracing
			if (m_kt != 0){
				float n;
				float hitAngle = dot(viewDir, hit.N);

				if (hitAngle >= 0){
					n = 1.00029 / 1.33;
					if (1 - pow(n, 2)*(1 - pow(dot(viewDir, hit.N), 2)) < 0){
						delete at;
						delete F;
						delete delta;
						delete ID;
						return L;
					}
					wr = -1 * n * (viewDir - dot(viewDir, hit.N)*hit.N) - sqrtf(1 - pow(n, 2)*(1 - pow(dot(viewDir, hit.N), 2)))*hit.N;
					wr.normalize();
					HitInfo hitRefract;
					Ray rayRefract(hit.P + epsilon * wr, wr);

					if (scene.trace(hitRefract, rayRefract, Material::refractDepth)){
						L += m_kt * hitRefract.material->shade(rayRefract, hitRefract, scene);
					}
				}
				else{
					n = 1.33 / 1.00029;
					if (1 - pow(n, 2)*(1 - pow(dot(ray.d, hit.N), 2)) < 0){
						delete at;
						delete F;
						delete delta;
						delete ID;
						return L;
					}
					wr = n * (ray.d - dot(ray.d, hit.N)*hit.N) + sqrtf(1 - pow(n, 2)*(1 - pow(dot(ray.d, hit.N), 2)))*hit.N;

					wr.normalize();
					HitInfo hitRefract;
					Ray rayRefract(hit.P + epsilon * wr, wr);

					if (scene.trace(hitRefract, rayRefract, Material::refractDepth)){
						L += m_kt * hitRefract.material->shade(rayRefract, hitRefract, scene);
					}
				}
			}

			// add the ambient component
			L += m_ka;

			if (isShadow && shadowOn){
				L *= 0.7;
			}

			HitInfo path;
			Vector3 newDir;
			float cosTheta = -1;
			do{
				float d;
				float x, y, z;
				do{
					x = -1.0f + (2.0f *((double)rand() / (RAND_MAX)));
					y = -1.0f + (2.0f *((double)rand() / (RAND_MAX)));
					z = -1.0f + (2.0f *((double)rand() / (RAND_MAX)));
					d = sqrtf(x * x + y * y + z * z);
					//std::cout << "I'm stuck in random choice." << std::endl;
					//std::cout << x << std::endl;
				} while (d > 1);
				x /= d;
				y /= d;
				z /= d;
				newDir.x = x;
				newDir.y = y;
				newDir.z = z;
				cosTheta = dot(newDir, hit.N);
			} while (cosTheta < 0);
			Ray pathRay(hit.P + newDir*epsilon, newDir);
			Vector3 brdf = 2 * m_kd * cosTheta / PI;

			if (scene.trace(path, pathRay)){
				//std::cout << "current << std::endl;
				Vector3 s = path.material->shade(pathRay, path, scene, depth + 1);
				reflectedColor = L + brdf * s;
				//if (reflectedColor.x != 0){
				//std::cout << "current reflected color: " << reflectedColor.x << ", " << reflectedColor.y << ", " << reflectedColor.z << std::endl;
				//std::cout << "s: " << s.x << ", " << s.y << ", " << s.z << std::endl;
				//std::cout << "brdf: " << brdf.x << ", " << brdf.y << ", " << brdf.z << std::endl;

				//}
			}
			delete at;
			delete F;
			delete delta;
			delete ID;
			return reflectedColor;
		}
		else{
			//std::cout << "I'm stuck1" << std::endl;

			Vector3 reflectedColor;
			Vector3 finalColor;
			if (hit.material->light){
				delete at;
				delete F;
				delete delta;
				delete ID;
				return hit.material->lightColor;
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
						}
					}
				}


				// get the diffuse component
				float nDotL = dot(hit.N, l);
				Vector3 result = pLight->color();
				if (noise){
					result *= m_kd * Vector3(factor);
					Vector3 nV(Pnoise);
					if (nV.x < 0) nV.x = 0;
					if (nV.y < 0) nV.y = 0;
					if (nV.z < 0) nV.z = 0;
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

				Vector3 wr;

				if (m_ks != 0){

					wr = 2 * dot(viewDir, hit.N) * hit.N + ray.d;
					L += m_ks * std::max(0.0f, E) * powf(dot(viewDir, wr), 80) / (nDotL);
				}


			}
			Vector3 wr;

			// reflection ray tracing
			if (m_ks != 0){

				wr = 2 * dot(viewDir, hit.N) * hit.N + ray.d;
				HitInfo hitReflect;
				Ray rayReflect(hit.P + wr * epsilon, wr);

				if (scene.trace(hitReflect, rayReflect, Material::reflectDepth)){
					L += m_ks * hitReflect.material->shade(rayReflect, hitReflect, scene);
				}
			}

			//refraction ray tracing
			if (m_kt != 0){
				float n;
				float hitAngle = dot(viewDir, hit.N);

				if (hitAngle >= 0){
					n = 1.00029 / 1.33;
					if (1 - pow(n, 2)*(1 - pow(dot(viewDir, hit.N), 2)) < 0){
						delete at;
						delete F;
						delete delta;
						delete ID;
						return L;
					}
					wr = -1 * n * (viewDir - dot(viewDir, hit.N)*hit.N) - sqrtf(1 - pow(n, 2)*(1 - pow(dot(viewDir, hit.N), 2)))*hit.N;
					wr.normalize();
					HitInfo hitRefract;
					Ray rayRefract(hit.P + epsilon * wr, wr);

					if (scene.trace(hitRefract, rayRefract, Material::refractDepth)){
						L += m_kt * hitRefract.material->shade(rayRefract, hitRefract, scene);
					}
				}
				else{
					n = 1.33 / 1.00029;
					if (1 - pow(n, 2)*(1 - pow(dot(ray.d, hit.N), 2)) < 0){
						delete at;
						delete F;
						delete delta;
						delete ID;
						return L;
					}
					wr = n * (ray.d - dot(ray.d, hit.N)*hit.N) + sqrtf(1 - pow(n, 2)*(1 - pow(dot(ray.d, hit.N), 2)))*hit.N;

					wr.normalize();
					HitInfo hitRefract;
					Ray rayRefract(hit.P + epsilon * wr, wr);

					if (scene.trace(hitRefract, rayRefract, Material::refractDepth)){
						L += m_kt * hitRefract.material->shade(rayRefract, hitRefract, scene);
					}
				}
			}

			// add the ambient component
			L += m_ka;

			if (isShadow && shadowOn){
				L *= 0.7;
			}

			for (int i = 0; i < 1000; i++){
				//std::cout << "I'm stuck here" << std::endl;
				HitInfo path;
				Vector3 newDir;
				float cosTheta = -1;
				do{
					float d;
					float x, y, z;
					do{
						x = -1.0f + (2.0f *((double)rand() / (RAND_MAX)));
						y = -1.0f + (2.0f *((double)rand() / (RAND_MAX)));
						z = -1.0f + (2.0f *((double)rand() / (RAND_MAX)));
						d = sqrtf(x * x + y * y + z * z);
						//std::cout << "I'm stuck in random choice." << std::endl;
						//std::cout << x << std::endl;
					} while (d > 1);
					x /= d;
					y /= d;
					z /= d;
					newDir.x = x;
					newDir.y = y;
					newDir.z = z;
					cosTheta = dot(newDir, hit.N);
				} while (cosTheta < 0);
				Ray pathRay(hit.P + newDir*epsilon, newDir);
				Vector3 brdf = 2 * m_kd * cosTheta / PI;

				if (scene.trace(path, pathRay)){
					//std::cout << "current << std::endl;
					finalColor += L + brdf * path.material->shade(pathRay, path, scene, depth + 1);
					//std::cout << "current reflected color: " << reflectedColor.x << ", " << reflectedColor.y << ", " << reflectedColor.z << std::endl;
				}
			}

			//std::cout << "reflected color: " << reflectedColor.x << ", " << reflectedColor.y << ", " << reflectedColor.z << std::endl;
			L = finalColor / 1000.0f;
		}
	}
    
	delete at;
	delete F;
	delete delta;
	if (!noise)
	delete ID;

    return L;
}
