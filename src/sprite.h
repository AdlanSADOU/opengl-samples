#pragma once

#include <array>
#include <gl/glew.h>

struct Sprite {
    float _x           = 0;
    float _y           = 0;
    float _width       = 1;
    float _height      = 1;
    float _scaleFactor = 1.f;

    GLuint _VBO = 0;
    GLuint _VAO = 0;

    std::array<GLfloat, 12> _vertsQuad;

    glm::mat4 _modelMatrix          = glm::mat4(1.0f);
    GLint     _modelUniformLocation = -1;

    bool _changed = false;

    void create(float x, float y, float width, float height, GLuint shaderProgram)
    {
        _x      = x;
        _y      = y;
        _width  = width;
        _height = height;

        _vertsQuad = {
            _x + _width, y + _height, // top left
            _x, _y + _height,         // bot left
            _x, _y,                   // top right
            _x, _y,                   // bot left
            _x + _width, _y,          // bot right
            _x + _width, _y + _height // top right
        };

        glGenVertexArrays(1, &_VAO);
        glBindVertexArray(_VAO);

        glGenBuffers(1, &_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferData(GL_ARRAY_BUFFER, _vertsQuad.size() * 6 * sizeof(GLfloat), &_vertsQuad[0], GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        // glBindVertexArray(0);

        _modelUniformLocation = glGetUniformLocation(shaderProgram, "model");
    };

    void draw()
    {
        GLenum err = GL_NO_ERROR;
        if (_changed) {

            glBindBuffer(GL_ARRAY_BUFFER, _VBO);
            glBufferData(GL_ARRAY_BUFFER, _vertsQuad.size() * 6 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, _vertsQuad.size() * 6 * sizeof(GLfloat), &_vertsQuad[0]);

            _changed = false;
        }

        glUniformMatrix4fv(_modelUniformLocation, 1, GL_FALSE, glm::value_ptr(_modelMatrix));
        glBindVertexArray(_VAO); // just rebind this when we want to draw the above quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        if ((err = glGetError()) != GL_NO_ERROR) {
            printf("Error: 0x%x\n", err);
        }
    };

    void move(float x, float y)
    {
        _x += x;
        _y += y;

        glm::vec3 translate(x, y, .0f);
        _modelMatrix = glm::translate(_modelMatrix, translate);

        _changed = true;
    }

    void setPosition(float x, float y)
    {
        _x = x;
        _y = y;

        glm::vec3 translate(x, y, .0f);

        _modelMatrix = glm::translate(_modelMatrix, translate);

        _changed = true;
    }

    void setScale(float scale)
    {
        _scaleFactor = scale;
        glm::vec3 scaleVec3(scale, scale, .0f);
        _modelMatrix = glm::scale(_modelMatrix, scaleVec3);
    }
};