#pragma once

#include "vec3.h"
#include "material.h"

class Intersection {
    float _distance = 0.0f;
    Vec3 _position = {};
    Vec3 _surfaceNormal = {};
    Material _material = {};

public:
    Intersection() = default;
    Intersection(Vec3 position, Vec3 surfaceNormal, float distance, Material material)
        : _position(position), _surfaceNormal(surfaceNormal.normalize()), _distance(distance), _material(material) {}

    Vec3 position() const { return _position; }
    Vec3 surfaceNormal() const { return _surfaceNormal; }
    float distance() const { return _distance; }
    Material material() const { return _material; }
};
