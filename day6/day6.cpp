
#include "advent.hpp"
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

using num_row_t = std::vector<long>;

std::tuple<std::vector<num_row_t>, std::vector<char>> part1_read_file(int num_operands, const std::string &filename)
{
    std::ifstream infile(filename);
    std::vector<num_row_t> num_matrix;
    std::vector<char> ops;

    for (int i = 0; i < num_operands; i++) {
        std::string line;
        std::getline(infile, line);
        std::istringstream ss(line);

        num_matrix.emplace_back();

        long num{ 0 };

        while (ss >> num) {
            num_matrix.back().push_back(num);
        }
    }

    std::string line;
    std::getline(infile, line);
    std::istringstream ss(line);

    char op{ 0 };

    while (ss >> op) {
        ops.push_back(op);
    }

    return { num_matrix, ops };
}

std::vector<std::string> part2_read_file(int num_operands, const std::string &filename)
{
    std::ifstream infile(filename);
    std::vector<std::string> char_matrix;

    for (int i = 0; i < num_operands; i++) {
        std::string line;
        std::getline(infile, line);

        char_matrix.push_back(line);
    }

    return char_matrix;
}

long part1_do_op(char op, const std::vector<long> &operands)
{
    long result{ 0 };
    switch (op) {
    case '*':
        result = 1;
        std::ranges::for_each(operands, [&result](auto num) { result *= num; });
        break;

    case '+':
        std::ranges::for_each(operands, [&result](auto num) { result += num; });
        break;

    default:
        throw std::runtime_error("Unknown op!");
        break;
    }

    return result;
}

std::vector<long> part2_decode_operands(const std::vector<std::string> &char_matrix, size_t &position)
{
    std::vector<long> operands;

    while (position > 0) {
        long scale{ 1 };
        long num{ 0 };
        bool all_spaces{ true };
        for (auto i = char_matrix.rbegin(); i < char_matrix.rend(); i++) {
            const char num_char = (*i).at(position - 1);
            if (num_char == ' ')
                continue;
            all_spaces = false;
            num += advt::char_to_int(num_char) * scale;
            scale *= 10;
        }
        position--;
        if (!all_spaces)
            operands.push_back(num);
        else
            break;
    }

    return operands;
}

int main()
{
    const int num_operands = 4;
    const std::string filename = "input.txt";
    const auto [num_matrix, ops] = part1_read_file(num_operands, filename);

    long part1_result{ 0 };

    for (size_t i = 0; i < num_matrix.front().size(); i++) {
        std::vector<long> operands;
        for (const auto &row : num_matrix)
            operands.push_back(row.at(i));

        part1_result += part1_do_op(ops.at(i), operands);
    }

    std::cout << part1_result << '\n';

    const auto char_matrix = part2_read_file(num_operands, filename);

    size_t position{ char_matrix.front().size() };
    auto op = ops.rbegin();
    long part2_result{ 0 };

    while (position > 0 && op < ops.rend()) {
        const auto operands = part2_decode_operands(char_matrix, position);
        part2_result += part1_do_op(*op, operands);
        op++;
    }

    std::cout << part2_result << '\n';
}