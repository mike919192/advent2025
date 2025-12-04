
#include <algorithm>
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

int find_max_joltage(const battery_row_t & row)
{
    //find first num
    //max from begin() to end() - 2
    auto view1 = row | std::views::take(row.size() - 1);
    auto result1 = std::ranges::max_element(view1);
    if (result1 == std::ranges::end(view1))
        throw std::runtime_error("Max not found!");
    auto result_distance = std::distance(row.begin(), result1);

    auto view2 = row | std::views::drop(result_distance + 1);
    auto result2 = std::ranges::max_element(view2);
    if (result2 == std::ranges::end(view2))
        throw std::runtime_error("Max not found!");

    return *result1 * 10 + *result2;
}

int main()
{
    const auto batteries = read_file();
    long sum_part1{0};

    for (const auto & row : batteries)
        sum_part1 += find_max_joltage(row);

    std::cout << sum_part1 << '\n';
}