#pragma once

enum class Classification { ORANGE, BLUE };

struct Point {
    float x;
    float y;
    Classification classification;

    Point(float x, float y, Classification classification)
        : x(x), y(y), classification(classification) {}
};
