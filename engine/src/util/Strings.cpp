#include "Utilities.hpp"

namespace Util {
    std::string ToLower(const std::string &str) {
        std::string out;
        for (auto c : str) {
            out.push_back(std::tolower(c));
        }
        return out;
    }
}