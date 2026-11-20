#include <Utilities.hpp>
#include <fstream>
#include <sstream>

namespace Util {
    std::string ReadFileToString(std::string fileName) {
        std::ifstream file(fileName);
        if (!file.is_open()) {
            throw std::runtime_error(std::format("Could not open file {}", fileName));
        }
        std::stringstream buf;
        buf << file.rdbuf();
        const std::string output = buf.str();
        file.close();
        return output;
    }
}
