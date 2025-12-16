
#pragma once

#include <algorithm>
#include <numeric>
#include <span>
#include <stdexcept>
#include <vector>

namespace advt
{

inline int char_to_int(char in)
{
    if (in < '0' || in > '9')
        throw std::runtime_error("Char is not a number!");

    return in - '0';
}

struct xy_pos {
    int x{ 0 };
    int y{ 0 };

    // compound assignment (does not need to be a member,
    // but often is, to modify the private members)
    xy_pos &operator+=(const xy_pos &rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this; // return the result by reference
    }

    xy_pos &operator-=(const xy_pos &rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this; // return the result by reference
    }

    // friends defined inside class body are inline and are hidden from non-ADL lookup
    // passing lhs by value helps optimize chained a+b+c
    // otherwise, both parameters may be const references
    friend xy_pos operator+(xy_pos lhs, const xy_pos &rhs)
    {
        lhs += rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }

    friend xy_pos operator-(xy_pos lhs, const xy_pos &rhs)
    {
        lhs -= rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }

    xy_pos operator-() const
    {
        return xy_pos{ -x, -y };
    }

    friend bool operator<(const xy_pos &lhs, const xy_pos &rhs)
    {
        return std::tie(lhs.y, lhs.x) < std::tie(rhs.y, rhs.x); // keep the same order
    }

    friend bool operator>(const xy_pos &lhs, const xy_pos &rhs)
    {
        return rhs < lhs;
    }

    friend bool operator<=(const xy_pos &lhs, const xy_pos &rhs)
    {
        return !(lhs > rhs);
    }

    friend bool operator>=(const xy_pos &lhs, const xy_pos &rhs)
    {
        return !(lhs < rhs);
    }

    friend bool operator==(const xy_pos &lhs, const xy_pos &rhs)
    {
        return std::tie(lhs.y, lhs.x) == std::tie(rhs.y, rhs.x);
    }

    friend bool operator!=(const xy_pos &lhs, const xy_pos &rhs)
    {
        return !(lhs == rhs);
    }
};

inline bool is_pos_on_map(const xy_pos &pos, const xy_pos &dim)
{
    return pos.x >= 0 && pos.y >= 0 && pos.x < dim.x && pos.y < dim.y;
}

template <typename t_t>
struct map : public t_t {
    auto &&at_pos(const xy_pos &pos) const
    {
        return this->at(pos.y).at(pos.x);
    }

    auto &&at_pos(const xy_pos &pos)
    {
        return this->at(pos.y).at(pos.x);
    }

    auto &&at_pos(int x, int y) const
    {
        return this->at(y).at(x);
    }

    auto &&at_pos(int x, int y)
    {
        return this->at(y).at(x);
    }
};

template <typename t_t, t_t reset_value_t = 0>
struct permutator {
private:
    std::vector<t_t> m_nums{};
    t_t m_max_value;

public:
    permutator(size_t n, t_t max_value) : m_nums(n, reset_value_t), m_max_value(max_value)
    {
    }

    //return false when the permutation repeats
    bool next_permutation()
    {
        for (auto &num : m_nums) {
            if (num == m_max_value) {
                num = reset_value_t;
            } else {
                num++;
                break;
            }
        }
        return !std::ranges::all_of(m_nums, [](auto i) { return i == reset_value_t; });
    }

    std::span<const t_t> get_nums()
    {
        return std::span(m_nums);
    }
};

struct fraction {
    int num{ 0 };
    int denom{ 1 };

    fraction() = default;

    fraction(int a) : num(a)
    {
    }

    fraction(int a, int b) : num(a), denom(b)
    {
    }

    void simplify()
    {
        int gcd = std::gcd(num, denom);
        if (denom < 0)
            gcd *= -1;
        num /= gcd;
        denom /= gcd;
    }

    void reciprocal()
    {
        std::swap(num, denom);
    }

    // compound assignment (does not need to be a member,
    // but often is, to modify the private members)
    fraction &operator+=(const fraction &rhs)
    {
        if (denom != rhs.denom) {
            const int lcd = std::lcm(denom, rhs.denom);
            const int scale = lcd / denom;
            const int rhs_scale = lcd / rhs.denom;
            denom = lcd;
            num = (num * scale) + (rhs.num * rhs_scale);
            simplify();
            return *this;
        }
        num += rhs.num;
        simplify();
        return *this; // return the result by reference
    }

    fraction operator-() const
    {
        return { -num, denom };
    }

    fraction &operator-=(const fraction &rhs)
    {
        *this += -rhs;
        return *this; // return the result by reference
    }

    fraction &operator*=(const fraction &rhs)
    {
        num *= rhs.num;
        denom *= rhs.denom;
        simplify();
        return *this; // return the result by reference
    }

    fraction &operator/=(const fraction &rhs)
    {
        num *= rhs.denom;
        denom *= rhs.num;
        simplify();
        return *this; // return the result by reference
    }

    // friends defined inside class body are inline and are hidden from non-ADL lookup
    // passing lhs by value helps optimize chained a+b+c
    // otherwise, both parameters may be const references
    friend fraction operator+(fraction lhs, const fraction &rhs)
    {
        lhs += rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }

    friend fraction operator-(fraction lhs, const fraction &rhs)
    {
        lhs -= rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }

    friend fraction operator*(fraction lhs, const fraction &rhs)
    {
        lhs *= rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }

    friend fraction operator/(fraction lhs, const fraction &rhs)
    {
        lhs /= rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }

    friend bool operator<(const fraction &lhs, const fraction &rhs)
    {
        if (lhs.denom != rhs.denom) {
            const int lcd = std::lcm(lhs.denom, rhs.denom);
            const int lhs_scale = lcd / lhs.denom;
            const int rhs_scale = lcd / rhs.denom;
            return lhs.num * lhs_scale < rhs.num * rhs_scale;
        }
        return lhs.num < rhs.num; // keep the same order
    }

    friend bool operator>(const fraction &lhs, const fraction &rhs)
    {
        return rhs < lhs;
    }

    friend bool operator<=(const fraction &lhs, const fraction &rhs)
    {
        return !(lhs > rhs);
    }

    friend bool operator>=(const fraction &lhs, const fraction &rhs)
    {
        return !(lhs < rhs);
    }

    friend bool operator==(const fraction &lhs, const fraction &rhs)
    {
        return lhs.num == rhs.num && lhs.denom == rhs.denom;
    }

    friend bool operator!=(const fraction &lhs, const fraction &rhs)
    {
        return !(lhs == rhs);
    }
};

}

template <>
struct std::hash<advt::xy_pos> {
    size_t operator()(const advt::xy_pos &k) const
    {
        // Compute individual hash values for fields
        // and combine them using XOR
        // and bit shifting:

        return hash<int>{}(k.x) ^ (hash<int>{}(k.y) << 1U);
    }
};
