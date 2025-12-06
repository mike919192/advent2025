
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

using pair_t = std::pair<long, long>;

std::tuple<std::vector<pair_t>, std::vector<long>> read_file()
{
    std::ifstream infile("input.txt");
    std::vector<pair_t> id_ranges;
    std::vector<long> ids;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);

        long num1{ 0 };
        long num2{ 0 };
        char dash{ 0 };

        if (!(ss >> num1 >> dash >> num2))
            break;

        id_ranges.emplace_back(num1, num2);
    }

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);

        long num1{ 0 };

        if (!(ss >> num1))
            break;

        ids.push_back(num1);
    }

    return { id_ranges, ids };
}

int main()
{
    const auto [id_ranges, ids] = read_file();

    long part1_count{ 0 };

    for (const auto id : ids) {
        if (std::ranges::any_of(id_ranges, [&id](const pair_t &id_range) {
                return id >= id_range.first && id <= id_range.second;
            }))
            part1_count++;
    }

    std::cout << part1_count << '\n';
}