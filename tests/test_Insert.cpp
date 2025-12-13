#include "rtree.hpp"
#include <iostream>

int main() {
    RTree<int> tree;

    tree.Insert(Rect(1, 1), 1);
    tree.Insert(Rect(2, 2), 2);
    tree.Insert(Rect(3, 3), 3);
    tree.Insert(Rect(10, 10), 10);
    tree.Insert(Rect(9, 9), 9);
    tree.Insert(Rect(8, 8), 8);

    Rect q(0, 0, 5, 5);
    auto res = tree.Search(q);
    std::cout << "Query: ";
    std::cout << "(0, 0, 5, 5)" << std::endl;
    std::cout << "Found id: ";
    for (auto v : res) std::cout << v << " ";
    std::cout << std::endl;

    std::cout << "---------------" << std::endl;

    Rect q_point(1, 1);
    auto res_point = tree.Search(q_point);
    std::cout << "Query: ";
    std::cout << "(1, 1)" << std::endl;
    std::cout << "Found id: ";
    for (auto v : res_point) std::cout << v << " ";
    std::cout << std::endl;

    std::cout << "---------------" << std::endl;
    std::cout << "> Repeat insert id=1" << std::endl;

    tree.Insert(Rect(1, 1), 1);
    auto res_repeat = tree.Search(q);
    std::cout << "Query: ";
    std::cout << "(0, 0, 5, 5)" << std::endl;
    std::cout << "Found id: ";
    for (auto v : res_repeat) std::cout << v << " ";
    std::cout << std::endl;
}