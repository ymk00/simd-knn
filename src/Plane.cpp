#include "Plane.h"
#include <algorithm>
#include <cstring>
#include <immintrin.h>

const size_t LANES = 8;

Plane::Plane() = default;

void Plane::add_point(const Point &point) { points.push_back(point); }
const std::vector<Point> &Plane::get_points() const { return points; }

std::vector<const Point *> Plane::knn(const Point &target, size_t k) const {
    size_t n = points.size();
    std::vector<std::pair<float, const Point *>> distances;
    distances.reserve(n);

    __m256 x0 = _mm256_set1_ps(target.x);
    __m256 y0 = _mm256_set1_ps(target.y);

    size_t i = 0;

    alignas(32) float dists[LANES];
    while (i + LANES <= n) {
        alignas(32) float x[LANES], y[LANES];
        for (size_t j = 0; j < LANES; ++j) {
            x[j] = points[i + j].x;
            y[j] = points[i + j].y;
        }

        __m256 xs = _mm256_load_ps(x);
        __m256 ys = _mm256_load_ps(y);

        __m256 dx = _mm256_sub_ps(xs, x0);
        __m256 dy = _mm256_sub_ps(ys, y0);

        __m256 dist;
#ifdef __FMA__
        dist = _mm256_fmadd_ps(dx, dx, _mm256_mul_ps(dy, dy));
#else
        dist = _mm256_add_ps(_mm256_mul_ps(dx, dx), _mm256_mul_ps(dy, dy));
#endif

        _mm256_store_ps(dists, dist);
        for (size_t j = 0; j < LANES; ++j) {
            distances.emplace_back(dists[j], &points[i + j]);
        }

        i += LANES;
    }

    // Stragglers
    for (; i < n; ++i) {
        float dx = points[i].x - target.x;
        float dy = points[i].y - target.y;
        float dist = dx * dx + dy * dy;
        distances.emplace_back(dist, &points[i]);
    }

    std::vector<const Point *> result;
    result.reserve(k);
    std::partial_sort(distances.begin(), distances.begin() + k, distances.end(),
                      [](const auto &a, const auto &b) { return a.first < b.first; });
    for (size_t j = 0; j < k; ++j) {
        result.push_back(distances[j].second);
    }
    return result;
}
