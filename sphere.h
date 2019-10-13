#pragma once

#include <optional>

#include "sceneobject.h"
#include "vec3.h"
#include "material.h"

class Sphere : public SceneObject {
private:
    Vec3 _center = {};
    float _radius = {};
    Material _material = {};
public:
    Sphere(Vec3 center, float radius, Material material)
        : _center(center), _radius(radius), _material(material) {}

    Vec3 center() const { return _center; }

    // Inherited via SceneObject
    std::optional<Intersection> intersect(const Ray &ray) const override;
};
