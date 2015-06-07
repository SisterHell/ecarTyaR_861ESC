#pragma once
#include "Vector3.h"
#include "Miro.h"
#include <vector>

class Sampler
{
public:
	Sampler();
	virtual ~Sampler();

	Vector3 generate_sphere_sample(float e, Vector3&, Vector3&, Vector3&);
	Vector3 generate_vector_sample();
};