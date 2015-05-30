#ifndef CSE168_LAMBERT_H_INCLUDED
#define CSE168_LAMBERT_H_INCLUDED

#include "Material.h"



class Lambert : public Material
{
public:
    Lambert(const Vector3 & kd = Vector3(1),
            const Vector3 & ka = Vector3(0),
			const Vector3 & ks = Vector3(0),
			const Vector3 & kt = Vector3(0),
			const bool & NS = false);
    virtual ~Lambert();

    const Vector3 & kd() const {return m_kd;}
    const Vector3 & ka() const {return m_ka;}

    void setKd(const Vector3 & kd) {m_kd = kd;}
    void setKa(const Vector3 & ka) {m_ka = ka;}
	void setKs(const Vector3 & ks) {m_ks = ks;}
	void setKt(const Vector3 & kt) {m_kt = kt;}
	void setNoise(const bool & NS) { noise = NS; }
	void setRn(float rn) { m_rn = rn; }
    virtual void preCalc() {}
    
    virtual Vector3 shade(const Ray& ray, const HitInfo& hit,
                          const Scene& scene, const int depth = 0) const;
	
	float m_rn;		// material refraction index

protected:
    Vector3 m_kd;	// constant for diffuse shading
    Vector3 m_ka;   // constant for ambient shading
	Vector3 m_ks;	// constant for specular reflection
	Vector3 m_kt;   // constant for specular refraction
	bool noise;
};

#endif // CSE168_LAMBERT_H_INCLUDED
