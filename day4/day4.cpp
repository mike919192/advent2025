
#include "advent.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using map_row_t = std::vector<char>;
using paper_map_t = std::vector<map_row_t>;

paper_map_t read_file()
{
    std::ifstream infile("input.txt");
    paper_map_t map;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        map.emplace_back();

        char value{ 0 };
        while (ss >> value) {
            map.back().push_back(value);
        }
    }

    return map;
}

template <typename t_t>
int count_accessible(t_t &map)
{
    const advt::xy_pos dim{ static_cast<int>(map.front().size()), static_cast<int>(map.size()) };
    int count_available{ 0 };

    for (int y = 0; y < dim.y; y++) {
        for (int x = 0; x < dim.x; x++) {
            if (map.at(y).at(x) != '@')
                continue;

            int count_adjacent{ 0 };

            for (int sub_y = -1; sub_y <= 1; sub_y++) {
                for (int sub_x = -1; sub_x <= 1; sub_x++) {
                    if (advt::is_pos_on_map(advt::xy_pos{ x + sub_x, y + sub_y }, dim) &&
                        map.at(y + sub_y).at(x + sub_x) != '.') {
                        count_adjacent++;
                    }
                }
            }

            //the loop self counts, < 4 adjacent rolls
            if (count_adjacent < 5) {
                count_available++;
                //part2 only
                if constexpr (!std::is_const_v<t_t>) {
                    //mark as x, to be cleaned up
                    map.at(y).at(x) = 'x';
                }
            }
        }
    }

    return count_available;
}

void remove_accessible(paper_map_t &map)
{
    for (auto &row : map)
        for (auto &cell : row)
            if (cell == 'x')
                cell = '.';
}

int main()
{
    const auto map = read_file();
    auto map_mut = map;

    const auto result_part1 = count_accessible(map);
    std::cout << result_part1 << '\n';

    int result_part2{ 0 };
    while (const int result = count_accessible(map_mut)) {
        result_part2 += result;
        remove_accessible(map_mut);
    }

    std::cout << result_part2 << '\n';
}