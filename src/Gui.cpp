#include "Gui.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <stdexcept>

Gui::Gui(int width, int height)
    : width(width), height(height), display(nullptr), clicked_point(nullptr),
      classification(Classification::ORANGE) {
    display = XOpenDisplay(nullptr);
    if (!display) {
        throw std::runtime_error("Failed to open X11 display");
    }

    int screen = DefaultScreen(display);

    window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, width, height, 1,
                                 BlackPixel(display, screen), WhitePixel(display, screen));

    // https://stackoverflow.com/questions/5557933/disable-resize-of-an-x11-window
    XSizeHints *size_hints = XAllocSizeHints();
    if (size_hints) {
        size_hints->flags = PMinSize | PMaxSize;
        size_hints->min_width = width;
        size_hints->max_width = width;
        size_hints->min_height = height;
        size_hints->max_height = height;

        XSetWMNormalHints(display, window, size_hints);
        XFree(size_hints);
    }

    XSelectInput(display, window, ExposureMask | ButtonPressMask);

    XMapWindow(display, window);

    gc = XCreateGC(display, window, 0, nullptr);
}

Gui::~Gui() {
    if (display) {
        XFreeGC(display, gc);
        XDestroyWindow(display, window);
        XCloseDisplay(display);
    }
}

void Gui::run(Plane &plane, size_t k) {
    while (true) {
        XEvent event;
        while (XPending(display)) {
            XNextEvent(display, &event);

            switch (event.type) {
            case Expose:
                render(plane, clicked_point, nearest_neighbors);
                break;

            case ButtonPress:
                if (event.xbutton.button == Button1) {
                    handle_mouse_click(plane, event.xbutton.x, event.xbutton.y, k);
                    render(plane, clicked_point, nearest_neighbors);
                }
                break;

            case ClientMessage:
                break;

            default:
                break;
            }
        }
    }
}

void Gui::render(const Plane &plane, const Point *target,
                 const std::vector<const Point *> &neighbors) {
    XClearWindow(display, window);

    for (const auto &point : plane.get_points()) {
        unsigned long color =
            (point.classification == Classification::ORANGE) ? 0xFFA500 : 0x0000FF;
        XSetForeground(display, gc, color);
        XDrawArc(display, window, gc, point.x - 2, point.y - 2, 4, 4, 0, 360 * 64);
    }

    if (target) {
        int y_offset = 70;
        int oranges = 0, blues = 0;

        for (const auto *neighbor : neighbors) {
            bool is_orange = neighbor->classification == Classification::ORANGE;
            unsigned long color = is_orange ? 0xFFA500 : 0x0000FF;

            XSetForeground(display, gc, color);
            XFillArc(display, window, gc, neighbor->x - 2, neighbor->y - 2, 4, 4, 0, 360 * 64);

            if (is_orange) {
                oranges++;
            } else {
                blues++;
            }

            char neighbor_info[100];
            snprintf(neighbor_info, sizeof(neighbor_info), "Neighbor: (%.1f, %.1f) %s", neighbor->x,
                     neighbor->y, is_orange ? "ORANGE" : "BLUE");

            XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
            XDrawString(display, window, gc, 580, y_offset, neighbor_info, strlen(neighbor_info));
            y_offset += 15;
        }

        // Who needs .c_str?
        char total_info[100];
        snprintf(total_info, sizeof(total_info), "Total: ORANGE=%d, BLUE=%d", oranges, blues);
        XDrawString(display, window, gc, 580, y_offset, total_info, strlen(total_info));

        char classification_text[100];
        snprintf(classification_text, sizeof(classification_text), "Classification: %s",
                 (classification == Classification::ORANGE) ? "ORANGE" : "BLUE");

        XDrawString(display, window, gc, 580, 50, classification_text, strlen(classification_text));

        // Cursor
        XSetForeground(display, gc, 0x000000);
        XDrawArc(display, window, gc, target->x - 4, target->y - 4, 8, 8, 0, 360 * 64);
        XFillArc(display, window, gc, target->x - 4, target->y - 4, 8, 8, 0, 360 * 64);
    }
}

void Gui::handle_mouse_click(const Plane &plane, int x, int y, size_t k) {
    clicked_point = new Point(static_cast<float>(x), static_cast<float>(y), Classification::ORANGE);
    nearest_neighbors = plane.knn(*clicked_point, k);
    classification = classify_point(nearest_neighbors);
}

Classification Gui::classify_point(const std::vector<const Point *> &neighbors) {
    std::map<Classification, int> class_count;
    for (const auto *neighbor : neighbors) {
        class_count[neighbor->classification]++;
    }
    return (class_count[Classification::ORANGE] > class_count[Classification::BLUE])
               ? Classification::ORANGE
               : Classification::BLUE;
}
