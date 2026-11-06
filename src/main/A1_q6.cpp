#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <memory>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// Assessment 01 Q6; see 题面.pdf.
namespace aim::q6 {
struct Target { int id; int x; int y; int hp; bool visible; };

// BEGIN IMPLEMENTATION

Target cmp(const Target& a, const Target& b) {
    long long distance_a = 1ll * a.x * a.x + 1ll * a.y * a.y;
    long long distance_b = 1ll * b.x * b.x + 1ll * b.y * b.y;
    if (a.hp < b.hp) {
        return a;
    }
    else if (a.hp > b.hp) {
        return b;
    }
    else if (distance_a < distance_b){
         return a;
    }
    else if (distance_a > distance_b) {
        return b;
    }
    else if (a.id < b.id) {
        return a;
    }
    else {
        return b;
    }
}

int select_target(const std::vector<Target>& targets, int max_range) {
    if (max_range < 0)
        return -1;
    Target result;
    result.id = -1;
    for(int i = 0; i < targets.size(); i++) {
        Target v = targets[i];
        // std::cerr<< "Target: id=" << v.id << ", x=" << v.x << ", y=" << v.y << ", hp=" << v.hp << ", visible=" << v.visible << "\n";
        if (!v.visible || v.hp <= 0) {
            continue;
        }
        long long distance = 1ll * v.x * v.x + 1ll * v.y * v.y;
        if (distance <= 1ll * max_range * max_range) {
            if (result.id == -1) {
                result = v;
            }
            else {
                result = cmp(result, v);
            }
        }
        // std::cerr<< "Current best target: id=" << result.id << ", x=" << result.x << ", y=" << result.y << ", hp=" << result.hp << ", visible=" << result.visible << "\n";
    }
    return result.id;
}
// END IMPLEMENTATION
}

#define CHECK(...) do { if (!(__VA_ARGS__)) throw std::runtime_error("check failed"); } while(false)
#define EXPECT_INVALID(...) do { bool caught=false; try { (void)(__VA_ARGS__); } catch(const std::invalid_argument&) { caught=true; } CHECK(caught); } while(false)
#define EXPECT_RANGE(...) do { bool caught=false; try { (void)(__VA_ARGS__); } catch(const std::out_of_range&) { caught=true; } CHECK(caught); } while(false)

using namespace aim::q6;
int main() {
    try {
        { CHECK(select_target({{7,3,4,20,true},{2,0,5,20,true}},5)==2); }
        { CHECK(select_target({},5)==-1); }
        std::cout << "PUBLIC 2/2\n";
    } catch(const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}
