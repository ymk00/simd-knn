#pragma once

#include "Point.h"
#include <cstddef>
#include <vector>

class Plane {
  private:
    std::vector<Point> points;

  public:
    Plane();
    void add_point(const Point &point);
    std::vector<const Point *> knn(const Point &target, size_t k) const;
    const std::vector<Point> &get_points() const;
};
