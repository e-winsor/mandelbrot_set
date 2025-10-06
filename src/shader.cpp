#include "shader.hpp"
#include <vector>

Shader::Shader(const char *vertex_shader_path, const char *fragment_shader_path) : program_ID(0) {
    program_ID = glCreateProgram();
    addShader(program_ID, vertex_shader_path, GL_VERTEX_SHADER);
    addShader(program_ID, fragment_shader_path, GL_FRAGMENT_SHADER);
    glLinkProgram(program_ID);

    int success;
    glGetProgramiv(program_ID, GL_LINK_STATUS, &success);
    if (!success) {
        char error_message[512];
        glGetProgramInfoLog(program_ID, 512, nullptr, error_message);
        std::cout << "Error linking shader program: " << error_message << "\n";
    }
}

Shader::~Shader() {
    if (program_ID != 0) {
        GLint count;
        glGetProgramiv(program_ID, GL_ATTACHED_SHADERS, &count);
        std::vector<GLuint> shaders(count);
        glGetAttachedShaders(program_ID, count, nullptr, shaders.data());

        for (GLuint shader:shaders)
        {
            glDetachShader(program_ID, shader);
            glDeleteShader(shader);
        }

        glDeleteProgram(program_ID);
        program_ID = 0;
    }
}

std::string Shader::readShaderFile(const char *file_path) {
    std::ifstream shader_file(file_path);
    if (!shader_file.is_open()) {
        std::cout << "Failed to open shader file: " << file_path << "\n";
        return "";
    }

    std::stringstream buffer;
    buffer << shader_file.rdbuf();
    return buffer.str();
}

void Shader::addShader(unsigned int program, const char *shader_path, GLenum shader_type) {
    std::string shader_code = readShaderFile(shader_path);
    if (shader_code.empty()) return;

    const char* code = shader_code.c_str();
    unsigned int shader = glCreateShader(shader_type);

    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char error_message[512];
        glGetShaderInfoLog(shader, 512, nullptr, error_message);
        std::cout << "Error compiling shader at " << shader_path << ":\n" << error_message << "\n";
    }

    glAttachShader(program, shader);
    glDeleteShader(shader); // Mark for deletion
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(program_ID, name.c_str()), value);
}

void Shader::setVec4(const std::string &name, const glm::vec4& vec) const {
    glUniform4f(glGetUniformLocation(program_ID, name.c_str()), vec.x, vec.y, vec.z, vec.w);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(program_ID, name.c_str()), value);
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(program_ID, name.c_str()), (int)value);
}
