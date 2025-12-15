
#include "advent.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
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
    for (size_t i = 0; i < mat.size() && col < mat.at(0).size() - 1; i++) {
        bool swapped{ false };
        //swap to make leading 1 match column count
        if (mat.at(i).at(col) == 0) {
            for (size_t j = i; j < mat.size(); j++) {
                if (j == i)
                    continue;
                if (mat.at(j).at(i) != 0) {
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

        //scale the first to 1
        if (mat.at(i).at(col) == -1) {
            for (size_t k = 0; k < mat.at(0).size(); k++)
                mat.at(i).at(k) *= -1;
        }
        if (mat.at(i).at(col) != 1)
            throw std::runtime_error("Row is not started with 1!");

        //subtract any above and below it
        for (size_t j = 0; j < mat.size(); j++) {
            if (j == i)
                continue;
            while (mat.at(j).at(col) != 0) {
                if (mat.at(j).at(col) > 0) {
                    //mat.at(j) -= mat.at(i)
                    for (size_t k = 0; k < mat.at(0).size(); k++)
                        mat.at(j).at(k) -= mat.at(i).at(k);
                } else if (mat.at(j).at(col) < 0) {
                    for (size_t k = 0; k < mat.at(0).size(); k++)
                        mat.at(j).at(k) += mat.at(i).at(k);
                }
            }
        }
        col++;
    }

    //remove any rows with all zeros
    for (auto i = mat.begin(); i < mat.end();) {
        if (std::ranges::all_of(*i, [](auto a) { return a == 0; }))
            mat.erase(i);
        else
            ++i;
    }
}

std::vector<int> part2_find_free_vars(const mat_t &mat)
{
    std::vector<int> free_vars;
    size_t col{ 0 };
    for (size_t i = 0; i < mat.size() && col < mat.at(0).size() - 1; i++) {
        if (mat.at(i).at(col) != 1) {
            //found free_var
            free_vars.emplace_back(col);
            i--;
        }
        col++;
    }

    while (col + 1 < mat.at(0).size()) {
        free_vars.emplace_back(col);
        col++;
    }

    return free_vars;
}

int part2_solve_mat(const mat_t &mat, const std::vector<int> &free_vars, std::span<const int> vals)
{
    auto mat_mut = mat;
    std::vector<u_int8_t> val_found(mat.at(0).size() - 1);
    std::vector<int> test_vals(mat.at(0).size() - 1);
    {
        size_t i{ 0 };
        for (auto var : free_vars) {
            val_found.at(var) = true;
            test_vals.at(var) = vals[i];
            i++;
        }
    }

    //fill in the known vals
    for (auto &row : mat_mut) {
        size_t i{ 0 };
        for (auto var : test_vals) {
            if (val_found.at(i) && row.at(i) != 0) {
                row.back() -= var * row.at(i);
                row.at(i) = 0;
            }
            i++;
        }
    }

    //look for new variables that can be assigned values
    for (auto &row : mat_mut) {
        //if there is only one entry we can assign the value
        const int num_nonzeros =
            std::accumulate(row.begin(), row.end() - 1, 0, [](auto a, auto b) { return b += a != 0 ? 1 : 0; });
        if (num_nonzeros == 1) {
            const auto nonzero_iter = std::ranges::find_if(row, [](auto a) { return a != 0; });
            const auto index = std::distance(row.begin(), nonzero_iter);
            val_found.at(index) = true;
            test_vals.at(index) = row.back();
            row.clear();
        }
    }

    //remove empty arrays
    for (auto i = mat_mut.begin(); i < mat_mut.end();) {
        if ((*i).empty())
            mat_mut.erase(i);
        else
            ++i;
    }

    if (!mat_mut.empty())
        throw std::runtime_error("mat_mut not empty!");
    if (!std::ranges::all_of(val_found, [](auto a) { return a == true; }))
        throw std::runtime_error("All values not found!");

    //if any variables are negative then values are not a solution
    if (std::ranges::all_of(test_vals, [](auto a) { return a >= 0; })) {
        //if values are a solution, count the button presses, we are looking for the minimum
        return std::accumulate(test_vals.begin(), test_vals.end(), 0);
    }

    return -1;
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

    long part2_result{ 0 };

    for (const auto &mach : machines) {
        auto mat = part2_machine_to_mat(mach);
        part2_row_reduce_mat(mat);
        const auto free_vars = part2_find_free_vars(mat);
        const auto max_val =
            std::ranges::max_element(mat, [](const auto &a, const auto &b) { return a.back() < b.back(); });

        advt::permutator<int> perm(free_vars.size(), (*max_val).back());
        int min_presses{ std::numeric_limits<int>::max() };

        do {
            const auto presses = part2_solve_mat(mat, free_vars, perm.get_nums());
            if (presses > 0)
                min_presses = std::min(min_presses, presses);
            std::cout << "HALLO" << '\n';
        } while (perm.next_permutation());
        part2_result += min_presses;
    }
}