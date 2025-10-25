#pragma once
#include <cstddef>
#include <variant>
#include <optional>
#include <vector>

namespace rtree {

// ======= Rectangles def. =======
struct Rect {
    double xmin, ymin, xmax, ymax;  // input should be min <= max

    Rect() : xmin(0),ymin(0),xmax(0),ymax(0) {}
    Rect(double x0,double y0,double x1,double y1)
        : xmin(std::min(x0,x1)), ymin(std::min(y0,y1)),
        xmax(std::max(x0,x1)), ymax(std::max(y0,y1)) {}

    double area() const;
    bool intersects(const Rect & b) const;
};

// ======= Node def. =======
struct DataRef { std::size_t id; };
struct ChildRef { std::size_t node_idx; };
using Data_Child_switch = std::variant<DataRef, ChildRef>;
struct Entry {
    Rect mbr;                   // Minimum Bounding Rectangle
    Data_Child_switch ref;      // child pointer will be either data or node
};

struct Node {
    bool leaf = true;           // Identify the type of node
    std::vector<Entry> entries; // cargos in a Node
    int parent = -1;            // a ref. of parent (for Adjust_tree)
};

// ======= RTree structure def. =======
struct Params {
    int Max_deg = 5;
    int min_deg = 3;
};

class RTree {
public:
    explicit RTree(Params p = {});

    std::size_t height() const;
    std::size_t node_count() const;

    // ------- future works 
    std::size_t Insert(const Rect & r);
    bool Delete(std::size_t data_id);
    std::vector<std::size_t> region_search(const Rect & qurey) const;
    // ------- futre works ^

private:
    Params params_;
    std::vector<Node> nodes_;
    std::vector<Rect> data_;

    // ------- future works 
    void Adjust_tree(int node);
    // ------- futre works ^
};
}
