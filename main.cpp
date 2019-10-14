#include <iostream>
#include <optional>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <vector>
#include <deque>
#include <cstdio>
#include <future>
#include <chrono>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "scene.h"
#include "ray.h"
#include "vec3.h"


Color shootRay(const Ray &ray, const Scene &scene, int depth) {
    const int MAX_DEPTH = 3;
    if (depth > MAX_DEPTH) {
        return Color(50, 50, 50);
    }

    auto intersection = scene.firstIntersection(ray);
    if (!intersection) {
        // Hit outside of the world
        return Color(70, 70, 70);
    }
    auto material = intersection->material();
    auto emittingColor = material.emittingColor();
    if (emittingColor) {
        return emittingColor.value();
    }

    auto selfColor = material.color();
    // return selfColor;

    // Shoot a random ray, to simulate global illumination
    auto randomDirection = vectorutils::createRandomVectorInHemisphere(intersection->surfaceNormal());
    auto newRay = Ray(intersection->position(), randomDirection);
    auto randomVecColor = shootRay(newRay, scene, depth + 1);
    randomVecColor = randomVecColor * 0.7f;
    return colorutils::multiplyColors(selfColor, randomVecColor);

    auto color = Color();
    auto reflectionPercent = material.reflectionPercent();
    if (reflectionPercent) {
        auto normal = intersection->surfaceNormal();

        auto newRayOrigin = intersection->position() + normal * 0.5f;
        auto delta = normal * 2 * ray.direction().dot(normal);
        auto newRayDirection = ray.direction() - delta;
        newRayDirection = newRayDirection.normalize();

        auto newRay = Ray(newRayOrigin, newRayDirection);
        auto reflectionColor = shootRay(newRay, scene, depth + 1);

        color = reflectionColor * reflectionPercent.value() + color * (1 - reflectionPercent.value());
    }


    auto intersectionPosition = intersection->position();

    // Easy to debug intersection problems. Use with the last object in the scene (uncommented).
    //if (intersectionPosition.y() > 4.5f) {
    //    int breakhere = 3;
    //}

    // Shoot ray to the light
    auto lightPosition = scene.light();
    auto lightRayDirection = lightPosition - intersection->position();
    lightRayDirection = lightRayDirection.normalize();

    // Move the origin a little bit out of the object so it does not hit itself
    auto lightRayOrigin = intersection->position() + intersection->surfaceNormal() * 0.5f;
    auto rayToLight = Ray(lightRayOrigin, lightRayDirection);

    auto cosBetweenNormalAndLight = lightRayDirection.dot(intersection->surfaceNormal());
    cosBetweenNormalAndLight = std::max(0.2f, cosBetweenNormalAndLight);
    color = color * cosBetweenNormalAndLight;

    if (!scene.hitsLight(rayToLight)) {
        color = color * 0.8;
    }
    return color;
}

Color shootRayforPixel(int x, int y, const Scene &scene) {
    auto camera = scene.camera();
    // TODO: This hard codes the camera direction vector. Change.
    auto pointOnVirtualScreen = camera.origin() + Vec3(float(x), float(y), 500.0f);
    auto rayDirection = pointOnVirtualScreen - camera.origin();
    rayDirection = rayDirection.normalize();
    auto ray = Ray(camera.origin(), rayDirection);
    return shootRay(ray, scene, 0);
}

struct PixelWork {
public:
    int x = -1;
    int y = -1;
    Color pixelColor = {};
};

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
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    std::deque<std::future<PixelWork>> pixelFutures = {};

    // Shoot rays
    // TODO: Get orthogonal plane to direction vector?
    for (auto x = 0; x < WINDOW_WIDTH; x++) {
        for (auto y = 0; y < WINDOW_WIDTH; y++) {
            auto pixelColorLambda = [WINDOW_WIDTH, WINDOW_HEIGHT, &scene = std::as_const(scene)](int x, int y) {
                PixelWork work = {};
                work.x = x;
                work.y = y;

                auto moved_x = x - (WINDOW_WIDTH / 2);
                // Positive y is up in world space, but in screen (sdl) space its down
                auto moved_y = (WINDOW_WIDTH / 2) - y;

                const int NUM_SAMPLES = 1024;
                for (auto i = 0; i < NUM_SAMPLES; i++) {
                    work.pixelColor = work.pixelColor + shootRayforPixel(moved_x, moved_y, scene);
                }
                work.pixelColor = work.pixelColor / NUM_SAMPLES;
                // Make the whole scene brighter. TODO: Why is it so dark?
                work.pixelColor = work.pixelColor * 10.0f;
                work.pixelColor = work.pixelColor.clamp(0, 255);

                return work;
            };

            auto pixelColorWork = std::async(std::launch::async, pixelColorLambda, x, y);
            pixelFutures.push_back(std::move(pixelColorWork));
        }
    }

    while (1) {
        if (SDL_PollEvent(&event) &&
            event.type == SDL_QUIT)
            break;

        while (pixelFutures.size() > 0 &&
            utils::futureReady(pixelFutures.front())) {
            auto pixelFuture = std::move(pixelFutures.front());
            pixelFutures.pop_front();
            auto pixel = pixelFuture.get();
            auto pixelColor = pixel.pixelColor;

            SDL_SetRenderDrawColor(renderer,
                pixelColor.x(), pixelColor.y(), pixelColor.z(), 255);
            SDL_RenderDrawPoint(renderer, pixel.x, pixel.y);
        }

        SDL_Delay(10);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}