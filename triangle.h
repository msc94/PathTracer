#pragma once

#include "sceneobject.h"

class Triangle : public SceneObject {
    Vec3 _vertex0 = {};
    Vec3 _vertex1 = {};
    Vec3 _vertex2 = {};
    Material _material;
public:
    Triangle() = default;
    Triangle(Vec3 vertex0, Vec3 vertex1, Vec3 vertex2, Material material)
        : _vertex0(vertex0), _vertex1(vertex1), _vertex2(vertex2), _material(material) {}
    ~Triangle() = default;

    // Inherited via SceneObject
    virtual std::optional<Intersection> intersect(const Ray &ray) const override;
};
