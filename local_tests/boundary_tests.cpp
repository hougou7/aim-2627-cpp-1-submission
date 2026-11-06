#define main q1_embedded_main
#include "../src/main/A1_q1.cpp"
#undef main
#define main q2_embedded_main
#include "../src/main/A1_q2.cpp"
#undef main
#define main q3_embedded_main
#include "../src/main/A1_q3.cpp"
#undef main
#define main q4_embedded_main
#include "../src/main/A1_q4.cpp"
#undef main
#define main q5_embedded_main
#include "../src/main/A1_q5.cpp"
#undef main
#define main q6_embedded_main
#include "../src/main/A1_q6.cpp"
#undef main
#define main q7_embedded_main
#include "../src/main/A1_q7.cpp"
#undef main
#define main q8_embedded_main
#include "../src/main/A1_q8.cpp"
#undef main

void expect(bool condition, const char* name) {
  if (!condition) {
    throw std::runtime_error(name);
  }
}

void test_normalization_boundaries() {
  expect(aim::q1::ratio(-5, 100) == 0, "Q1 negative hp");
  expect(aim::q1::ratio(101, 100) == 100, "Q1 hp above maximum");
  expect(aim::q1::ratio(1, 3) == 33, "Q1 integer truncation");
  expect(aim::q1::ratio(1000000000000LL, 1000000000000LL) == 100,
         "Q1 wide multiplication");
  expect(aim::q1::ratio(5, 0) == -1, "Q1 invalid maximum");
  expect(aim::q1::report(0, 4, 0, -7) ==
             "ID=0;HP=INVALID;BAT=0;STATE=LOW",
         "Q1 invalid hp and negative battery");
  expect(aim::q1::report(1, 1, 2, 20) ==
             "ID=1;HP=50;BAT=20;STATE=WARNING",
         "Q1 warning threshold");
  expect(aim::q1::report(1, 1, 2, 60) == "ID=1;HP=50;BAT=60;STATE=OK",
         "Q1 ok threshold");
}

void test_complete_labels_and_sequential_edits() {
  expect(aim::q2::edit_labels("cam2,cam", {{"cam", "imu"}}) == "cam2,imu",
         "Q2 skip partial match");
  expect(aim::q2::edit_labels("a,b,a", {{"a", "x"}}) == "x,b,a",
         "Q2 first complete match only");
  expect(aim::q2::edit_labels("a,b,c", {{"a", ""}}) == "b,c",
         "Q2 remove first label");
  expect(aim::q2::edit_labels("a,b,c", {{"b", ""}}) == "a,c",
         "Q2 remove middle label");
  expect(aim::q2::edit_labels("a,b,c", {{"c", ""}}) == "a,b",
         "Q2 remove last label");
  expect(aim::q2::edit_labels("a", {{"a", ""}, {"", "b"}}) == "b",
         "Q2 clear then append");
  expect(aim::q2::edit_labels("", {{"", "a"}, {"a", "long_name"},
                                  {"long_name", "z"}}) == "z",
         "Q2 find positions after length changes");
  expect(aim::q2::edit_labels("a", {{"A", "x"}, {"", ""}}) == "a",
         "Q2 case sensitivity and empty edit");
}

void test_window_bounds_and_wide_sums() {
  expect(aim::q3::alarms({}, 1, 0).empty(), "Q3 empty input");
  expect(aim::q3::alarms({1, 2, 3}, 0, 0).empty(), "Q3 zero window");
  expect(aim::q3::alarms({1, 2, 3}, 4, 0).empty(), "Q3 oversized window");
  expect(aim::q3::alarms({1, 2, 3}, 1, 1) == std::vector<int>({1, 2}),
         "Q3 single element windows");
  expect(aim::q3::alarms({1, 2, 3}, 3, 6).empty(), "Q3 strict threshold");
  expect(aim::q3::alarms({-5, -2, -3}, 2, -6) == std::vector<int>({2}),
         "Q3 negative windows");
  expect(aim::q3::alarms({1000000000, 1000000000, 1000000000}, 3,
                         2147483647LL) == std::vector<int>({2}),
         "Q3 sum exceeds int range");
}

void test_command_rejection_preserves_state() {
  auto result = aim::q4::execute({"SET 5", "RESET x", "ADD 12x", "ADD 1.0",
                                  "ADD 9999999999999999999999999", "add 5",
                                  "", "ADD +2"});
  expect(result.value == 7 && result.accepted == 2 && result.rejected == 6,
         "Q4 invalid commands preserve state");
  result = aim::q4::execute({"SET 1000000", "ADD 1", "RESET",
                             "ADD -1000000", "ADD -1"});
  expect(result.value == -1000000 && result.accepted == 3 &&
             result.rejected == 2,
         "Q4 result range boundaries");
  result = aim::q4::execute({"SET 1", "ADD 9223372036854775807"});
  expect(result.value == 1 && result.accepted == 1 && result.rejected == 1,
         "Q4 validate operand before addition");
  result = aim::q4::execute({" \tADD +2 \r", "RESET \t"});
  expect(result.value == 0 && result.accepted == 2 && result.rejected == 0,
         "Q4 surrounding whitespace");
  result = aim::q4::execute({});
  expect(result.value == 0 && result.accepted == 0 && result.rejected == 0,
         "Q4 independent empty call");
}

