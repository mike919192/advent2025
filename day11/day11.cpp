
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using list_devices_t = std::unordered_map<std::string, std::vector<std::string>>;
using memo_map_t = std::unordered_map<std::string, int>;

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

int part1_eval_device(const list_devices_t &devices, const std::string &dev_name, memo_map_t &memo_map)
{
    //check if device is already in memo map
    if (memo_map.contains(dev_name))
        return memo_map.at(dev_name);

    const auto &outputs = devices.at(dev_name);
    int ret_val{ 0 };
    for (const auto &output : outputs) {
        if (output == "out")
            ret_val++;
        else
            ret_val += part1_eval_device(devices, output, memo_map);
    }
    //add to memo map
    memo_map.emplace(dev_name, ret_val);
    return ret_val;
}

int main()
{
    const auto devices = read_file("input.txt");
    memo_map_t memo_map;

    const auto part1_result = part1_eval_device(devices, "you", memo_map);

    std::cout << part1_result << '\n';
}
