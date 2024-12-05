#include "Gui.h"
#include "Plane.h"
#include <iostream>
#include <random>

int main() {
    Plane plane;

    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(35.0f, 565.0f);
    std::uniform_int_distribution<int> class_dist(0, 1);

    for (size_t i = 0; i < 1000; ++i) {
        Classification classification =
            (class_dist(rng) == 0) ? Classification::ORANGE : Classification::BLUE;
        plane.add_point({dist(rng), dist(rng), classification});
    }

    try {
        Gui gui(800, 600);
        size_t k = 20;
        gui.run(plane, k);
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
