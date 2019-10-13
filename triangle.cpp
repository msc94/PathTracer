#include "triangle.h"

#include <limits>

std::optional<Intersection> Triangle::intersect(const Ray &ray) const {
    const float EPSILON = std::numeric_limits<float>::epsilon();

    Vec3 edge1 = _vertex1 - _vertex0;
    Vec3 edge2 = _vertex2 - _vertex0;

    Vec3 h = ray.direction().cross(edge2);
    float a = edge1.dot(h);

    if (a > -EPSILON && a < EPSILON)
        return std::optional<Intersection>();    // This ray is parallel to this triangle.

    float f = 1.0 / a;
    Vec3 s = ray.origin() - _vertex0;
    float u = f * s.dot(h);
    if (u < 0.0 || u > 1.0)
        return std::optional<Intersection>();

    Vec3 q = s.cross(edge1);
    float v = f * ray.direction().dot(q);
    if (v < 0.0 || u + v > 1.0)
        return std::optional<Intersection>();

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * edge2.dot(q);
    if (t > EPSILON && t < 1 / EPSILON) // ray intersection
    {
        auto intersectionPosition = ray.origin() + ray.direction() * t;
        auto intersection = Intersection(
            intersectionPosition,
            edge1.cross(edge2),
            t,
            _material
        );
        return intersection;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return std::optional<Intersection>();    // This ray is parallel to this triangle.
}
