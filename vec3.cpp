#include "vec3.h"

Vec3 vectorutils::randomVector(float low, float high) {
    return Vec3(
        utils::randomFloat(low, high),
        utils::randomFloat(low, high),
        utils::randomFloat(low, high)
    );
}

Vec3 vectorutils::createRandomVectorInHemisphere(Vec3 other) {
    other = other.normalize();
    auto randomVec = randomVector(-1.0f, 1.0f).normalize();

    // Check if the two vectors are > 90 degree apart
    if (other.dot(randomVec) < 0) {
        randomVec = randomVec * -1.0f;
    }

    assert(other.dot(randomVec) > 0);

    return randomVec;
}

Color colorutils::multiplyColors(Color a, Color b) {
    return Color(
        a.x() * b.x(),
        a.y() * b.y(),
        a.z() * b.z()
    ) / 255;
}
