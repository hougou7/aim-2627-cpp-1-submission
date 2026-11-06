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

// Assessment 01 Q8; see 题面.pdf.
namespace aim::q8 {
struct Telemetry { int id; int hp; };
struct Summary { int valid; int duplicate; int invalid; long long total_hp; std::vector<int> ids; };

// BEGIN IMPLEMENTATION
Summary summarize(const std::vector<Telemetry>& frames) {
    Summary result{0, 0, 0, 0, {}};
    // std::array<int, 10000> id_count{};
    int id_count[10000]{};
    for (int i = 0; i < frames.size(); i++) {
        const Telemetry& frame = frames[i];
        // std::cerr<<"Processing frame " << i << ": id=" << frame.id << ", hp=" << frame.hp << "\n";
        if (frame.hp < -1000 || frame.hp > 1000 || frame.id < 0 || frame.id >= 10000) {
            result.invalid++;
            continue;
        }
        if (id_count[frame.id] == 0) {
            result.valid++;
            if (frame.hp < 0)
                result.total_hp += 0;
            else if (frame.hp > 100)
                result.total_hp += 100;
            else
                result.total_hp += frame.hp;
            result.ids.push_back(frame.id);
            id_count[frame.id]++;
        }
        else {
            result.duplicate++;
        }
        // std::cerr<<"result.valid: " << result.valid << ", result.total_hp: " << result.total_hp << ", result.duplicate: " << result.duplicate <<", result.invalid: " << result.invalid << "\n";
    }
    sort(result.ids.begin(), result.ids.end());
    return result;
}
// END IMPLEMENTATION
}

#define CHECK(...) do { if (!(__VA_ARGS__)) throw std::runtime_error("check failed"); } while(false)
#define EXPECT_INVALID(...) do { bool caught=false; try { (void)(__VA_ARGS__); } catch(const std::invalid_argument&) { caught=true; } CHECK(caught); } while(false)
#define EXPECT_RANGE(...) do { bool caught=false; try { (void)(__VA_ARGS__); } catch(const std::out_of_range&) { caught=true; } CHECK(caught); } while(false)

using namespace aim::q8;
int main() {
    try {
        { aim::q8::Summary r=summarize({{2,200},{1,-5},{2,10},{-1,30}}); CHECK(r.valid==2 && r.duplicate==1 && r.invalid==1 && r.total_hp==100 && (r.ids==std::vector<int>{1,2})); }
        { aim::q8::Summary r=summarize({{1,-1001},{1,30},{1,1001}}); CHECK(r.valid==1 && r.duplicate==0 && r.invalid==2 && r.total_hp==30 && (r.ids==std::vector<int>{1})); }
        std::cout << "PUBLIC 2/2\n";
    } catch(const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}
