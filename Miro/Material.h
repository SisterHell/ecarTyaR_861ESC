#ifndef CSE168_MATERIAL_H_INCLUDED
#define CSE168_MATERIAL_H_INCLUDED

#include "Miro.h"
#include "Vector3.h"
#define MAX_DEPTH 15

extern bool shadowOn;

class Material
{
public:
	Material();
	virtual ~Material();

	virtual void preCalc() {}

	virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
		const Scene& scene, const int depth = 0) const;

	void isLight(Vector3 color){ light = true; lightColor = color; }
	virtual const Vector3& getKD() const { return Vector3(1, 1, 1); }

	static int reflectDepth; // this number avoid too many ray trace recursion for refelction
	static int refractDepth; // this number avoid too many ray trace recursion for refraction
	int tracingDepth;
	bool light = false;
	Vector3 lightColor;
};


#endif // CSE168_MATERIAL_H_INCLUDED