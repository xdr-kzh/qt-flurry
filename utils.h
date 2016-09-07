#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace utils
{
    std::string adler32(const std::string& _input)
    {
        int d = 1;
        long long b = 0;
        int e = 0;
        int c = 0;
        int j = 0;
        for (j = (int)_input.size(); 0 <= j ? e < j : e > j; c = 0 <= j ? ++e : --e)
            c = _input[c], d += c, b += d;
        d %= 65521;
        b %= 65521;
        auto result = b * 65536 + d;
        return std::to_string(result);
    }
}

#endif // UTILS_H
