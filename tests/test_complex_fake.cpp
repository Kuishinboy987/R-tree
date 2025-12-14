#include "rtree.hpp"
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <cassert>

static std::vector<int> brute_force(const std::vector<std::pair<Rect,int>>& data, const Rect& q) {
    std::vector<int> out;
    out.reserve(64);
    for (auto& [r,id] : data) {
        if (r.intersect(q)) out.push_back(id);
    }
    return out;
}

static Rect random_query(std::mt19937_64& rng, double minx, double maxx, double miny, double maxy) {
    std::uniform_real_distribution<double> ux(minx, maxx);
    std::uniform_real_distribution<double> uy(miny, maxy);

    // query size
    std::uniform_real_distribution<double> us(0.0, 1.0);
    double t = us(rng);
    double sx, sy;
    if (t < 0.70) { sx = 3.0;   sy = 3.0; }     // small
    else if (t < 0.95) { sx = 30.0; sy = 30.0; } // medium
    else { sx = 300.0; sy = 300.0; }             // large

    double cx = ux(rng), cy = uy(rng);
    return Rect(cx - sx, cy - sy, cx + sx, cy + sy);
}

int main() {
    using Id = int;
    RTree<Id, 8, 3> tree;

    std::mt19937_64 rng(1234567);

    const int N = 20000;
    const int Q = 3000;

    std::uniform_real_distribution<double> u(0.0, 10000.0);

    std::vector<std::pair<Rect,Id>> data;
    data.reserve(N);

    // 1. Insert
    for (int i = 0; i < N; ++i) {
        double x = u(rng);
        double y = u(rng);
        Rect r(x, y);          // point
        Id id = i;

        tree.Insert(r, id);
        data.emplace_back(r, id);

        // 2. Repeat Insert (should be banned)
        if (i % 2000 == 0) {
            tree.Insert(r, id); // duplicate attempt
            Rect q(x-0.0, y-0.0, x+0.0, y+0.0);
            auto a = tree.Search(q);
            auto b = brute_force(data, q);
            std::sort(a.begin(), a.end());
            std::sort(b.begin(), b.end());
            assert(a == b);
        }
    }

    // 3) Random queries: oracle compare
    for (int k = 0; k < Q; ++k) {
        Rect q = random_query(rng, 0.0, 10000.0, 0.0, 10000.0);

        auto a = tree.Search(q);
        auto b = brute_force(data, q);

        std::sort(a.begin(), a.end());
        std::sort(b.begin(), b.end());

        if (a != b) {
            std::cerr << "Mismatch at query " << k << "\n";
            std::cerr << "RTree size=" << a.size() << " brute size=" << b.size() << "\n";
            return 1;
        }
    }

    // 4. Random deletes + re-query
    std::shuffle(data.begin(), data.end(), rng);
    const int D = 5000;
    for (int i = 0; i < D; ++i) {
        auto [r, id] = data.back();
        data.pop_back();

        bool ok = tree.Delete(r, id);
        assert(ok);

        // sampling validation
        if (i % 200 == 0) {
            Rect q = random_query(rng, 0.0, 10000.0, 0.0, 10000.0);
            auto a = tree.Search(q);
            auto b = brute_force(data, q);
            std::sort(a.begin(), a.end());
            std::sort(b.begin(), b.end());
            assert(a == b);
        }
    }

    std::cout << "Random oracle test passed.\n";
    return 0;
}
