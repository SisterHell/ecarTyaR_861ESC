#include "Sampler.h"


Sampler::Sampler()
{
}


Sampler::~Sampler()
{
}

Vector3 Sampler::generate_sphere_sample(float e, Vector3& u, Vector3& v, Vector3& w)
{
	float x = ((float)rand() / (RAND_MAX));
	float y = ((float)rand() / (RAND_MAX));

	float cos_phi = cosf(2 * PI * x);
	float sin_phi = sinf(2 * PI * x);
	float cos_the = pow(1.0 - y, 1.0 / (e + 1.0));
	float sin_the = sqrtf(1.0 - cos_the * cos_the);
	float px = sin_the * cos_phi;
	float py = sin_the * sin_phi;
	float pz = cos_the;

	Vector3 result = px * u + py *w + pz*v;
	result.normalize();
	return result;
}

Vector3 Sampler::generate_vector_sample()
{
	float x = -1.0f + (2.0f *((float)rand() / (RAND_MAX)));
	float y = -1.0f + (2.0f *((float)rand() / (RAND_MAX)));
	float z = -1.0f + (2.0f *((float)rand() / (RAND_MAX)));

	Vector3 result(x, y, z);
	result.normalize();
	return result;
}