#pragma once

#include <array>
#include <gl/glew.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "types.h"

struct Texture {
    GLuint _GPU_TextureId;

    int            _width         = 0;
    int            _height        = 0;
    int            _channelsCount = 0;
    unsigned char *_pixels        = nullptr;

    void create(const char *filepath)
    {
        // load image
        _pixels = stbi_load(filepath, &_width, &_height, &_channelsCount, 0);

        if (!_pixels) {
            printf("Error::Texture:: pixel data nullptr\n");
            return;
        }

        // Generate & Bind Texture buffer
        glGenBuffers(1, &_GPU_TextureId);
        glBindBuffer(GL_TEXTURE_2D, _GPU_TextureId);
        // get the image from ram to vram
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(_pixels);
    };
};

struct Vertex {
    glm::vec3 Position;
    // glm::vec2 TexCoords;
};

struct Sprite {
    FRect _rect;
    float _scaleFactor = 1.f;

    GLuint _VBO = 0;
    GLuint _VAO_pos = 0;
    GLuint _VAO_tex = 0;

    std::array<GLfloat, 10> _vertsQuad;
    std::array<Vertex, 4>   _vertsQuadV;

    glm::mat4 _modelMatrix          = glm::mat4(1.0f);
    GLint     _modelUniformLocation = -1;

    bool _changed = false;

    void create(const char *filepath, FRect rect, GLuint shaderProgram)
    {
        _rect = rect;

        _vertsQuadV[0].Position = {_rect.x + rect.w, _rect.y, 0};
        _vertsQuadV[1].Position = {_rect.x, _rect.y, 0};
        _vertsQuadV[2].Position = {_rect.x, _rect.y + _rect.h, 0};
        _vertsQuadV[3].Position = {_rect.x + rect.w, rect.y + _rect.h, 0};

        glGenVertexArrays(1, &_VAO_pos);
        glBindVertexArray(_VAO_pos);

        glGenBuffers(1, &_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferData(GL_ARRAY_BUFFER, _vertsQuadV.size() * 4 * sizeof(GLfloat), &_vertsQuadV[0], GL_DYNAMIC_DRAW);

        unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        unsigned int EBO;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
        //---------------------------------

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);




        _modelUniformLocation = glGetUniformLocation(shaderProgram, "model");
    };

    void draw()
    {
        GLenum err = GL_NO_ERROR;
        if (_changed) {

            glBindBuffer(GL_ARRAY_BUFFER, _VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, _vertsQuadV.size() * 4 * sizeof(GLfloat), &_vertsQuadV[0]);

            _changed = false;
        }

        glUniformMatrix4fv(_modelUniformLocation, 1, GL_FALSE, glm::value_ptr(_modelMatrix));
        glBindVertexArray(_VAO_pos); // just rebind this when we want to draw the above quad
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        if ((err = glGetError()) != GL_NO_ERROR) {
            printf("Error: 0x%x\n", err);
        }
    };

    void move(float x, float y)
    {
        _rect.x += x;
        _rect.y += y;

        glm::vec3 translate(x, y, .0f);
        _modelMatrix = glm::translate(_modelMatrix, translate);

        _changed = true;
    }

    void setPosition(float x, float y)
    {
        _rect.x = x;
        _rect.y = y;

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