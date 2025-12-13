
#include "advent.hpp"
#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

using points_t = std::vector<advt::xy_pos>;
using areas_map_t = std::multimap<long, std::array<advt::xy_pos, 2>>;
using map_t = std::unordered_map<advt::xy_pos, std::array<advt::xy_pos, 2>>;
using keepout_map_t = std::unordered_map<advt::xy_pos, char>;

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
            const long i_x = static_cast<long>((*i).x);
            const long i_y = static_cast<long>((*i).y);
            const long j_x = static_cast<long>((*j).x);
            const long j_y = static_cast<long>((*j).y);
            const long area = (std::abs(i_x - j_x) + 1) * (std::abs(i_y - j_y) + 1);

            areas.emplace(area, std::array<advt::xy_pos, 2>{ (*i), (*j) });
        }
    }

    return areas;
}

void normalize_dir(advt::xy_pos &dir)
{
    const int max = std::max(std::abs(dir.x), std::abs(dir.y));
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
    if ((last_dir == advt::xy_pos{ 0, 1 } && dir == advt::xy_pos{ 1, 0 }) ||
        (last_dir == advt::xy_pos{ 1, 0 } && dir == advt::xy_pos{ 0, -1 }) ||
        (last_dir == advt::xy_pos{ 0, -1 } && dir == advt::xy_pos{ -1, 0 }) ||
        (last_dir == advt::xy_pos{ -1, 0 } && dir == advt::xy_pos{ 0, 1 }))
        return -90;

    return 0;
}

advt::xy_pos create_turn(const advt::xy_pos &dir, bool clockwise)
{
    if (clockwise) {
        if (dir == advt::xy_pos{ 1, 0 })
            return advt::xy_pos{ 0, 1 };
        if (dir == advt::xy_pos{ 0, 1 })
            return advt::xy_pos{ -1, 0 };
        if (dir == advt::xy_pos{ -1, 0 })
            return advt::xy_pos{ 0, -1 };
        if (dir == advt::xy_pos{ 0, -1 })
            return advt::xy_pos{ 1, 0 };
    } else {
        if (dir == advt::xy_pos{ 1, 0 })
            return advt::xy_pos{ 0, -1 };
        if (dir == advt::xy_pos{ 0, -1 })
            return advt::xy_pos{ -1, 0 };
        if (dir == advt::xy_pos{ -1, 0 })
            return advt::xy_pos{ 0, 1 };
        if (dir == advt::xy_pos{ 0, 1 })
            return advt::xy_pos{ 1, 0 };
    }
    throw std::runtime_error("Cannot determine turn!");
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
            map.emplace(trace, std::array<advt::xy_pos, 2>{ dir, to_last_point });
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

keepout_map_t part2_gen_keepout(const map_t &map, bool clockwise)
{
    keepout_map_t keepout;
    for (const auto &point : map) {
        const auto dir = create_turn(point.second.at(0), !clockwise);
        const auto pt1 = point.first + dir;
        const auto dir2 = create_turn(point.second.at(1), clockwise);
        const auto pt2 = point.first + dir2;
        if (!keepout.contains(pt1) && !map.contains(pt1))
            keepout.emplace(pt1, 0);
        if (!keepout.contains(pt2) && !map.contains(pt2))
            keepout.emplace(pt2, 0);
    }
    return keepout;
}

bool part2_check_rectangle(const std::array<advt::xy_pos, 2> &rect, const keepout_map_t &keepout)
{
    std::array<advt::xy_pos, 4> points = { rect.at(0), advt::xy_pos{ rect.at(0).x, rect.at(1).y }, rect.at(1),
                                           advt::xy_pos{ rect.at(1).x, rect.at(0).y } };

    for (auto *i = points.begin(); i < points.end(); i++) {
        auto *i2 = i + 1;
        if (i2 == points.end())
            i2 = points.begin();
        advt::xy_pos dir = (*i2) - (*i);
        normalize_dir(dir);
        advt::xy_pos trace = *i;
        while (trace != *i2) {
            if (keepout.contains(trace))
                return false;
            trace += dir;
        }
    }

    return true;
}

int main()
{
    const auto points = read_file("input.txt");

    //gets all possible areas
    //creates a multimap
    //key is area
    //value is 2 points that made the rectangle
    const auto areas = part1_find_areas(points);

    std::cout << (*areas.rbegin()).first << '\n';

    //connect the points
    //map is unordered_map
    //key is xy_point
    //value is array of 2 vectors, next point and prev point
    //clockwise is a bool if we traced in clockwise direction
    const auto [map, clockwise] = part2_trace_points(points);

    //keepout is a unordered map of points outside the connected points
    //key is xy_point
    //value is unused
    //if any rectangles overlap the keepout then they are not valid
    const auto keepout = part2_gen_keepout(map, clockwise);

    long part2_answer{ 0 };
    int test{ 0 };

    for (auto i = areas.rbegin(); i != areas.rend(); i++) {
        //loop the rectangles started with largest
        //only pass if the rectangle lines do not overlap any keepout
        const bool pass = part2_check_rectangle((*i).second, keepout);
        if (pass) {
            part2_answer = (*i).first;
            break;
        }
        std::cout << test << '\n';
        test++;
    }

    std::cout << part2_answer << '\n';
}