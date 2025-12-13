#pragma once
#include <vector>
#include <limits>
#include <algorithm>
#include <cmath>
#include <iostream>


// ======= Rectangles def. =======
struct Rect {
    double xmin, ymin, xmax, ymax;  // input should be min <= max

    Rect() : xmin(0),ymin(0),xmax(0),ymax(0) {}
    Rect(double x0,double y0,double x1,double y1)       // rectangle
        : xmin(std::min(x0,x1)), 
          ymin(std::min(y0,y1)),
          xmax(std::max(x0,x1)), 
          ymax(std::max(y0,y1)) {}
    Rect(double x, double y) : Rect(x, y, x, y) {}      // point
    // Api: create a point rectangle
    // static Rect RectPoint(double x, double y) {
    //     return Rect(x, y, x, y);
    // }

    double width() const { return xmax - xmin; }
    double height() const { return ymax - ymin; }
    double area() const {return width() * height(); }

    bool intersect(const Rect& b) const {
        // True:  they intersect
        // False: they don't
        return !(xmax < b.xmin || b.xmax < xmin || ymax < b.ymin || b.ymax < ymin);
    }

    bool contain(const Rect& b) const {
        return xmin <= b.xmin && xmax >= b.xmax && ymin <= b.ymin && ymax >= b.ymax;
    }

    static Rect combine(const Rect& a, const Rect& b) {
        return Rect(
            std::min(a.xmin, b.xmin),
            std::min(a.ymin, b.ymin),
            std::max(a.xmax, b.xmax),
            std::max(a.ymax, b.ymax)
        );
    }
    double enlarge(const Rect& newRect) const {
        Rect c = Rect::combine(*this, newRect);
        return c.area() - this->area();
    }
};

// ======= Node def. =======
template <typename T> struct Node;

template <typename T>
struct Entry {
    Rect mbr;                   // Minimum Bounding Rectangle
    Node<T>* child;             // internal node: child pointer
    T value;                    // leaf node: data
};

template <typename T>
struct Node {
    bool leaf;                  // true: leaf
    std::vector<Entry<T>> entries;

    Node(bool isLeaf = true) : leaf(isLeaf) {}

    Rect createNodeMBR() const {
        if (entries.empty()) return Rect(0, 0);
        Rect r = entries[0].mbr;
        for (std::size_t i = 1; i < entries.size(); ++i){
            r = Rect::combine(r, entries[i].mbr);
        }
        return r;
    }

    ~Node() {
        if (!leaf) {
            for (auto& e : entries) {
                delete e.child;
            }
        }
    }
};

// ======= RTree structure def. =======
template <typename T, int MAX_ENTRIES = 8, int MIN_ENTRIES = 3>

class RTree {
public:
    RTree() : root(nullptr) {}
    ~RTree() { delete root; }

    void Insert(const Rect& r, const T& value) {
        if (!root) {
            // no node, insert a node as the root
            root = new Node<T>(true);
            Entry<T> e;
            e.mbr = r;
            e.child = nullptr;
            e.value = value;
            root -> entries.push_back(e);
            return;
        }

        // the root exists, try to append MBR into the root
        Node<T>* newSibling = nullptr;
        Rect siblingMBR;
        insertInternal(root, r, value, newSibling, siblingMBR);

        // the content of the root has reached Max_ENTRIES
        if (newSibling) {
            // create a new root
            Node<T>* newRoot = new Node<T>(false);
            Entry<T> e1, e2;
            e1.mbr = root->createNodeMBR();
            e1.child = root;

            e2.mbr = siblingMBR;
            e2.child = newSibling;

            newRoot->entries.push_back(e1);
            newRoot->entries.push_back(e2);
            root = newRoot;
        }
    }

    bool Delete(std::size_t data_id);

    std::vector<T> Search(const Rect& query) const {
        std::vector<T> result;
        if (!root) return result;
        searchInternal(root, query, result);
        return result;
    }

private:
    Node<T>* root;

    void insertInternal(Node<T>* node, const Rect& r, const T& value, Node<T>* newSibling, Rect& newSiblingMBR) {
        newSibling = nullptr;

        // tracing node is a leaf
        if (node->leaf){
            Entry<T> e;
            e.mbr = r;
            e.child = nullptr;
            e.value = value;
            node->entries.push_back(e);

            // split the node if it reaches the MAX_ENTRIES
            if ((int)node->entries.size() > MAX_ENTRIES) {
                splitNode(node, newSibling, newSiblingMBR);
            }
        } 

        // tracing node is an internal node
        else {
            // choose a better child to go down
            int idx = chooseSubtree(node, r);
            Entry<T>& chosen = node->entries[idx];
            Node<T>* child = chosen.child;

            Node<T>* childSibling = nullptr;
            Rect childSiblingMBR;
            insertInternal(child, r, value, childSibling, childSiblingMBR);

            // update MBR of child
            chosen.mbr = child->createNodeMBR();

            // keep the child if split happens in the child
            if (childSibling) {
                Entry<T> ce;
                ce.mbr = childSiblingMBR;
                ce.child = childSibling;
                node->entries.push_back(ce);
            }

            // split if "keep the child" make it exceed MAX_ENTRIES
            if ((int)node->entries.size() > MAX_ENTRIES) {
                splitNode(node, newSibling, newSiblingMBR);
            }
        }
    }

