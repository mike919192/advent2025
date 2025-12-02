
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

std::vector<int> read_file()
{
    std::ifstream infile("input.txt");
    std::vector<int> rotations;

    for (std::string line; std::getline(infile, line);) {
        std::istringstream ss(line);
        char dir = 0;
        int turn = 0;

        ss >> dir >> turn;

        if (dir == 'L')
            turn *= -1;

        rotations.push_back(turn);
    }

    return rotations;
}

struct safe_t {
    int current_position{ 0 };
    int count_zero{ 0 };

    void turn(int rotation)
    {
        current_position += rotation;
        while (current_position < 0)
            current_position += 100;
        while (current_position >= 100)
            current_position -= 100;

        if (current_position == 0)
            count_zero++;
    }
};

int main()
{
    auto rotations = read_file();
    safe_t safe{ 50 };

    for (auto rot : rotations)
        safe.turn(rot);

    std::cout << safe.count_zero << '\n';
}