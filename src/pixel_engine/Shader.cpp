#include <fstream>
#include <string>

#include "Shader.h"

constexpr static auto DefaultVertexShader = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;

void main() {
   gl_Position = vec4(aPos.x, aPos.y, 1.0, 1.0);
   texCoord = aTexCoord;
}
)";

constexpr static auto DefaultFragmentShader = R"(
#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D tex0;

void main(){
   FragColor = texture(tex0, texCoord);
}
)";

static std::string readFile(std::string_view fileName) {
    std::ifstream is{fileName.data(), std::ifstream::binary};
    is.seekg(0, std::ifstream::end);

    std::string contents(is.tellg(), '\0');

    is.seekg(0, std::ifstream::beg);
    is.read(contents.data(), contents.size());

    return contents;
}

Shader::Shader(std::string_view vertexFile, std::string_view fragmentFile) {
    std::string vertexCode = vertexFile.empty() ? DefaultVertexShader : readFile(vertexFile);
    const char* vertexSource = vertexCode.data();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    std::string fragmentCode = fragmentFile.empty() ? DefaultFragmentShader : readFile(fragmentFile);
    const char* fragmentSource = fragmentCode.data();
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint id = glCreateProgram();
    programId = id;

    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::Shader(Shader&& other) noexcept {
    programId = other.programId;
    other.programId.reset();
}

Shader::~Shader() {
    if (programId.has_value()) {
        glDeleteProgram(programId.value());
    }
}

void Shader::activate() {
    if (programId.has_value()) {
        glUseProgram(programId.value());
    }
}

GLuint Shader::id() const {
    return programId.value();
}
