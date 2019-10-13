#pragma once

#include <optional>
#include <vector>
#include <memory>

#include "intersection.h"
#include "ray.h"
#include "camera.h"
#include "sceneobject.h"

class Scene {
    Camera _camera = {};
    std::vector<std::unique_ptr<SceneObject>> _objects = {};
    Vec3 _lightOrigin = {};

public:
    Camera camera() const { return _camera; }
    Vec3 light() const { return _lightOrigin; }

    void initialize();
    std::optional<Intersection> firstIntersection(const Ray &ray) const;
};
