#pragma once

#include "vec3.h"

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
