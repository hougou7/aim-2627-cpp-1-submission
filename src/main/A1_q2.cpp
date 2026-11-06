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

// Assessment 01 Q2; see 题面.pdf.
namespace aim::q2 {
struct LabelEdit { std::string from; std::string to; };

// BEGIN IMPLEMENTATION
std::string add_to(std::string labels,std::string to){
    if(labels.empty())
        return to;
    return labels + "," + to;
}

std::string edit_labels(std::string labels, const std::vector<LabelEdit>& edits) {
    for(int i = 0; i < edits.size(); i++){
        std::string from = edits[i].from;
        std::string to = edits[i].to;
        if (from.empty() && !to.empty())
            labels = add_to(labels,to);
        else if (!from.empty()){
            int pos = labels.find(from);
            while (pos != std::string::npos) {
                int end = pos + from.length();
                bool leftOK = (pos == 0 || labels[pos - 1] == ',');
                bool rightOK = (end == labels.size() || labels[end] == ',');
                if (leftOK && rightOK) {
                    break;
                }
                pos = labels.find(from, pos + 1);
            }
            if (pos != std::string::npos){
                if(to.empty()){
                    if(pos == 0)
                        labels = labels.erase(pos, from.length()+1);
                    else
                        labels = labels.erase(pos-1, from.length()+1);
                }
                else {
                    labels.replace(pos, from.length(), to);
                }
            }
        }

    }
    return labels;
}
// END IMPLEMENTATION
}

#define CHECK(...) do { if (!(__VA_ARGS__)) throw std::runtime_error("check failed"); } while(false)
#define EXPECT_INVALID(...) do { bool caught=false; try { (void)(__VA_ARGS__); } catch(const std::invalid_argument&) { caught=true; } CHECK(caught); } while(false)
#define EXPECT_RANGE(...) do { bool caught=false; try { (void)(__VA_ARGS__); } catch(const std::out_of_range&) { caught=true; } CHECK(caught); } while(false)

using namespace aim::q2;
int main() {
    try {
        { CHECK(edit_labels("cam,imu,radar",{{"imu","gyro"},{"cam",""},{"","lidar"}})=="gyro,radar,lidar"); }
        { CHECK(edit_labels("cam,cam2,cam",{{"cam","camera"},{"cam",""}})=="camera,cam2"); }
        std::cout << "PUBLIC 2/2\n";
    } catch(const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}
