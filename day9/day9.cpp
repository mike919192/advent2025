
#include "advent.hpp"
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using points_t = std::vector<advt::xy_pos>;
using areas_map_t = std::multimap<long, std::array<advt::xy_pos, 2>>;
using map_t = std::unordered_map<advt::xy_pos, std::array<advt::xy_pos, 2>>;

points_t read_file(const std::string &filename)
{
    std::ifstream infile(filename);
    points_t points;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        long x = 0;
        long y = 0;
        char comma = 0;

        (ss >> x >> comma >> y);

        points.emplace_back(x, y);
    }

    return points;
}

areas_map_t part1_find_areas(const points_t &points)
{
    areas_map_t areas;

    for (auto i = points.begin(); i < points.end(); i++) {
        for (auto j = i + 1; j < points.end(); j++) {
            long i_x = static_cast<long>((*i).x);
            long i_y = static_cast<long>((*i).y);
            long j_x = static_cast<long>((*j).x);
            long j_y = static_cast<long>((*j).y);
            long area = (std::abs(i_x - j_x) + 1) * (std::abs(i_y - j_y) + 1);

            std::array<advt::xy_pos, 2> test = { (*i), (*j) };
            areas.emplace(area, test);
        }
    }

    return areas;
}

void normalize_dir(advt::xy_pos &dir)
{
    int max = std::max(std::abs(dir.x), std::abs(dir.y));
    if (max == 0)
        throw std::runtime_error("Max is zero!");
    dir.x /= max;
    dir.y /= max;
}

int determine_turn(const advt::xy_pos &last_dir, const advt::xy_pos &dir)
{
    if ((last_dir == advt::xy_pos{ 1, 0 } && dir == advt::xy_pos{ 0, 1 }) ||
        (last_dir == advt::xy_pos{ 0, 1 } && dir == advt::xy_pos{ -1, 0 }) ||
        (last_dir == advt::xy_pos{ -1, 0 } && dir == advt::xy_pos{ 0, -1 }) ||
        (last_dir == advt::xy_pos{ 0, -1 } && dir == advt::xy_pos{ 1, 0 }))
        return 90;
    else if ((last_dir == advt::xy_pos{ 0, 1 } && dir == advt::xy_pos{ 1, 0 }) ||
             (last_dir == advt::xy_pos{ 1, 0 } && dir == advt::xy_pos{ 0, -1 }) ||
             (last_dir == advt::xy_pos{ 0, -1 } && dir == advt::xy_pos{ -1, 0 }) ||
             (last_dir == advt::xy_pos{ -1, 0 } && dir == advt::xy_pos{ 0, 1 }))
        return -90;

    return 0;
}

std::tuple<map_t, bool> part2_trace_points(const points_t &points)
{
    int degrees{ 0 };
    map_t map;
    advt::xy_pos last_dir{ 0, 0 };
    advt::xy_pos to_last_point{ 0, 0 };

    for (auto i = points.begin(); i < points.end(); ++i) {
        auto i2 = i + 1;
        if (i2 == points.end())
            i2 = points.begin();
        advt::xy_pos dir = (*i2) - (*i);
        normalize_dir(dir);
        advt::xy_pos trace = *i;
        to_last_point = -last_dir;
        while (trace != *i2) {
            map.emplace(trace, std::array<advt::xy_pos, 2>{dir, to_last_point});
            trace += dir;
            to_last_point = -dir;
        }
        degrees += determine_turn(last_dir, dir);
        last_dir = dir;
    }

    //need to add reverse direction to first point
    map.at(points.at(0)).at(1) = -last_dir;

    return { map, degrees > 0 };
}

bool check_point(const map_t & map, const advt::xy_pos & pt, const advt::xy_pos & dir)
{
    //if no line in that direction return true
    if (!map.contains(pt + dir))
        return true;

    int degrees = determine_turn(dir, map.at(pt + dir)[0]);
    int degrees2 = determine_turn(dir, map.at(pt + dir)[1]);
    
    return degrees >= 0 && degrees2 <= 0;
}

bool part2_check_rectangle(const map_t &map, const std::array<advt::xy_pos, 2> &rect)
{
    std::array<advt::xy_pos, 4> pts = { rect.at(0), advt::xy_pos{ rect.at(0).x, rect.at(1).y }, rect.at(1),
                                        advt::xy_pos{ rect.at(1).x, rect.at(0).y } };

    std::array<advt::xy_pos, 4> check_pts = { advt::xy_pos{ 1, 0 }, advt::xy_pos{ 0, 1 }, advt::xy_pos{ -1, 0 },
                                                  advt::xy_pos{ 0, -1 } };

    for (auto i = pts.begin(); i < pts.end(); i++) {
        auto i2 = i + 1;
        if (i2 == pts.end())
            i2 = pts.begin();
        advt::xy_pos dir = (*i2) - (*i);
        normalize_dir(dir);
        advt::xy_pos trace = *i;
        while (trace != *i2) {
            if (map.contains(trace)) {
                trace += dir;
                continue;
            }
            for (const auto & check_pt : check_pts) {
                if (!check_point(map, trace, check_pt))
                    return false;
            }
            trace += dir;
        }
    }

    return true;
}

int main()
{
    const auto points = read_file("input.txt");

    const auto areas = part1_find_areas(points);

    std::cout << (*areas.rbegin()).first << '\n';

    const auto [map, clockwise] = part2_trace_points(points);

    long part2_answer {0};
    int test {0};

    for (auto i = areas.rbegin(); i != areas.rend(); i++) {
        bool pass = part2_check_rectangle(map, (*i).second);
        if (pass) {
            part2_answer = (*i).first;
            break;
        }
        std::cout << test << '\n';
        test++;
    }
    

    std::cout << part2_answer << '\n';
}