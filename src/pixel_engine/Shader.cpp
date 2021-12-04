#include <fstream>
#include <string>

#include "Shader.h"

static std::string readFile(std::string_view fileName) {
    std::ifstream is{fileName.data(), std::ifstream::binary};
    is.seekg(0, std::ifstream::end);

    std::string contents(is.tellg(), '\0');

    is.seekg(0, std::ifstream::beg);
    is.read(contents.data(), contents.size());

    return contents;
}

Shader::Shader(std::string_view vertexFile, std::string_view fragmentFile) {
    std::string vertexCode = readFile(vertexFile);
    const char* vertexSource = vertexCode.data();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    std::string fragmentCode = readFile(fragmentFile);
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
