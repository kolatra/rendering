#ifndef ENGINE_V29090909_ISHADER_HPP
#define ENGINE_V29090909_ISHADER_HPP

#include <string>

class IShader {
public:
    virtual ~IShader() = default;
    virtual void use() = 0;
    virtual void setBool(const std::string &name, bool value) = 0;
    virtual void setInt(const std::string &name, int value) = 0;
    virtual void setFloat(const std::string &name, float value) = 0;
};

#endif //ENGINE_V29090909_ISHADER_HPP
