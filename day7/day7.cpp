
#include "advent.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>

using manifold_map_t = std::map<advt::xy_pos, long>;

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
                splitters.emplace(advt::xy_pos{ x, y }, 0);
            else if (value == 'S')
                beam.emplace(advt::xy_pos{ x, y }, 0);

            x++;
        }

        y++;
    }

    return { splitters, beam, advt::xy_pos{ x, y } };
}

void add_to_map(manifold_map_t &active_beams, const advt::xy_pos &pos)
{
    if (!active_beams.contains(pos)) {
        active_beams.emplace(pos, 0);
    }
}

long part1_split_beams(const manifold_map_t &splitters, manifold_map_t &active_beams, manifold_map_t &inactive_beams,
                       const advt::xy_pos &dim)
{
    long split_count{ 0 };
    manifold_map_t next_active_beams;
    for (const auto &beam : active_beams) {
        advt::xy_pos next_pos = beam.first + advt::xy_pos{ 0, 1 };
        if (advt::is_pos_on_map(next_pos, dim)) {
            if (splitters.contains(next_pos)) {
                add_to_map(next_active_beams, next_pos + advt::xy_pos{ -1, 0 });
                add_to_map(next_active_beams, next_pos + advt::xy_pos{ 1, 0 });
                split_count++;
            } else {
                next_active_beams.emplace(next_pos, 0);
            }
        }
        inactive_beams.insert(beam);
    }

    active_beams = next_active_beams;
    return split_count;
}

long part2_trace_beam(const advt::xy_pos &beam, const manifold_map_t &splitters, const advt::xy_pos &dim)
{
    advt::xy_pos local_beam = beam;
    while (advt::is_pos_on_map(local_beam, dim) && !splitters.contains(local_beam))
        local_beam += advt::xy_pos{ 0, 1 };

    if (!advt::is_pos_on_map(local_beam, dim))
        return 1;

    return splitters.at(local_beam);
}

//splitter is ordered map so loop in reverse
//trace beam paths
//if hit bottom add 1
//if hit other splitter add that value
void part2_count_timeline(manifold_map_t &splitters, const advt::xy_pos &dim)
{
    for (auto i = splitters.rbegin(); i != splitters.rend(); i++) {
        long count{ 0 };
        count += part2_trace_beam((*i).first + advt::xy_pos{ -1, 0 }, splitters, dim);
        count += part2_trace_beam((*i).first + advt::xy_pos{ 1, 0 }, splitters, dim);
        (*i).second = count;
    }
}

int main()
{
    auto [splitters, active_beam, dim] = read_file();
    manifold_map_t inactive_beams;

    long part1_answer{ 0 };

    while (!active_beam.empty())
        part1_answer += part1_split_beams(splitters, active_beam, inactive_beams, dim);

    std::cout << part1_answer << '\n';

    part2_count_timeline(splitters, dim);

    std::cout << (*splitters.begin()).second << '\n';
}