    // based on the smallest area
    int chooseSubtree(Node<T>* node, const Rect& r) {
        int best_idx = 0;
        double best_enlarge = std::numeric_limits<double>::infinity();
        double best_area = std::numeric_limits<double>::infinity();

        for (int i = 0; i < (int)node->entries.size(); ++i) {
            Rect& m = node->entries[i].mbr;
            double enlarge = m.enlarge(r);
            double area = m.area();
            if (enlarge < best_enlarge || (enlarge == best_enlarge && area < best_area)) {
                best_enlarge = enlarge;
                best_area = area;
                best_idx = i;
            }
        }
        return best_idx;
    }

    void splitNode(Node<T>* node, Node<T>*& outSibling, Rect& outSiblingMBR) {
        outSibling = new Node<T>(node->leaf);

        // pick seeds
        int total = (int)node->entries.size();
        std::vector<bool> taken(total, false);
        int seedA = 0;
        int seedB = 1;
        pickSeeds(node->entries, seedA, seedB);

        // build split groups
        std::vector<Entry<T>> groupA;
        std::vector<Entry<T>> groupB;
        groupA.reserve(MAX_ENTRIES + 1);
        groupB.reserve(MAX_ENTRIES + 1);
        groupA.push_back(std::move(node->entries[seedA]));
        groupB.push_back(std::move(node->entries[seedB]));
        taken[seedA] = taken[seedB] = true;

        Rect mbrA = groupA[0].mbr;
        Rect mbrB = groupB[0].mbr;
        int countA = 1, countB = 1;
        int remain = total - 2;

        // 
        while (remain > 0) {
            // lucky case: matching MIN_ENTRIES
            if (countA + remain == MIN_ENTRIES) {
                for (int i = 0; i < total; ++i) {
                    if (!taken[i]) {
                        groupA.push_back(std::move(node->entries[i]));
                        mbrA = Rect::combine(mbrA, groupA.back().mbr);
                        taken[i] = true;
                        ++countA;
                        --remain;
                    }
                }
                break;
            }
            if (countB + remain == MIN_ENTRIES) {
                for (int i = 0; i < total; ++i) {
                    if (!taken[i]) {
                        groupB.push_back(std::move(node->entries[i]));
                        mbrB = Rect::combine(mbrB, groupB.back().mbr);
                        taken[i] = true;
                        ++countB;
                        --remain;
                    }
                }
                break;
            }

            // original case: quadratic picknext
            int next = pickNext(node->entries, taken, mbrA, mbrB);
            taken[next] = true;
            --remain;

            double enlargeA = mbrA.enlarge(node->entries[next].mbr);
            double enlargeB = mbrB.enlarge(node->entries[next].mbr);

            if (enlargeA < enlargeB ||
                (enlargeA == enlargeB && mbrA.area() < mbrB.area())) {
                // to groupA
                groupA.push_back(std::move(node->entries[next]));
                mbrA = Rect::combine(mbrA, groupA.back().mbr);
                ++countA;
            } else {
                // to groupB
                groupB.push_back(std::move(node->entries[next]));
                mbrB = Rect::combine(mbrB, groupB.back().mbr);
                ++countB;
            }
        }

        // to node and sibling
        node->entries.clear();
        node->entries.swap(groupA); 
        outSibling->entries.swap(groupB);
        outSiblingMBR = mbrB;
    }

    void pickSeeds(const std::vector<Entry<T>>& entries, int& seedA, int& seedB) {
        const int n = static_cast<int>(entries.size());
        double worstWaste = -1.0;
        seedA = 0;
        seedB = 1;

        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                Rect c = Rect::combine(entries[i].mbr, entries[j].mbr);
                double d = c.area() - entries[i].mbr.area() - entries[j].mbr.area();
                if (d > worstWaste) {
                    worstWaste = d;
                    seedA = i;
                    seedB = j;
                }
            }
        }
    }

    int pickNext(const std::vector<Entry<T>>& entries, const std::vector<bool>& taken, const Rect& mbrA, const Rect& mbrB) {
        const int n = static_cast<int>(entries.size());
        int bestIdx = -1;
        double bestDiff = -1.0;

        for (int i = 0; i < n; ++i) {
            if (taken[i]) continue;
            double enlargeA = mbrA.enlarge(entries[i].mbr);
            double enlargeB = mbrB.enlarge(entries[i].mbr);
            double diff = std::abs(enlargeA - enlargeB);
            if (diff > bestDiff) {
                bestDiff = diff;
                bestIdx = i;
            }
        }
        return bestIdx;
    }

    void searchInternal(Node<T>* node, const Rect& query, std::vector<T>& out) const {
        if (node->leaf) {
            for (auto& e : node->entries) {
                if (e.mbr.intersect(query)) {
                    out.push_back(e.value);
                }
            }
        } else {
            for (auto& e : node->entries) {
                if (e.mbr.intersect(query)) {
                    searchInternal(e.child, query, out);
                }
            }
        }
    }
};
