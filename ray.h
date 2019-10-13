#pragma once

#include "vec3.h"

class Ray {
    Vec3 _origin = {};
    Vec3 _direction = {};

public:
    Ray() = default;
    Ray(Vec3 origin, Vec3 direction)
        : _origin(origin), _direction(direction.normalize()) {}

    Vec3 origin() const { return _origin; }
    Vec3 direction() const { return _direction; }
};

