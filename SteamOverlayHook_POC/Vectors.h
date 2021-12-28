#pragma once
#include <math.h>
class Vector2
{
public:
    float x, y;

    Vector2& operator+(Vector2 arg)
    {
        x += arg.x;
        y += arg.y;

        return *this;
    }
    Vector2& operator-(Vector2 arg)
    {
        x -= arg.x;
        y -= arg.y;

        return *this;
    }

    Vector2& operator*(float arg)
    {
        x *= arg;
        y *= arg;

        return *this;
    }
};

struct Vector3
{
    float x, y, z;

    Vector3& operator+(Vector3 arg)
    {
        x += arg.x;
        y += arg.y;
        z += arg.z;

        return *this;
    }
    Vector3& operator-(Vector3 arg)
    {
        x -= arg.x;
        y -= arg.y;
        z -= arg.z;

        return *this;
    }

    Vector3& operator*(float arg)
    {
        x *= arg;
        y *= arg;
        z *= arg;

        return *this;
    }
    inline float Dot(Vector3 v)
    {
        return x * v.x + y * v.y + z * v.z;
    }

    inline float Distance(Vector3 v)
    {
        return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
    }
};

    struct Matrix4x4 {
        union {
            struct {
                float        _11, _12, _13, _14;
                float        _21, _22, _23, _24;
                float        _31, _32, _33, _34;
                float        _41, _42, _43, _44;

            }; float m[4][4];
        };
    };


