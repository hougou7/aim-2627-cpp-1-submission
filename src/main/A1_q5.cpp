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

// Assessment 01 Q5; see 题面.pdf.
namespace aim::q5 {


// BEGIN IMPLEMENTATION
std::vector<std::vector<int>> risk(const std::vector<std::string>& grid) {
    if (grid.empty()) {
        return {};
    }

    if (grid[0].empty()) {
        throw std::invalid_argument("error");
    }

    for (int i = 0; i < grid.size(); i++) {
        std::string row = grid[i];
        if (row.size() != grid[0].size()) {
            throw std::invalid_argument("error");
        }

        for (int j = 0; j < row.size(); j++) {
            char cell = row[j];
            if (cell != '.' && cell != '#') {
                throw std::invalid_argument("error");
            }
        }
    }
    std::vector<std::vector<int>> result(grid.size(), std::vector<int>(grid[0].size(), 0));
    for (int i = 0; i < grid.size();i++) {
        for (int j = 0; j < grid[i].size();j++) {
            if (grid[i][j] == '#') {
                result[i][j] = -1;
            }
            else if (grid[i][j] == '.') {
                if(i > 0 && grid[i-1][j] == '#') {
                    result[i][j]++;
                }
                if(i < grid.size()-1 && grid[i+1][j] == '#') {
                    result[i][j]++;
                }
                if(j > 0 && grid[i][j-1] == '#') {
                    result[i][j]++;
                }
                if(j < grid[i].size()-1 && grid[i][j+1] == '#') {
                    result[i][j]++;
                }
            }
        }
    }
    return result;
}
// END IMPLEMENTATION
}

#define CHECK(...) do { if (!(__VA_ARGS__)) throw std::runtime_error("check failed"); } while(false)
#define EXPECT_INVALID(...) do { bool caught=false; try { (void)(__VA_ARGS__); } catch(const std::invalid_argument&) { caught=true; } CHECK(caught); } while(false)
#define EXPECT_RANGE(...) do { bool caught=false; try { (void)(__VA_ARGS__); } catch(const std::out_of_range&) { caught=true; } CHECK(caught); } while(false)

using namespace aim::q5;
int main() {
    try {
        { CHECK((risk({".#",".."})==std::vector<std::vector<int>>{{1,-1},{0,1}})); }
        { CHECK(risk({}).empty()); }
        std::cout << "PUBLIC 2/2\n";
    } catch(const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}
