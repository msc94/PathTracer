#include <iostream>
#include <optional>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <vector>
#include <cstdio>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

class Scene;
class Material;

template<class T>
struct Vec3T {
    using VecType = Vec3T<T>;

    T _x = 0.0f;
    T _y = 0.0f;
    T _z = 0.0f;

public:
    Vec3T(T x, T y, T z)
        : _x(x), _y(y), _z(z) {}

    Vec3T()
        : Vec3T(0.0f, 0.0f, 0.0f) {}

    T x() const { return _x; }
    T y() const { return _y; }
    T z() const { return _z; }

    Vec3T operator+ (const VecType &other) const {
        return VecType(
            _x + other.x(),
            _y + other.y(),
            _z + other.z()
        );
    }

    Vec3T operator- (const VecType &other) const {
        return VecType(
            _x - other.x(),
            _y - other.y(),
            _z - other.z()
        );
    }

    template<class K>
    Vec3T operator* (K scalar) const {
        return VecType(
            _x * scalar,
            _y * scalar,
            _z * scalar
        );
    }

    template<class K>
    Vec3T operator/ (K scalar) const {
        return VecType(
            _x / scalar,
            _y / scalar,
            _z / scalar
        );
    }

    T dot(const VecType &other) const {
        T dot = 0;
        dot += _x * other.x();
        dot += _y * other.y();
        dot += _z * other.z();
        return dot;
    }

    T length() const {
        return std::sqrt(
            std::pow(_x, 2) +
            std::pow(_y, 2) +
            std::pow(_z, 2)
        );
    }

    Vec3T normalize() const {
        return *this / length();
    }
};

using Vec3 = Vec3T<float>;
using Color = Vec3T<int>;

class Material {
    Color _color = {};
public:
    Material() = default;
    Material(Color color)
        : _color(color) {}

    Color color() const { return _color; }

    static Material red() { return Material(Color(255, 0, 0)); }
    static Material green() { return Material(Color(0, 255, 0)); }
    static Material blue() { return Material(Color(0, 0, 255)); }
    static Material black() { return Material(Color(0, 0, 0)); }
    static Material pink() { return Material(Color(255, 192, 203)); }
};

class Intersection {
    float _distance = 0.0f;
    Vec3 _position = {};
    Vec3 _surfaceNormal = {};
    Material _material = {};

public:
    Intersection() = default;
    Intersection(Vec3 position, Vec3 surfaceNormal, float distance, Material material)
        : _position(position), _surfaceNormal(surfaceNormal.normalize()), _distance(distance), _material(material) {}

    Vec3 position() const { return _position; }
    Vec3 surfaceNormal() const { return _surfaceNormal; }
    float distance() const { return _distance; }
    Material material() const { return _material; }
};

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

class SceneObject {
public:
    SceneObject() = default;
    virtual ~SceneObject() = default;
    virtual std::optional<Intersection> intersect(const Ray &ray) const = 0;
};

class Sphere : public SceneObject {
private:
    Vec3 _center = {};
    float _radius = {};
    Material _material = {};
public:
    Sphere(Vec3 center, float radius, Material material)
        : _center(center), _radius(radius), _material(material) {}

    // Inherited via SceneObject
    std::optional<Intersection> intersect(const Ray &ray) const override {
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
};

class Scene {
    Camera _camera = {};
    std::vector<std::unique_ptr<SceneObject>> _objects = {};
    Vec3 _lightOrigin = {};
public:
    void initialize() {
        _camera = Camera(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f));
        _lightOrigin = Vec3(0.0f, 50.0f, 30.0f);

        _objects.push_back(
            std::make_unique<Sphere>(Vec3(5.0f, -3.0f, 50.0f), 5.0, Material::red())
        );
        _objects.push_back(
            std::make_unique<Sphere>(Vec3(-5.0f, 5.0f, 30.0f), 5.0, Material::green())
        );
        _objects.push_back(
            std::make_unique<Sphere>(Vec3(15.0f, 15.0f, 60.0f), 5.0, Material::blue())
        );
        _objects.push_back(
            std::make_unique<Sphere>(Vec3(-15.0f, -15.0f, 60.0f), 5.0, Material::pink())
        );

