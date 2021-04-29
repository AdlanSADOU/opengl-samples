#pragma once

#include <array>
#include <gl/glew.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "types.h"

#define PI               3.14159265359f
#define PI_OVER_180      0.0174532925199432957692369076849f
#define HALFTURN_OVER_PI 57.2957795130823208767981548141f

#define DEG_TO_RAD(x) (x * PI_OVER_180)
#define RAD_TO_DEG(x) (x * 180_OVER_PI)

extern ShaderProgram shaderProgram;

class Color
{
    float r;
    float g;
    float b;
    float a;
};

struct Texture {
    GLuint _GPU_TextureId;

    int            _width         = 0;
    int            _height        = 0;
    int            _channelsCount = 0;
    unsigned char *_pixels        = nullptr;

    void create(const char *filepath)
    {
        // load image
        stbi_set_flip_vertically_on_load(true);
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

        float borderColor[] = {1.0f, 1.0f, 0.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        stbi_image_free(_pixels);
    };
};

struct Vertex {
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 TexCoords;
};

struct Sprite {
    FRect   _rect;
    float   _scaleFactor = 1.f;
    Color   color;
    Texture texture;
    GLuint  _VBO = 0;
    GLuint  _VAO = 0;

    std::array<GLfloat, 10> _vertsQuad;
    std::array<Vertex, 4>   _vertsQuadV;

    glm::mat4 _modelMatrix          = glm::mat4(1.0f);
    GLint     _modelUniformLocation = -1;

    bool _changed = false;

    void create(const char *filepath, FRect rect, GLuint shaderProgram)
    {
        _rect = rect;
        texture.create(filepath);

        /* vertex 1 */ _vertsQuadV[0].Position  = {_rect.x + rect.w, _rect.y, 0};
        /*          */ _vertsQuadV[0].Color     = {1, 1, 1, 1}; // alpha values have no effect
        /*          */ _vertsQuadV[0].TexCoords = {1, 1};

        /* vertex 2 */ _vertsQuadV[1].Position  = {_rect.x, _rect.y, 0};
        /*          */ _vertsQuadV[1].Color     = {1, 1, 1, 1};
        /*          */ _vertsQuadV[1].TexCoords = {1, 0};

        /* vertex 3 */ _vertsQuadV[2].Position  = {_rect.x, _rect.y + _rect.h, 0};
        /*          */ _vertsQuadV[2].Color     = {1, 1, 1, 1};
        /*          */ _vertsQuadV[2].TexCoords = {0, 0};

        /* vertex 4 */ _vertsQuadV[3].Position  = {_rect.x + rect.w, rect.y + _rect.h, 0};
        /*          */ _vertsQuadV[3].Color     = {1, 1, 1, 1};
        /*          */ _vertsQuadV[3].TexCoords = {0, 1};

        glGenVertexArrays(1, &_VAO);
        glBindVertexArray(_VAO);

        glGenBuffers(1, &_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferData(GL_ARRAY_BUFFER, _vertsQuadV.size() * 12 * sizeof(GLfloat), &_vertsQuadV[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(7 * sizeof(float)));
        glEnableVertexAttribArray(2);

        unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
        };

        unsigned int EBO;
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        //---------------------------------

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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture._GPU_TextureId);

        glUniformMatrix4fv(_modelUniformLocation, 1, GL_FALSE, glm::value_ptr(_modelMatrix));
        glBindVertexArray(_VAO); // just rebind this when we want to draw the above quad
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // if ((err = glGetError()) != GL_NO_ERROR) {
        //     printf("Error: 0x%x\n", err);
        // }
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

    void setColor()
    {
    }
};

class LineShape
{
  private:
    GLuint vao;
    GLuint vbo;
    GLuint posAttrib;
    bool   _changed = false;

    Vec2 points[2];

  public:
    ~LineShape(){};

    LineShape(Vec2 p1, Vec2 p2, float size = 2)
    {

        points[0] = p1;
        points[1] = p2;

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);

        posAttrib = glGetAttribLocation(shaderProgram._programId, "aPos");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

        glLineWidth(size);
    }

    void Draw()
    {
        GLenum err = GL_NO_ERROR;
        if (_changed) {

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), &points[0]);

            _changed = false;
        }
        glBindVertexArray(vao);
        glDrawArrays(GL_LINES, 0, 2);
    }
};

class Circle
{
  private:
    GLuint vao;
    GLuint vbo;
    GLuint posAttrib;
    bool   _changed = false;

    std::vector<Vec2> points;

  public:
    ~Circle(){};

    Circle(int x, int y, float r, float size = 2)
    {
        int count  = 0;
        int amount = 2;

        for (float i = -1; i < amount; i += 0.1f) {
            Vec2 p;

            p.x = r * cosf(i * (2 * PI) / amount) + x;
            p.y = r * sinf(i * (2 * PI) / amount) + y;

            points.push_back(p);
            ++count;
        }

        printf("n of points: %d\n", count);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(points[0]) * points.size(), points.data(), GL_STATIC_DRAW);

        posAttrib = glGetAttribLocation(shaderProgram._programId, "aPos");
        glEnableVertexAttribArray(posAttrib);
        glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

        glPointSize(size);
    }

    void
    Draw()
    {
        GLenum err = GL_NO_ERROR;
        if (_changed) {

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points[0]) * points.size(), points.data());

            _changed = false;
        }
        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_STRIP, 0, points.size());
    }
};
