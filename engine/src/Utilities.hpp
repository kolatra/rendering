#pragma once
#include <random>

#include <fmt/core.h>

namespace Util {
    template <typename T>
    T Random(T min, T max) {
        std::random_device rd;
        std::uniform_real_distribution<T> distribution(min, max);
        return distribution(rd);
    }

    std::string ReadFileToString(std::string fileName);
    std::string ToLower(const std::string &str);
}
