
#include <algorithm>
#include <cmath>
#include <exception>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>
#include "advent.hpp"

using battery_row_t = std::vector<int>;
using battery_map_t = std::vector<battery_row_t>;

battery_map_t read_file()
{
    std::ifstream infile("input.txt");
    battery_map_t batteries;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        batteries.emplace_back();
        char batt_char{0};

        while(ss >> batt_char) {
            batteries.back().push_back(advt::char_to_string(batt_char));
        }
    }

    return batteries;
}

int find_max_part1(const battery_row_t & row)
{
    //find first num
    //max from begin() to end() - 1
    auto view1 = row | std::views::take(row.size() - 1);
    auto result1 = std::ranges::max_element(view1);
    if (result1 == std::ranges::end(view1))
        throw std::runtime_error("Max not found!");
    auto result_distance = std::distance(row.begin(), result1);

    //find first num
    //max from result + 1 to end()
    auto view2 = row | std::views::drop(result_distance + 1);
    auto result2 = std::ranges::max_element(view2);
    if (result2 == std::ranges::end(view2))
        throw std::runtime_error("Max not found!");

    return *result1 * 10 + *result2;
}

long find_max_joltage(const battery_row_t & row, int num_batts)
{
    auto view1 = std::views::all(row);
    battery_row_t results;
    std::ptrdiff_t result_distance{ -1 };

    for (int i = 0; i < num_batts; i++) {
        auto view2 = view1 | std::views::take(std::ranges::size(view1) - (num_batts - (i + 1)) ) | std::views::drop(result_distance + 1);
        auto result = std::ranges::max_element(view2);
        if (result == std::ranges::end(view2))
            throw std::runtime_error("Max not found!");
        result_distance = std::distance(row.begin(), result);
        results.push_back(*result);
    }

    long out_result{0};
    long scale = std::pow(10, results.size() - 1);
    for (auto num : results) {
        out_result += num * scale;
        scale /= 10;
    }
    return out_result;
}

int main()
{
    const auto batteries = read_file();
    long sum_part1{0};
    long sum_part2{0};

    for (const auto & row : batteries)
        sum_part1 += find_max_joltage(row, 2);

    for (const auto & row : batteries)
        sum_part2 += find_max_joltage(row, 12);

    std::cout << sum_part1 << '\n';
    std::cout << sum_part2 << '\n';
}