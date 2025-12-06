
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

//this algorithm requires the ranges to be sorted
void part2_merge_ranges(std::vector<pair_t> &id_ranges)
{
    for (size_t i = 0; i < id_ranges.size() - 1; i++) {
        for (size_t j = i + 1; j < id_ranges.size(); j++) {
            //if (i) second is greater than (j) first than we can merge
            if (id_ranges.at(i).second >= id_ranges.at(j).first - 1) {
                id_ranges.at(i).second = std::max(id_ranges.at(i).second, id_ranges.at(j).second);
                id_ranges.erase(id_ranges.begin() + j);
                j--;
            } else {
                break;
            }
        }
    }
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

    auto id_ranges_mut = id_ranges;

    std::ranges::sort(id_ranges_mut);
    part2_merge_ranges(id_ranges_mut);

    long part2_count{ 0 };

    for (const auto &range : id_ranges_mut)
        part2_count += range.second - range.first + 1;

    std::cout << part1_count << '\n';
    std::cout << part2_count << '\n';
}