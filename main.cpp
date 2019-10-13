#include <iostream>
#include <optional>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <vector>
#include <cstdio>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "scene.h"
#include "ray.h"
#include "vec3.h"


Color shootRay(const Ray &ray, const Scene &scene, int depth) {
    return Color();
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
            color = color * 0.8;
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