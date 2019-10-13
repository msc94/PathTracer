#include "sphere.h"

#include <cmath>

std::optional<Intersection> Sphere::intersect(const Ray &ray) const {
    auto oc = ray.origin() - _center;
    auto a = ray.direction().dot(ray.direction());
    auto b = 2.0 * oc.dot(ray.direction());
    auto c = oc.dot(oc) - _radius * _radius;
    auto discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return std::optional<Intersection>();
    }

    float distance = (-b - std::sqrtf(discriminant)) / (2.0f * a);

    // We do not go backwards along the ray.
    if (distance < 0.0f) {
        return std::optional<Intersection>();
    }

    Vec3 hit = ray.origin() + ray.direction() * distance;
    // The normal is just a vector from the origin to the hit
    Vec3 normal = (hit - _center).normalize();

    return Intersection(
        hit,
        normal,
        distance,
        _material
    );
}
