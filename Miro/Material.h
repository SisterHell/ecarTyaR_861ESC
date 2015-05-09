#ifndef CSE168_MATERIAL_H_INCLUDED
#define CSE168_MATERIAL_H_INCLUDED

#include "Miro.h"
#include "Vector3.h"
#define MAX_DEPTH 7

class Material
{
public:
    Material();
    virtual ~Material();

    virtual void preCalc() {}
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene) const;

	static int reflectDepth; // this number avoid too many ray trace recursion for refelction
	static int refractDepth; // this number avoid too many ray trace recursion for refraction
};


#endif // CSE168_MATERIAL_H_INCLUDED
