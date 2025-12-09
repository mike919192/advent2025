
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using point_t = std::array<long, 3>;
using points_t = std::vector<point_t>;
using points_groups_t = std::vector<points_t>;
using distances_map_t = std::map<long, std::array<std::array<long, 3>, 2>>;

points_t read_file()
{
    std::ifstream infile("input.txt");
    points_t points;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        long x = 0;
        long y = 0;
        long z = 0;
        char comma = 0;

        (ss >> x >> comma >> y >> comma >> z);

        points.push_back({x, y, z});
    }

    return points;
}

distances_map_t part1_find_distances(const points_t & points)
{
    distances_map_t distances;
    
    for (auto i = points.begin(); i < points.end(); i++) {
        for (auto j = i + 1; j < points.end(); j++) {

            long distance = ((*i)[0] - (*j)[0]) * ((*i)[0] - (*j)[0]) +
                            ((*i)[1] - (*j)[1]) * ((*i)[1] - (*j)[1]) +
                            ((*i)[2] - (*j)[2]) * ((*i)[2] - (*j)[2]);
            
            std::array<std::array<long, 3>, 2> test = {(*i), (*j)};
            distances.emplace(distance, test);
        }
    }

    return distances;
}

auto part1_find_point_in_group(points_groups_t & groups, const point_t & point)
{
    auto i = groups.begin();
    while (i != groups.end()) {
        for (const auto & pnt : (*i)) {
            if (pnt == point)
                return i;
        }
        i++;
    }

    return i;
}

points_groups_t part1_find_groups(const distances_map_t & distances)
{
    points_groups_t groups;
    auto dis = distances.begin();
    for (int i = 0 ; i < 1000; i++) {
        //check if either point is in a group
        auto i1 = part1_find_point_in_group(groups, (*dis).second.at(0));
        auto i2 = part1_find_point_in_group(groups, (*dis).second.at(1));

        //neither one is part of group
        if (i1 == groups.end() && i2 == groups.end()) {
            groups.emplace_back();
            groups.back().emplace_back((*dis).second.at(0));
            groups.back().emplace_back((*dis).second.at(1));
        } else if (i1 == i2) {
            //already in the same group nothing happens!
        } else if (i1 != groups.end() && i2 == groups.end()) {
            (*i1).emplace_back((*dis).second.at(1));
        } else if (i2 != groups.end() && i1 == groups.end()) {
            (*i2).emplace_back((*dis).second.at(0));
        } else {
            //merge
            (*i1).insert((*i1).end(), (*i2).begin(), (*i2).end());
            groups.erase(i2);
        }

        ++dis;
    }

    return groups;
}

int main()
{
    const auto points = read_file();

    const auto distances = part1_find_distances(points);

    auto groups = part1_find_groups(distances);

    std::ranges::sort(groups, [](const auto & a, const auto & b) { return a.size() > b.size(); });

    long part1_result = groups.at(0).size() * groups.at(1).size() * groups.at(2).size();

    std::cout << part1_result << '\n';
}