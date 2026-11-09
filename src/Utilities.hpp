#pragma once
#include <fstream>
#include <random>
#include <sstream>

namespace Util {
    template <typename T>
    T Random(T min, T max) {
        std::random_device rd;
        std::uniform_real_distribution<T> distribution(min, max);
        return distribution(rd);
    }

    inline std::string ReadFileToString(std::string fileName) {
        std::ifstream file(fileName);
        if (!file.is_open()) {
            return fileName;
        }
        std::stringstream buf;
        buf << file.rdbuf();
        const std::string output = buf.str();
        file.close();
        return output;
    }
}
