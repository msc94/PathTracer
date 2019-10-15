#include "scene.h"

#include <algorithm>

#include "sphere.h"
#include "triangle.h"

std::vector<std::unique_ptr<SceneObject>> createRectangleSurface(Vec3 origin, Vec3 dir1, Vec3 dir2, Material material) {
    Vec3 v1 = origin;
    Vec3 v2 = origin + dir1;
    Vec3 v3 = origin + dir2;
    Vec3 v4 = origin + dir1 + dir2;

    auto vec = std::vector<std::unique_ptr<SceneObject>>();
    vec.push_back(std::make_unique<Triangle>(v1, v2, v3, material));
    vec.push_back(std::make_unique<Triangle>(v2, v4, v3, material));
    return vec;
}

void Scene::initialize() {
    _camera = Camera(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));

    auto whiteEmittingColor = Material::white().setEmittingColor(Color(255, 255, 255));
    _light = std::make_unique<Sphere>(Vec3(0.0f, 30.0f, 10.0f), 5.0, whiteEmittingColor);

    _objects.push_back(
        std::make_unique<Sphere>(Vec3(5.0f, -3.0f, 50.0f), 5.0, Material::red().setReflectingPercent(0.1f))
    );
    _objects.push_back(
        std::make_unique<Sphere>(Vec3(-5.0f, 5.0f, 30.0f), 5.0, Material::green().setReflectingPercent(0.2f))
    );
    _objects.push_back(
        std::make_unique<Sphere>(Vec3(15.0f, 15.0f, 60.0f), 5.0, Material::blue().setReflectingPercent(1.0f))
    );
    _objects.push_back(
        std::make_unique<Sphere>(Vec3(-15.0f, -15.0f, 60.0f), 5.0, Material::pink())
    );


    auto boundaryColor = Material::white();
    // Floor
    auto floorTriangles = createRectangleSurface(
        Vec3(-500.0f, -40.0f, -0.0f),
        Vec3(0.0f, 0.0f, 1000.0f),
        Vec3(1000.0f, 0.0f, 0.0f),
        boundaryColor
    );
    _objects.insert(_objects.end(), 
        std::make_move_iterator(floorTriangles.begin()), 
        std::make_move_iterator(floorTriangles.end())
    );

    // Ceiling
    auto ceilingTriangles = createRectangleSurface(
        Vec3(-500.0f, 40.0f, -0.0f),
        Vec3(1000.0f, 0.0f, 0.0f),
        Vec3(0.0f, 0.0f, 1000.0f),
        boundaryColor
    );
    _objects.insert(_objects.end(),
        std::make_move_iterator(ceilingTriangles.begin()),
        std::make_move_iterator(ceilingTriangles.end())
    );

    // Left wall
    auto leftWallTriangles = createRectangleSurface(
        Vec3(-40.0f, -500.0f, -0.0f),
        Vec3(0.0f, 1000.0f, 0.0f),
        Vec3(0.0f, 0.0f, 1000.0f),
        boundaryColor
    );
    _objects.insert(_objects.end(),
        std::make_move_iterator(leftWallTriangles.begin()),
        std::make_move_iterator(leftWallTriangles.end())
    );

    // Right wall
    auto rightWallTriangles = createRectangleSurface(
        Vec3(40.0f, -500.0f, -0.0f),
        Vec3(0.0f, 0.0f, 1000.0f),
        Vec3(0.0f, 1000.0f, 0.0f),
        boundaryColor
    );
    _objects.insert(_objects.end(),
        std::make_move_iterator(rightWallTriangles.begin()),
        std::make_move_iterator(rightWallTriangles.end())
    );

    // Front wall
    auto frontWallTriangles = createRectangleSurface(
        Vec3(-500.0f, -500.0f, -0.5f),
        Vec3(1000.0f, 0.0f, 0.0f),
        Vec3(0.0f, 1000.0f, 0.0f),
        boundaryColor
    );
    _objects.insert(_objects.end(),
        std::make_move_iterator(frontWallTriangles.begin()),
        std::make_move_iterator(frontWallTriangles.end())
    );

    // Back wall
    auto backWallTriangles = createRectangleSurface(
        Vec3(-500.0f, -500.0f, 150.0f),
        Vec3(0.0f, 1000.0f, 0.0f),
        Vec3(1000.0f, 0.0f, 0.0f),
        boundaryColor
    );
    _objects.insert(_objects.end(),
        std::make_move_iterator(backWallTriangles.begin()),
        std::make_move_iterator(backWallTriangles.end())
    );

    // Easy object exactly on the x and y axis. Easy to debug intersection problems.
    //_objects.push_back(
    //    std::make_unique<Sphere>(Vec3(0.0f, 0.0f, 30.0f), 5.0, Material::green())
    //);
}

std::optional<Intersection> Scene::firstIntersection(const Ray &ray) const {
    auto intersections = std::vector<Intersection>();
    // Make this reasonable sized, so we don't hit as many array resizings.
    intersections.reserve(16);

    for (const auto &object : _objects) {
        auto intersection = object->intersect(ray);
        if (intersection) {
            intersections.push_back(*intersection);
        }
    }

    // TODO: Is light just another scene object?
    auto lightIntersection = _light->intersect(ray);
    if (lightIntersection) {
        intersections.push_back(*lightIntersection);
    }

    if (intersections.empty()) {
        return std::optional<Intersection>();
    }

    return *std::min_element(intersections.begin(), intersections.end(), [&](const Intersection &a, const Intersection &b) {
        return a.distance() < b.distance();
    });
}

bool Scene::hitsLight(const Ray &ray) const {
    std::vector<Intersection> intersections;
    for (const auto &object : _objects) {
        auto intersection = object->intersect(ray);
        if (intersection) {
            intersections.push_back(*intersection);
        }
    }

    // TODO: Is light just another scene object?
    auto lightIntersection = _light->intersect(ray);

    if (!lightIntersection) {
        return false;
    }

    // Are all other intersections distances bigger?
    return std::all_of(intersections.begin(), intersections.end(), [&](const Intersection &i) {
        return i.distance() > lightIntersection->distance();
    });
}