        // Easy object exactly on the x and y axis. Easy to debug intersection problems.
        //_objects.push_back(
        //    std::make_unique<Sphere>(Vec3(0.0f, 0.0f, 30.0f), 5.0, Material::green())
        //);
    }

    Camera camera() const { return _camera; }
    Vec3 light() const { return _lightOrigin; }
    // std::vector<SceneObject> objects() const { return _objects; }

    std::optional<Intersection> firstIntersection(const Ray &ray) const {
        std::vector<Intersection> intersections;
        for (const auto &object : _objects) {
            auto intersection = object->intersect(ray);
            if (intersection) {
                intersections.push_back(intersection.value());
            }
        }

        if (intersections.empty()) {
            return std::optional<Intersection>();
        }

        return *std::min_element(intersections.begin(), intersections.end(), [&](const Intersection &a, const Intersection &b) {
            return a.distance() < b.distance();
        });
    }
};

Color shootRay(const Ray &ray, const Scene &scene, int depth) {

}

Color shootRayforPixel(int x, int y, const Scene &scene) {
    auto camera = scene.camera();
    // TODO: This hard codes the camera direction vector. Change.
    auto pointOnVirtualScreen = camera.origin() + Vec3(float(x), float(y), 500.0f);
    auto rayDirection = pointOnVirtualScreen - camera.origin();
    auto ray = Ray(camera.origin(), rayDirection);

    auto intersection = scene.firstIntersection(ray);
    if (intersection) {
        auto color = intersection->material().color();

        auto intersectionPosition = intersection->position();

        // Easy to debug intersection problems. Use with the last object in the scene (uncommented).
        if (intersectionPosition.y() > 4.5f) {
            int breakhere = 3;
        }

        // Shoot ray to the light
        auto lightPosition = scene.light();
        auto lightRayDirection = lightPosition - intersection->position();

        // Move the origin a little bit out of the object so it does not hit itself
        auto lightRayOrigin = intersection->position() + intersection->surfaceNormal() * 0.5f;
        auto rayToLight = Ray(lightRayOrigin, lightRayDirection);

        lightRayDirection = lightRayDirection.normalize();
        rayDirection = rayDirection.normalize();

        // TODO: Why do we have to negate here? Both directions should give us the correct cosine.
        auto cosBetweenNormalAndLight = lightRayDirection.dot(intersection->surfaceNormal());
        cosBetweenNormalAndLight = std::max(0.2f, cosBetweenNormalAndLight);
        color = color * cosBetweenNormalAndLight;

        auto lightIntersection = scene.firstIntersection(rayToLight);
        if (lightIntersection) {
            auto position = lightIntersection->position();
            // We hit the world
            // color = color * 0.8;
            // color = Color();
        }
        return color;
    }
    return Color(70, 70, 70);
}

int main(int argc, char **argv) {
    SDL_SetMainReady();

    Scene scene = {};
    scene.initialize();

    const auto WINDOW_WIDTH = 500;
    const auto WINDOW_HEIGHT = 500;

    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;
    int i;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    // Shoot rays
    // TODO: Get orthogonal plane to direction vector?
    for (auto x = 0; x < WINDOW_WIDTH; x++) {
        for (auto y = 0; y < WINDOW_WIDTH; y++) {
            auto moved_x = x - (WINDOW_WIDTH / 2);
            // Positive y is up in world space, but in screen (sdl) space its down
            auto moved_y = (WINDOW_WIDTH / 2) - y;
            auto pixelColor = shootRayforPixel(moved_x, moved_y, scene);
            SDL_SetRenderDrawColor(renderer,
                pixelColor.x(), pixelColor.y(), pixelColor.z(), 255);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

    SDL_RenderPresent(renderer);
    while (1) {
        if (SDL_PollEvent(&event) && 
            event.type == SDL_QUIT)
            break;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}