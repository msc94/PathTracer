#pragma once

#include <optional>
#include <vector>
#include <memory>

#include "intersection.h"
#include "ray.h"
#include "camera.h"
#include "sphere.h"
#include "sceneobject.h"

class Scene {
    Camera _camera = {};
    std::vector<std::unique_ptr<SceneObject>> _objects = {};
    std::unique_ptr<Sphere> _light = {};

public:
    Camera camera() const { return _camera; }
    Vec3 light() const { return _light->center(); }

    void initialize();
    std::optional<Intersection> firstIntersection(const Ray &ray) const;
    bool hitsLight(const Ray &ray) const;
};
