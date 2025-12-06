
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

std::tuple<std::vector<num_row_t>, std::vector<char>> read_file(int num_operands)
{
    std::ifstream infile("input.txt");
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
        std::runtime_error("Unknown op!");
        break;
    }

    return result;
}

int main()
{
    int num_operands = 4;
    const auto [num_matrix, ops] = read_file(num_operands);

    long part1_result{ 0 };

    for (size_t i = 0; i < num_matrix.front().size(); i++) {
        std::vector<long> operands;
        for (int j = 0; j < num_operands; j++)
            operands.push_back(num_matrix.at(j).at(i));

        part1_result += part1_do_op(ops.at(i), operands);
    }

    std::cout << part1_result << '\n';
}