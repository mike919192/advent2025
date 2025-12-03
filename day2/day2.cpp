
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using pair_t = std::pair<long, long>;

std::vector<pair_t> read_file()
{
    std::ifstream infile("input.txt");
    std::vector<pair_t> rotations;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);

        while(true)
        {
            long num1 = 0;
            long num2 = 0;
            char dash = 0;
            char comma = 0;

            if (!(ss >> num1 >> dash >> num2))
                break;

            rotations.push_back(pair_t(num1, num2));

            if (!(ss >> comma))
                break;
        }
    }

    return rotations;
}

int main()
{
    const auto pairs = read_file();
    std::regex repeat_regex("(^\\d+)+\\1$");
    long sum_part1{0};

    for (const auto & pair : pairs) {
        for (long num = pair.first; num <= pair.second; num++) {
            auto check_str = std::to_string(num);
            auto check_begin = std::sregex_iterator(check_str.begin(), check_str.end(), repeat_regex);
            auto check_end = std::sregex_iterator();

            for (std::sregex_iterator i = check_begin; i != check_end; ++i) {
                std::smatch match = *i;
                std::string match_str = match.str();

                if (match.size() > 0) {
                    sum_part1 += num;
                }
            }
        }
    }

    std::cout << sum_part1 << '\n';
}