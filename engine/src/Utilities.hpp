#pragma once
#include <random>

namespace Util {
    template <typename T>
    T Random(T min, T max) {
        std::random_device rd;
        std::uniform_real_distribution<T> distribution(min, max);
        return distribution(rd);
    }

    std::string ReadFileToString(std::string fileName);
}
