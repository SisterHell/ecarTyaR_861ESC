#pragma once
#include "Vector3.h"
class Photon
{
public:
	Photon();
	virtual ~Photon();

	Vector3 position;
	Vector3 power;
	Vector3 direction;
};
