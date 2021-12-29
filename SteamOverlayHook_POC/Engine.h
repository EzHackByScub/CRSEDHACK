#pragma once
#include "Vectors.h"
#include "GameClass.h"
class Engine
{
public:
	static bool Worldtoscreen(Vector3* wtsvec, Vector3 position);
	static inline Camera* camera;
};

