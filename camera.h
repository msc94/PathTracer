#pragma once

#include "vec3.h"

class Camera {
    Vec3 _position = {};
    Vec3 _direction = {};

public:
    Camera() = default;
    Camera(Vec3 position, Vec3 direction)
        : _position(position), _direction(direction) {}

    Vec3 origin() const { return _position; }
    Vec3 direction() const { return _direction; }
};

