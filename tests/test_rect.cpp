#include "rtree.hpp"
#include <cstdlib>
#include <iostream>
using namespace rtree;

static int fails = 0;
#define CHECK(cond) do{ if(!(cond)){ std::cerr<<"FAIL @line "<<__LINE__<<"\n"; ++fails; }}while(0)

int main(){
    Rect a{0,0, 2,2};
    Rect b{1,1, 3,3};
    Rect c{3.1,3.1, 4,4};
    Rect d{2,0, 3,1}; // 和 a 在 x=2 貼邊

    CHECK(a.intersects(b));      // 重疊
    CHECK(!a.intersects(c));     // 分離
    CHECK(a.intersects(d));      // 邊貼邊視為相交
    CHECK(a.area() == 4.0);

    return fails ? EXIT_FAILURE : EXIT_SUCCESS;
}
