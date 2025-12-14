
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using wiring_t = std::vector<int>;
using mat_row_t = std::vector<int>;
using mat_t = std::vector<mat_row_t>;

struct machine {
    std::vector<char> indicators;
    std::vector<wiring_t> buttons;
    std::vector<int> joltage;
};

struct machine_bits {
    u_int32_t indicators{ 0 };
    std::vector<u_int32_t> buttons;

    machine_bits(const machine &mach)
    {
        for (size_t i = 0; i < mach.indicators.size(); i++) {
            if (mach.indicators.at(i) == '#')
                indicators |= 1U << i;
        }

        for (const auto &button : mach.buttons) {
            u_int32_t temp_button{ 0 };
            for (auto wire : button) {
                temp_button |= 1U << wire;
            }
            buttons.push_back(temp_button);
        }
    }
};

std::vector<machine> read_file(const std::string &filename)
{
    std::ifstream infile(filename);
    std::vector<int> rotations;
    std::vector<machine> machines;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        machines.emplace_back();
        {
            char indicator{ 0 }, left_bracket{ 0 };
            ss >> left_bracket >> indicator;
            while (indicator != ']') {
                machines.back().indicators.push_back(indicator);
                ss >> indicator;
            }
        }
        {
            char left_paren{ 0 }, comma{ 0 };
            int wire{ 0 };
            ss >> left_paren;
            while (left_paren == '(') {
                machines.back().buttons.emplace_back();
                do {
                    ss >> wire >> comma;
                    machines.back().buttons.back().push_back(wire);
                } while (comma == ',');
                ss >> left_paren;
            }
        }
        {
            int jolt{ 0 };
            char comma{ 0 };
            do {
                ss >> jolt >> comma;
                machines.back().joltage.push_back(jolt);
            } while (comma == ',');
        }
    }

    return machines;
}

u_int32_t part1_try_permutations(const machine_bits &mach)
{
    u_int32_t button_count = static_cast<u_int32_t>(mach.buttons.size());
    u_int32_t button_bits = 1U << button_count;
    std::vector<u_int32_t> results;

    for (u_int32_t i = 1; i < button_bits; i++) {
        u_int32_t result{ 0 };
        for (u_int32_t j = 0; j < button_count; j++) {
            u_int32_t bit_position = 1U << j;
            if (i & bit_position) {
                result = result ^ mach.buttons.at(j);
            }
        }
        if (result == mach.indicators) {
            results.push_back(i);
        }
    }

    u_int32_t min = std::ranges::min(results, [](auto a, auto b) { return std::popcount(a) < std::popcount(b); });

    return std::popcount(min);
}

mat_t part2_machine_to_mat(const machine &mach)
{
    mat_t mat(mach.joltage.size());
    {
        size_t i{ 0 };
        for (auto &row : mat) {
            row = mat_row_t(mach.buttons.size() + 1);
            row.back() = mach.joltage.at(i);
            i++;
        }
    }
    {
        size_t i{ 0 };
        for (const auto &button : mach.buttons) {
            for (auto wire : button)
                mat.at(wire).at(i) = 1;
            i++;
        }
    }

    return mat;
}

void part2_row_reduce_mat(mat_t &mat)
{
    size_t col{ 0 };
    for (size_t i = 0; i < mat.size(); i++) {
        bool swapped{ false };
        //swap to make leading 1 match column count
        if (mat.at(i).at(col) != 1) {
            for (size_t j = i; j < mat.size(); j++) {
                if (j == i)
                    continue;
                if (mat.at(j).at(i) == 1) {
                    mat.at(j).swap(mat.at(i));
                    swapped = true;
                    break;
                }
            }
            if (!swapped) {
                //increment col and try again
                i--;
                col++;
                continue;
            }
        }

        //subtract any above and below it
        for (size_t j = 0; j < mat.size(); j++) {
            if (j == i)
                continue;
            if (mat.at(j).at(col) == 1) {
                //mat.at(j) -= mat.at(i)
                for (size_t k = 0; k < mat.at(0).size(); k++)
                    mat.at(j).at(k) -= mat.at(i).at(k);
            }
        }
        col++;
    }
}

int main()
{
    const auto machines = read_file("test.txt");
    std::vector<machine_bits> machine_bits;

    for (const auto &mach : machines)
        machine_bits.emplace_back(mach);

    long part1_result{ 0 };
    for (const auto &mach_bits : machine_bits)
        part1_result += part1_try_permutations(mach_bits);

    std::cout << part1_result << '\n';

    auto mat = part2_machine_to_mat(machines.front());
    part2_row_reduce_mat(mat);
}