void test_grid_validation_and_edge_neighbors() {
  expect(aim::q5::risk({}).empty(), "Q5 empty grid");
  for (const auto& grid : std::vector<std::vector<std::string>>{
           {""}, {"..", "."}, {"..", ".x"}}) {
    bool rejected = false;
    try {
      aim::q5::risk(grid);
    } catch (const std::invalid_argument&) {
      rejected = true;
    }
    expect(rejected, "Q5 invalid grid throws invalid_argument");
  }
  expect(aim::q5::risk({"."}) == std::vector<std::vector<int>>({{0}}),
         "Q5 single open cell");
  expect(aim::q5::risk({"#"}) == std::vector<std::vector<int>>({{-1}}),
         "Q5 single obstacle");
  expect(aim::q5::risk({"#.#"}) == std::vector<std::vector<int>>({{-1, 2, -1}}),
         "Q5 single row neighbors");
  expect(aim::q5::risk({"#", ".", "#"}) ==
             std::vector<std::vector<int>>({{-1}, {2}, {-1}}),
         "Q5 single column neighbors");
}

void test_target_filtering_and_priority() {
  expect(aim::q6::select_target({}, 0) == -1, "Q6 empty input");
  expect(aim::q6::select_target({{1, 0, 0, 10, true}}, -1) == -1,
         "Q6 negative range");
  expect(aim::q6::select_target({{1, 0, 0, 0, true}, {2, 0, 0, 1, false}}, 0) ==
             -1,
         "Q6 dead and invisible targets");
  expect(aim::q6::select_target({{10, 3, 4, 1, true}, {2, 0, 0, 2, true}}, 5) ==
             10,
         "Q6 hp precedes distance");
  expect(aim::q6::select_target({{1, 5, 0, 10, true}, {2, 1, 0, 10, true}}, 5) ==
             2,
         "Q6 distance precedes id");
  expect(aim::q6::select_target({{10, 3, 4, 1, true}, {2, -3, -4, 1, true}}, 5) ==
             2,
         "Q6 equal distance uses id");
  expect(aim::q6::select_target({{1, 1, 1, 10, true}}, 1) == -1,
         "Q6 distance is not truncated");
  expect(aim::q6::select_target({{1, 0, 65536, 10, true},
                                 {2, 0, 1, 10, true}}, 1000000) == 2,
         "Q6 y multiplication uses wide integers");
  expect(aim::q6::select_target({{7, 1000000, 0, 1, true},
                                 {8, 1000000, 1000000, 1, true}}, 1000000) == 7,
         "Q6 maximum range boundary");
}

void test_cooling_and_rejected_requests() {
  auto result = aim::q7::simulate(4, 80, 10, 30, {});
  expect(result.shots == 0 && result.rejected == 0 && result.ammo == 4 &&
             result.heat == 80,
         "Q7 empty input does not cool");
  result = aim::q7::simulate(0, 50, 10, 20, {1});
  expect(result.shots == 0 && result.rejected == 1 && result.ammo == 0 &&
             result.heat == 40,
         "Q7 cool before rejecting empty magazine");
  result = aim::q7::simulate(1, 100, 0, 0, {1});
  expect(result.shots == 1 && result.rejected == 0 && result.ammo == 0 &&
             result.heat == 100,
         "Q7 heat equal to limit can fire");
  result = aim::q7::simulate(1, 100, 0, 1, {1});
  expect(result.shots == 0 && result.rejected == 1 && result.ammo == 1 &&
             result.heat == 100,
         "Q7 rejected shot preserves ammo and heat");
  result = aim::q7::simulate(0, 50, 100, 0, {0});
  expect(result.rejected == 0 && result.heat == 0, "Q7 idle request only cools");
}

void test_telemetry_classification_and_independent_calls() {
  const auto result = aim::q8::summarize({{1, -1001}, {1, 30}, {1, 1001},
                                          {1, 40}, {9999, 1000}, {0, -1000},
                                          {10000, 1}, {-1, 1}});
  expect(result.valid == 3 && result.duplicate == 1 && result.invalid == 4 &&
             result.total_hp == 130,
         "Q8 validation precedes duplicate detection");
  expect(result.ids == std::vector<int>({0, 1, 9999}), "Q8 ids are sorted");
  expect(result.valid + result.duplicate + result.invalid == 8 &&
             result.valid == static_cast<int>(result.ids.size()),
         "Q8 classification conservation");
  const auto empty = aim::q8::summarize({});
  expect(empty.valid == 0 && empty.duplicate == 0 && empty.invalid == 0 &&
             empty.total_hp == 0 && empty.ids.empty(),
         "Q8 independent empty call");
  const auto next = aim::q8::summarize({{1, 10}});
  expect(next.valid == 1 && next.duplicate == 0 && next.total_hp == 10,
         "Q8 seen ids are local to each call");
}

int main() {
  try {
    test_normalization_boundaries();
    test_complete_labels_and_sequential_edits();
    test_window_bounds_and_wide_sums();
    test_command_rejection_preserves_state();
    test_grid_validation_and_edge_neighbors();
    test_target_filtering_and_priority();
    test_cooling_and_rejected_requests();
    test_telemetry_classification_and_independent_calls();
    std::cout << "BOUNDARY 8/8\n";
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
  }
  return 0;
}
