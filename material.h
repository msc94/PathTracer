#pragma once

#include <optional>

#include "vec3.h"

class Material {
    std::optional<Color> _emittingColor = {};
    Color _color = {};
    std::optional<float> _reflectionPercent = {};

public:
    Material() = default;
    Material(Color color)
        : _color(color) {}


    Material setEmittingColor(Color emittingColor) const {
        Material m = *this;
        m._emittingColor = emittingColor;
        return m;
    }

    Material setReflectingPercent(float percent) const {
        Material m = *this;
        m._reflectionPercent = percent;
        return m;
    }

    Color color() const { return _color; }
    std::optional<Color> emittingColor() const { return _emittingColor; }
    std::optional<float> reflectionPercent() const { return _reflectionPercent; }

    static Material black() { return Material(Color(0, 0, 0)); }
    static Material white() { return Material(Color(255, 255, 255)); }
    static Material red() { return Material(Color(255, 0, 0)); }
    static Material green() { return Material(Color(0, 255, 0)); }
    static Material blue() { return Material(Color(0, 0, 255)); }
    static Material pink() { return Material(Color(255, 192, 203)); }
    static Material gray() { return Material(Color(211, 211, 211)); }
}; 
