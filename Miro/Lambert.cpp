#include "Lambert.h"
#include "Ray.h"
#include "Scene.h"
#include <algorithm>

Lambert::Lambert(const Vector3 & kd, const Vector3 & ka, const Vector3 & ks, const Vector3 & kt) :
m_kd(kd), m_ka(ka), m_ks(ks), m_kt(kt)
{

}

Lambert::~Lambert()
{
}

Vector3
Lambert::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
	Vector3 L = Vector3(0.0f, 0.0f, 0.0f);

	const Vector3 viewDir = -ray.d; // d is a unit vector

	const Lights *lightlist = scene.lights();

	// loop over all of the lights
	Lights::const_iterator lightIter;

	for (lightIter = lightlist->begin(); lightIter != lightlist->end(); lightIter++)
	{
		PointLight* pLight = *lightIter;

		// light direction
		Vector3 l = pLight->position() - hit.P;

		// the inverse-squared falloff
		float falloff = l.length2();

		// normalize the light direction
		l /= sqrt(falloff);

		// get the diffuse component
		float nDotL = dot(hit.N, l);
		Vector3 result = pLight->color();
		result *= m_kd;

		float E = nDotL / falloff * pLight->wattage() / (4 * PI);

		L += std::max(0.0f, E) * result;

		// reflection ray tracing
		Vector3 wr = 2 * dot(viewDir, hit.N) * hit.N + ray.d;
		HitInfo hitReflect;
		Ray rayReflect(hit.P + hit.N * 0.0001, wr);
		//std::cout << "ray.d = " << ray.d.x << "," << ray.d.y << "," << ray.d.z << std::endl;
		//std::cout << "wr = " << wr.x << "," << wr.y << "," << wr.z << std::endl;
		//std::cout << "hit point = " << hit.P.x << "," << hit.P.y << "," << hit.P.z << std::endl;
		//std::cin.get();
		if (m_ks != 0){
			if (scene.trace(hitReflect, rayReflect, Material::reflectDepth)){
				L += m_ks * hitReflect.material->shade(rayReflect, hitReflect, scene);
			}
		}

		//refraction ray tracing
		if (m_kt != 0){
			//std::cout << "refract!" << std::endl;
			float n;

			if (ray.n < 1.3){
				n = 1.00029 / 1.33;
			}
			else{
				n = 1.33 / 1.00029;
			}
			//n = 1.00029 / 1.33;
			wr = -1 * n * (viewDir - dot(viewDir, hit.N)*hit.N) - sqrtf(1 - pow(n, 2)*(1 - pow(dot(viewDir, hit.N), 2)))*hit.N;
			wr.normalize();
			HitInfo hitRefract;
			Ray rayRefract(hit.P+wr*0.001, wr);
			if (1 - pow(n, 2)*(1 - pow(dot(ray.d, hit.N), 2)) < 0){
				return L;
			}
			if (ray.n < 1.3){
				rayRefract.n = 1.33;
			}
			else{
				rayRefract.n = 1.00029;
			}

			
			//std::cout <<"n=" <<n << std::endl;
			//std::cout << "viewdir len = " << viewDir.length() << std::endl;
			//std::cout << "normal = " << hit.N.length() << std::endl;
			//std::cout << "result =" << 1 - pow(n, 2)*(1 - pow(dot(viewDir, hit.N), 2)) << std::endl;
			//std::cout << "ray.d = " << ray.d.x << "," << ray.d.y << "," << ray.d.z << std::endl;
			//std::cout << "viewdir = " << viewDir.x << "," << viewDir.y << "," << viewDir.z << std::endl;
			//std::cout << "wr = " << wr.x << "," << wr.y << "," << wr.z << std::endl;
			//std::cout << "hit point = " << hit.P.x << "," << hit.P.y << "," << hit.P.z << std::endl;
			//std::cout << "rayRefract = " << rayRefract.d.x << "," << rayRefract.d.y << "," << rayRefract.d.z << std::endl;
			//std::cout << std::endl;
		
			//std::cin.get();
			
			if (scene.trace(hitRefract, rayRefract, Material::refractDepth)){
				//std::cout << "hit" << std::endl;
				L += m_kt * hitRefract.material->shade(rayRefract, hitRefract, scene);
			}
			//std::cout << std::endl;
			//std::cout << "refraction done\n" << std::endl;
			//std::cin.get();
		}
		
    }
    
    // add the ambient component
    L += m_ka;
    
    return L;
}
