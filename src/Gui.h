#pragma once

#include "Plane.h"
#include <X11/Xlib.h>
#include <vector>

class Gui {
  public:
    Gui(int width, int height);
    ~Gui();
    void run(Plane &plane, size_t k);

  private:
    void render(const Plane &plane, const Point *target,
                const std::vector<const Point *> &neighbors);
    void handle_mouse_click(const Plane &plane, int x, int y, size_t k);
    Classification classify_point(const std::vector<const Point *> &neighbors);

    int width, height;
    Display *display;
    Window window;
    GC gc;
    Point *clicked_point;
    std::vector<const Point *> nearest_neighbors;
    Classification classification;
};
