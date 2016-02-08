#pragma once

struct Vec2 {

    float x, y;

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
