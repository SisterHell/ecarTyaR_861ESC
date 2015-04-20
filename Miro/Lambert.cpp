#include "Lambert.h"
#include "Ray.h"
#include "Scene.h"
#include <algorithm>

Lambert::Lambert(const Vector3 & kd, const Vector3 & ka,const Vector3 & ks) :
	m_kd(kd), m_ka(ka), m_ks(ks)
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

		Vector3 wr = 2 * dot(viewDir, hit.N) * hit.N + ray.d;
		HitInfo hitReflect;
		Ray rayReflect(hit.P+hit.N * 0.0001,wr);
		

		//std::cout << "ray.d = " << ray.d.x << "," << ray.d.y << "," << ray.d.z << std::endl;
		//std::cout << "wr = " << wr.x << "," << wr.y << "," << wr.z << std::endl;
		//std::cout << "hit point = " << hit.P.x << "," << hit.P.y << "," << hit.P.z << std::endl;
		//std::cin.get();
		if (Material::rayTraceDepth < MAX_DEPTH){
			if (scene.trace(hitReflect, rayReflect)){
				L += m_ks * hitReflect.material->shade(rayReflect, hitReflect, scene);
			}
		}
    }
    
    // add the ambient component
    L += m_ka;
    
    return L;
}
