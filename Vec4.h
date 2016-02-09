#pragma once

struct Vec4 {

    float x, y, x2, y2;

    friend Vec4 operator*(const Vec4 &a, float b) {
        return {a.x * b, a.y * b, a.x2 * b, a.y2 * b};
    }

};
