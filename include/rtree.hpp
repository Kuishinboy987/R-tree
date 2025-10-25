#pragma once
#include <algorithm>

namespace rtree {

struct Rect {
    double xmin, ymin, xmax, ymax; // 保證 xmin<=xmax, ymin<=ymax
    static Rect from_point(double x, double y) { return {x,y,x,y}; }

    double area() const {
        double w = std::max(0.0, xmax - xmin);
        double h = std::max(0.0, ymax - ymin);
        return w*h;
    }
    // 邊貼邊也算相交（常見做法）
    bool intersects(const Rect& b) const {
        return !(xmax < b.xmin || b.xmax < xmin || ymax < b.ymin || b.ymax < ymin);
    }
};
} // namespace rtree
