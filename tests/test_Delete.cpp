#include "rtree.hpp"
#include <iostream>

int main() {
    using Value = std::pair<double, int>;
    RTree<Value> tree;

    tree.Insert(Rect(1, 1), {1, 0});    // Value {payload, id}
    tree.Insert(Rect(2, 2), {2, 1});
    tree.Insert(Rect(3, 3), {3, 2});
    tree.Insert(Rect(10, 10), {4, 3});
    tree.Insert(Rect(9, 9), {9, 4});
    tree.Insert(Rect(8, 8), {8, 5});

    Rect q(0, 0, 5, 5);
    auto res = tree.Search(q);
    std::cout << "Found Value: ";
    for (auto v : res) std::cout << '(' << v.first << ',' << v.second << ')' << " ";
    std::cout << std::endl;

    std::cout << "--------------------" << std::endl;
    std::cout << "> delete id=1" << std::endl;

    Rect query = Rect(1, 1);
    Value target = {1, 0};
    bool check = tree.Delete(query, target);
    if (!check) std::cout << "no such entry, id:" << " " << target.second<< std::endl;
    if (check) std::cout << "delete success, id:" << " " << target.second << std::endl;
    auto res_del = tree.Search(q);
    std::cout << "Found Value: ";
    for (auto v : res_del) std::cout << '(' << v.first << ',' << v.second << ')' << " ";
    std::cout << std::endl;

    std::cout << "--------------------" << std::endl;
    std::cout << "> Repeat delete id=1" << std::endl;

    bool check_again = tree.Delete(query, target);
    if (!check_again) std::cout << "no such entry, id:" << " " << target.second<< std::endl;
    if (check_again) std::cout << "delete success, id:" << " " << target.second << std::endl;
    auto res_del_again = tree.Search(q);
    std::cout << "Found Value: ";
    for (auto v : res_del_again) std::cout << '(' << v.first << ',' << v.second << ')' << " ";
    std::cout << std::endl;
}