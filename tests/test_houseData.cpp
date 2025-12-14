#include "rtree.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <random>

static bool parse3(const std::string& line, int& id, double& x, double& y) {
    size_t p1 = line.find(',');
    if (p1 == std::string::npos) return false;
    size_t p2 = line.find(',', p1 + 1);
    if (p2 == std::string::npos) return false;

    try {
        id = std::stoi(line.substr(0, p1));
        x  = std::stod(line.substr(p1 + 1, p2 - (p1 + 1)));
        y  = std::stod(line.substr(p2 + 1));
    } catch (...) {
        return false;
    }
    return true;
}

static std::vector<int> brute_force(const std::vector<std::pair<Rect,int>>& data, const Rect& q) {
    std::vector<int> out;
    for (auto& [r,id] : data) if (r.intersect(q)) out.push_back(id);
    return out;
}

int main(int argc, char** argv) {
    std::string path =
        (argc >= 2) ? argv[1] : "Dataset/processed_house.csv";

    std::ifstream fin(path);
    if (!fin) {
        std::cerr << "Cannot open: " << path << "\n";
        return 1;
    }

    std::string line;
    std::getline(fin, line); // skip header

    RTree<int, 30, 15> tree;
    std::vector<std::pair<Rect,int>> data;
    data.reserve(200000);

    double minx=1e300, maxx=-1e300, miny=1e300, maxy=-1e300;

    auto t0 = std::chrono::steady_clock::now();

    int id; double price, area;
    while (std::getline(fin, line)) {
        if (!parse3(line, id, price, area)) continue;

        Rect p(price, area); // point MBR
        tree.Insert(p, id);
        data.emplace_back(p, id);

        minx = std::min(minx, price); maxx = std::max(maxx, price);
        miny = std::min(miny, area ); maxy = std::max(maxy, area );
    }

    auto t1 = std::chrono::steady_clock::now();
    std::cout << "Loaded rows: " << data.size() << "\n";
    std::cout << "Tree height: " << tree.Height() << "\n";
    // std::cout << minx << ',' << miny << ',' << maxx << ',' << maxy << "\n";
    std::cout << "read file time + Insert time (ms): "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()
              << "\n";

    // set queries, and insure RTree vs Linear will use the same queries
    const int Q = 20000;
    std::mt19937_64 rng(123);
    std::uniform_real_distribution<double> ux(minx, maxx);
    std::uniform_real_distribution<double> uy(miny, maxy);

    std::vector<Rect> queries;
    queries.reserve(Q);

    // set the size of queries
    double ratio = 0.1;
    double sx = (maxx - minx) * ratio;
    double sy = (maxy - miny) * ratio;

    for (int i = 0; i < Q; ++i) {
        double cx = ux(rng), cy = uy(rng);
        queries.emplace_back(cx - sx, cy - sy, cx + sx, cy + sy);
    }

    long long sumRTree = 0, sumLinear = 0;

    auto q0 = std::chrono::steady_clock::now();
    for (auto& q : queries) {
        auto r = tree.Search(q);
        sumRTree += (long long)r.size();
    }
    auto q1 = std::chrono::steady_clock::now();

    auto l0 = std::chrono::steady_clock::now();
    for (auto& q : queries) {
        auto r = brute_force(data, q);
        sumLinear += (long long)r.size();
    }
    auto l1 = std::chrono::steady_clock::now();

    auto rt_ms = std::chrono::duration_cast<std::chrono::milliseconds>(q1 - q0).count();
    auto ln_ms = std::chrono::duration_cast<std::chrono::milliseconds>(l1 - l0).count();

    std::cout << "RTree query time (ms): " << rt_ms << ", total hits=" << sumRTree << "\n";
    std::cout << "Linear query time (ms): " << ln_ms << ", total hits=" << sumLinear << "\n";

    if (sumRTree != sumLinear) {
        std::cerr << "WARNING: total hits mismatch -> Search correctness issue.\n";
        return 2;
    }

    return 0;
}
