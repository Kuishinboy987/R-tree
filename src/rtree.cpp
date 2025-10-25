#include "rtree.hpp"
#include <algorithm>
#include <cassert>

namespace rtree {

// ======= Rectangles imp. =======
double Rect::area() const {
    const double w = std::max(0.0, xmax - xmin);
    const double h = std::max(0.0, ymax - ymin);
    return w * h;
}

bool Rect::intersects(const Rect& b) const {
    return !(xmax < b.xmin || b.xmax < xmin || ymax < b.ymin || b.ymax < ymin);
}

// ============ RTree imp. ============
RTree::RTree(Params p) : params_(p) {
nodes_.clear();
nodes_.push_back(Node{}); // root
}


} // namespace rtree