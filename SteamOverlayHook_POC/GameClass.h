#pragma once
#include "Vectors.h"
class Camera
{
public:
	Matrix4x4 viewmatrix;
	Vector3 Position;
    void* Vtable;
};
class Actor
{
public:
	char fuckthisengine[0x24];
	Vector3 Position;
};
class WeaponCros
{
public:
	char fuckthisengine[0x354];
	Vector2 AimAngles;
};
class ActorArray
{
public:
	Actor* pActor[312];
};