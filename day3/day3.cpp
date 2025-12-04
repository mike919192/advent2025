
#include "advent.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using battery_row_t = std::vector<int>;
using battery_map_t = std::vector<battery_row_t>;

battery_map_t read_file()
{
    std::ifstream infile("input.txt");
    battery_map_t batteries;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        batteries.emplace_back();
        char batt_char{ 0 };

        while (ss >> batt_char) {
            batteries.back().push_back(advt::char_to_int(batt_char));
        }
    }

    return batteries;
}

long find_max_joltage(const battery_row_t &row, int num_batts)
{
    battery_row_t results;
    std::ptrdiff_t result_distance{ -1 };

    for (int i = 0; i < num_batts; i++) {
        auto view = row | std::views::take(row.size() - (num_batts - (i + 1))) | std::views::drop(result_distance + 1);
        auto result = std::ranges::max_element(view);
        if (result == std::ranges::end(view))
            throw std::runtime_error("Max not found!");
        result_distance = std::distance(row.begin(), result);
        results.push_back(*result);
    }

    long out_result{ 0 };
    long scale = static_cast<long>(std::pow(10, results.size() - 1));
    for (auto num : results) {
        out_result += num * scale;
        scale /= 10;
    }
    return out_result;
}

int main()
{
    const auto batteries = read_file();
    long sum_part1{ 0 };
    long sum_part2{ 0 };

    for (const auto &row : batteries)
        sum_part1 += find_max_joltage(row, 2);

    for (const auto &row : batteries)
        sum_part2 += find_max_joltage(row, 12);

    std::cout << sum_part1 << '\n';
    std::cout << sum_part2 << '\n';
}