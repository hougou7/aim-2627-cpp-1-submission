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

// Assessment 01 Q4; see 题面.pdf.
namespace aim::q4 {
struct CommandResult { long long value; int accepted; int rejected; };

// BEGIN IMPLEMENTATION

bool check(long long x){
    return x >= -1000000 && x <= 1000000;
}

CommandResult execute(const std::vector<std::string>& lines) {
    CommandResult result{0,0,0};
    for (int i = 0; i < lines.size(); i++) {
        // std::cerr << "Executing line " << i << ": " << lines[i] << "\n";
        std::stringstream ss(lines[i]);
        std::string command;
        ss >> command;
        if (ss.fail()){
            result.rejected++;
            // std::cerr << "Invalid command format on line " << i << "\n";
            continue;
        }
        if (command == "ADD") {
            long long x;
            std::string var;
            ss >> x;
            if (ss.fail() || (ss >> var)) {
                result.rejected++;
                // std::cerr << "Invalid ADD command on line " << i << "\n";
                continue;
            }
            if (!check(x) || !check(result.value + x)) {
                result.rejected++;
                continue;
            }
            result.value += x;
            result.accepted++;
            // std::cerr<< "ADD " << x << " => value=" << result.value << "\n";
        }
        else if (command == "SET") {
            long long x;
            std::string var;
            ss >> x;
            if (ss.fail() || (ss >> var)) {
                result.rejected++;
                // std::cerr << "Invalid SET command on line " << i << "\n";
                continue;
            }
            if (!check(x)) {
                result.rejected++;
                continue;
            }
            result.value = x;
            result.accepted++;
            // std::cerr<< "SET " << x << " => value=" << result.value << "\n";
        }
        else if (command == "RESET") {
            std::string var;
            if (ss.fail() || (ss >> var)) {
                result.rejected++;
                // std::cerr << "Invalid RESET command on line " << i << "\n";
                continue;
            }
            result.value = 0;
            result.accepted++;
            // std::cerr<< "RESET => value=" << result.value << "\n";
        }
        else {
            result.rejected++;
            // std::cerr << "Unknown command on line " << i << "\n";
        }
    }
    return result;
}
// END IMPLEMENTATION
}

#define CHECK(...) do { if (!(__VA_ARGS__)) throw std::runtime_error("check failed"); } while(false)
#define EXPECT_INVALID(...) do { bool caught=false; try { (void)(__VA_ARGS__); } catch(const std::invalid_argument&) { caught=true; } CHECK(caught); } while(false)
#define EXPECT_RANGE(...) do { bool caught=false; try { (void)(__VA_ARGS__); } catch(const std::out_of_range&) { caught=true; } CHECK(caught); } while(false)

using namespace aim::q4;
int main() {
    try {
        { aim::q4::CommandResult r=execute({"ADD 5","SET 8 x","RESET","ADD -2"}); CHECK(r.value==-2 && r.accepted==3 && r.rejected==1); }
        { aim::q4::CommandResult r=execute({}); CHECK(r.value==0 && r.accepted==0 && r.rejected==0); }
        std::cout << "PUBLIC 2/2\n";
    } catch(const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}
