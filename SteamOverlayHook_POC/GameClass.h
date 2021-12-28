#pragma once
#include "Vectors.h"
class Camera
{
public:
	Matrix4x4 viewmatrix;
	Vector3 Position;
    void* Vtable;
};