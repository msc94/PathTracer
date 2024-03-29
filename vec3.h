#pragma once

#include <cmath>
#include <cassert>

#include "utils.h"
#include "vec3_simd.h"

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

    VecType cross(const VecType &other) const {
        return VecType(
            _y * other.z() - _z * other.y(),
            _z * other.x() - _x * other.z(),
            _x * other.y() - _y * other.x()
        );
    }

    T length() const {
        return std::sqrt(
            std::pow(_x, 2) +
            std::pow(_y, 2) +
            std::pow(_z, 2)
        );
    }

    [[nodiscard]] VecType normalize() const {
        return *this / length();
    }

    VecType clamp(T min, T max) {
        return VecType(
            std::clamp(_x, min, max),
            std::clamp(_y, min, max),
            std::clamp(_z, min, max)
        );
    }
};

// using Vec3 = Vec3T<float>;
using Vec3 = SimdVector3;
using Color = Vec3T<int>;

namespace vectorutils {
    Vec3 randomVector(float low, float high);
    Vec3 createRandomVectorInHemisphere(Vec3 other);
}

namespace colorutils {
    Color multiplyColors(Color a, Color b);
}