#include "Material.h"

int Material::reflectDepth;
int Material::refractDepth;
bool shadowOn = true;

Material::Material()
{

}

Material::~Material()
{
}

Vector3
Material::shade(const Ray&, const HitInfo&, const Scene&) const
{
    return Vector3(1.0f, 1.0f, 1.0f);
}
