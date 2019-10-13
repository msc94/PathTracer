#pragma once

#include <optional>

#include "intersection.h"
#include "ray.h"

class SceneObject {
public:
    SceneObject() = default;
    virtual ~SceneObject() = default;
    virtual std::optional<Intersection> intersect(const Ray &ray) const = 0;
};
