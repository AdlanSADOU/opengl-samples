#pragma once

#include <GL/glew.h>

#include <fstream>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <sstream>
#include <string>

struct Shader {
    const char * _sourcePath = 0;
    unsigned int _ID         = 0;
    std::string  source      = {};

    unsigned int create(const char *sourcePath, int TYPE)
    {
        _sourcePath = sourcePath;

        std::ifstream shaderFile;
        shaderFile.open(sourcePath);

        std::stringstream fileStream;

        fileStream << shaderFile.rdbuf();
        shaderFile.close();

        source = fileStream.str();
        if (shaderFile.fail()) {
            printf("Could not read: %s\n", sourcePath);
            system("pause");
        }

        const GLchar *shaderSource = source.c_str();
        printf(shaderSource);

        _ID = glCreateShader(TYPE);
        glShaderSource(_ID, 1, &shaderSource, NULL);
        glCompileShader(_ID);

        int  success;
        char infoLog[512];
        glGetShaderiv(_ID, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(_ID, 512, NULL, infoLog);
            printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED: %s\n", infoLog);
            system("pause");
        }

        return _ID;
    };
};

struct ShaderProgram {
    unsigned int          _programId = 0;
    std::vector<Shader *> _shaders   = {};

    void attachShader(Shader *shader)
    {
        if (shader == NULL) {
            printf("Error::ShaderProgram: Shader is null\n");
            return;
        }
        _shaders.push_back(shader);
    }

    bool compile()
    {
        int  success = 0;
        char infoLog[512];

        _programId = glCreateProgram();

        for (auto &shader : _shaders) {
            glAttachShader(_programId, shader->_ID);
        }

        glLinkProgram(_programId);

        glGetProgramiv(_programId, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(_programId, 512, NULL, infoLog);
            printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED: %s\n", infoLog);
            system("pause");
            return 0;
        }
        return 1;
    }

    void bind()
    {
        glUseProgram(_programId);
    }

    void unbind()
    {
        glUseProgram(0);
    }

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const
    {
    }
    void setUniformMatrix4fv(char *name, glm::mat4 matrix)
    {
        GLint location = getUniformLocation(name);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    GLint getUniformLocation(char *name)
    {
        return glGetUniformLocation(_programId, name);
    }
};