#pragma once
#include <string>
#include <glm/gtc/type_ptr.hpp>

namespace UAV
{
class Shader
{
public:
    unsigned ID; // Shader program ID

    Shader(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader
    void use();

    // utility
    void setBool(const std::string &name, const bool value) const;
    void setInt(const std::string &name, const int value) const;
    void setUnsigned(const std::string &name, const unsigned value) const;
    void setFloat(const std::string &name, const float value) const;

    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;

    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat2(const std::string &name, const glm::mat2 &mat) const;

private:
    void checkCompileErrors(unsigned int shader, std::string type);
};
}