#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
    unsigned int program_ID;

    Shader(const char *vertex_shader_path, const char *fragment_shader_path);
    ~Shader();

    void use() const { glUseProgram(program_ID); }
    void use_shader() const { use(); } // Keep compatibility

    // Uniform setters
    void setFloat(const std::string &name, float value) const;
    void setVec4(const std::string &name, const glm::vec4& vec) const;
    void setInt(const std::string &name, int value) const;
    void setBool(const std::string &name, bool value) const;

private:
    std::string readShaderFile(const char *file_path);
    void addShader(unsigned int program, const char *shader_path, GLenum shader_type);
    bool checkCompileErrors(unsigned int shader, const std::string& type);
};
