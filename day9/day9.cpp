
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using point_t = std::array<long, 2>;
using points_t = std::vector<point_t>;
using areas_map_t = std::map<long, std::array<point_t, 2>>;

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

        points.push_back({ x, y });
    }

    return points;
}

areas_map_t part1_find_areas(const points_t &points)
{
    areas_map_t areas;

    for (auto i = points.begin(); i < points.end(); i++) {
        for (auto j = i + 1; j < points.end(); j++) {
            long area = (std::abs((*i)[0] - (*j)[0]) + 1) * (std::abs((*i)[1] - (*j)[1]) + 1);

            std::array<point_t, 2> test = { (*i), (*j) };
            areas.emplace(area, test);
        }
    }

    return areas;
}

int main()
{
    const auto points = read_file("input.txt");

    const auto areas = part1_find_areas(points);

    std::cout << (*areas.rbegin()).first << '\n';
}