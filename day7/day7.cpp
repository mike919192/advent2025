
#include "advent.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

using manifold_map_t = std::unordered_map<advt::xy_pos, char>;

std::tuple<manifold_map_t, manifold_map_t, advt::xy_pos> read_file()
{
    std::ifstream infile("input.txt");
    manifold_map_t splitters;
    manifold_map_t beam;

    int y{ 0 };
    int x{ 0 };

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);

        x = 0;

        char value{ 0 };
        while (ss >> value) {
            if (value == '^')
                splitters.emplace(advt::xy_pos{ x, y }, value);
            else if (value == 'S')
                beam.emplace(advt::xy_pos{ x, y }, value);

            x++;
        }

        y++;
    }

    return { splitters, beam, advt::xy_pos{ x, y } };
}

void add_to_map(manifold_map_t &active_beams, const advt::xy_pos &pos, char symbol)
{
    if (!active_beams.contains(pos)) {
        active_beams.emplace(pos, symbol);
    }
}

long part1_split_beams(const manifold_map_t &splitters, manifold_map_t &active_beams, const advt::xy_pos &dim)
{
    long split_count{ 0 };
    manifold_map_t next_active_beams;
    for (const auto &beam : active_beams) {
        advt::xy_pos next_pos = beam.first + advt::xy_pos{ 0, 1 };
        if (advt::is_pos_on_map(next_pos, dim)) {
            if (splitters.contains(next_pos)) {
                add_to_map(next_active_beams, next_pos + advt::xy_pos{ -1, 0 }, '|');
                add_to_map(next_active_beams, next_pos + advt::xy_pos{ 1, 0 }, '|');
                split_count++;
            } else {
                next_active_beams.emplace(next_pos, '|');
            }
        }
    }

    active_beams = next_active_beams;
    return split_count;
}

int main()
{
    const auto [splitters, active_beam, dim] = read_file();
    auto active_beam_mut = active_beam;

    long part1_answer{ 0 };

    while (!active_beam_mut.empty())
        part1_answer += part1_split_beams(splitters, active_beam_mut, dim);

    std::cout << part1_answer << '\n';
}