#ifndef NESEMULATOR_SHADER_H
#define NESEMULATOR_SHADER_H

#include <string_view>
#include <optional>

#include <glad/glad.h>

class Shader {
public:
    Shader(std::string_view vertexFile, std::string_view fragmentFile);
    Shader(const Shader& other) = delete;
    Shader(Shader&& other) noexcept;
    ~Shader();

    void activate();
    GLuint id() const;

private:
    std::optional<GLuint> programId;
};

#endif // NESEMULATOR_SHADER_H
