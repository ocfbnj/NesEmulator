#ifndef OCFBNJ_PIXEL_ENGINE_SHADER_H
#define OCFBNJ_PIXEL_ENGINE_SHADER_H

#include <optional>
#include <string_view>

#include <glad/glad.h>

class Shader {
public:
    explicit Shader(std::string_view vertexFile = "", std::string_view fragmentFile = "");

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&&) = default;
    Shader& operator=(Shader&&) = default;

    ~Shader();

    void activate();
    GLuint id() const;

private:
    std::optional<GLuint> programId;
};

#endif // OCFBNJ_PIXEL_ENGINE_SHADER_H
