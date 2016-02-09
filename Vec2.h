#pragma once

#include <cmath>

struct Vec2 {

    float x, y;

    Vec2 round() {
        return {std::round(x), std::round(y)};
    }
    Vec2 floor() {
        return {std::floor(x), std::floor(y)};
    }

    friend Vec2 operator+(const Vec2 &a, const Vec2 &b) {
        return {a.x + b.x, a.y + b.y};
    }
    friend Vec2 operator-(const Vec2 &a, const Vec2 &b) {
        return {a.x - b.x, a.y - b.y};
    }

};


struct IVec2 {

    int x, y;

};
