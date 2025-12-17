
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using list_devices_t = std::unordered_map<std::string, std::vector<std::string>>;
using memo_map_t = std::unordered_map<std::string, int64_t>;

list_devices_t read_file(const std::string &filename)
{
    std::ifstream infile(filename);
    list_devices_t devices;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        std::string name;

        ss >> name;
        name.pop_back();

        std::vector<std::string> outputs;
        std::string output;
        while (ss >> output)
            outputs.push_back(output);

        devices.emplace(name, outputs);
    }

    return devices;
}

int64_t part1_eval_device(const list_devices_t &devices, const std::string &dev_name, const std::string &dest_name,
                      memo_map_t &memo_map)
{
    //check if device is already in memo map
    if (memo_map.contains(dev_name))
        return memo_map.at(dev_name);

    const auto &outputs = devices.at(dev_name);
    int64_t ret_val{ 0 };
    for (const auto &output : outputs) {
        if (output == dest_name) {
            if (memo_map.contains(dest_name))
                ret_val += memo_map.at(dest_name);
            else
                ret_val++;
        }
        else if (output != "out")
            ret_val += part1_eval_device(devices, output, dest_name, memo_map);
    }
    //add to memo map
    memo_map.emplace(dev_name, ret_val);
    return ret_val;
}

int main()
{
    const auto devices = read_file("input.txt");
    {
        // const auto devices = read_file("test.txt");
        memo_map_t memo_map;
        const auto part1_result = part1_eval_device(devices, "you", "out", memo_map);
        std::cout << part1_result << '\n';
    }

    // const auto devices = read_file("test2.txt");
    
    std::array<std::array<std::string, 4>, 2> paths {"svr", "fft", "dac", "out", "svr", "dac", "fft", "out"};

    int64_t part2_result{ 0 };
    for (const auto & path : paths) {
        int64_t path_result{ 0 };
        memo_map_t memo_map;
        for (auto i = path.rbegin(); i < path.rend() - 1; ++i) {
            path_result = part1_eval_device(devices, *(i + 1), *i, memo_map);
            memo_map = memo_map_t();
            memo_map.emplace(*(i + 1), path_result);
        }
        part2_result += path_result;
    }
    std::cout << part2_result << '\n';
}
