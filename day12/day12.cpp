
#include "advent.hpp"
#include <array>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

using shape_t = std::array<std::array<char, 3>, 3>;
struct region {
    advt::xy_pos size;
    std::vector<int> num_shapes;
};

std::tuple<std::vector<shape_t>, std::vector<region>> read_file(const std::string &filename)
{
    std::ifstream infile(filename);
    std::vector<shape_t> shapes;
    std::vector<region> regions;

    //assume there's 6 shapes 3x3
    for (int i = 0; i < 6; i++) {
        shape_t shape{};
        std::string line;
        std::getline(infile, line);
        for (int j = 0; j < 3; j++) {
            std::getline(infile, line);
            std::istringstream ss(line);
            ss >> shape.at(j).at(0) >> shape.at(j).at(1) >> shape.at(j).at(2);
        }
        shapes.emplace_back(shape);
        std::getline(infile, line);
    }

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        region reg;
        int x{ 0 };
        int y{ 0 };
        int num_shape{ 0 };
        char x_colon{ 0 };
        ss >> x >> x_colon >> y >> x_colon;
        reg.size = advt::xy_pos{ x, y };
        while (ss >> num_shape)
            reg.num_shapes.push_back(num_shape);
        regions.emplace_back(reg);
    }

    return { shapes, regions };
}

int main()
{
    const auto [shapes, regions] = read_file("input.txt");

    //determine perfect packing for lower limit to exclude anything lower
    std::array<int, 6> perfect_packing{};
    for (size_t i = 0; i < perfect_packing.size(); ++i) {
        for (size_t j = 0; j < shapes.at(i).size(); ++j)
            perfect_packing.at(i) += std::accumulate(shapes.at(i).at(j).begin(), shapes.at(i).at(j).end(), 0,
                                                     [](auto a, auto b) { return a += b == '#' ? 1 : 0; });
    }

    //determine worst case packing for upper limit to include anything higher
    std::array<int, 6> worst_packing{ 9, 9, 9, 9, 9, 9 };

    int result{ 0 };

    for (const auto &region : regions) {
        //determine best and worst case size requirements
        std::array<int, 6> perfect_packing_size{};
        std::array<int, 6> worst_packing_size{};
        for (size_t i = 0; i < region.num_shapes.size(); ++i) {
            perfect_packing_size.at(i) = region.num_shapes.at(i) * perfect_packing.at(i);
            worst_packing_size.at(i) = region.num_shapes.at(i) * worst_packing.at(i);
        }
        const int best_size = region.size.x * region.size.y;
        const int worst_size = ((region.size.x / 3) * 3) * ((region.size.y / 3) * 3);
        const int perfect_packing_accum = std::accumulate(perfect_packing_size.begin(), perfect_packing_size.end(), 0);
        const int worst_packing_accum = std::accumulate(worst_packing_size.begin(), worst_packing_size.end(), 0);

        if (best_size < perfect_packing_accum) {
            //dont count this
        } else if (worst_packing_accum <= worst_size) {
            result++;
        } else {
            throw std::runtime_error("Can't determine this one!");
        }
    }

    std::cout << result << '\n';
}