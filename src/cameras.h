#include "types.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>

struct Camera2D {
    vec2      _position                 = {0.f, 0.f};
    glm::mat4 _matrix                   = glm::mat4(1.0f);
    float     _zoom                     = 1.f;
    GLfloat   _screenWidth              = 820;
    GLfloat   _screenHeight             = 680;
    bool      _changed                  = false;
    glm::mat4 projection                = glm::mat4(1.0f);
    GLint     projectionUniformLocation = 0;

    void
    create(int width, int height, GLuint shaderProgram)
    {
        _screenWidth              = (float)width;
        _screenHeight             = (float)height;
        _matrix                   = glm::ortho(0.f, (float)width, (float)height, 0.f, -1.f, 1.0f);
        projectionUniformLocation = glGetUniformLocation(shaderProgram, "projection");
    };

    void update()
    {
        if (_changed) //TODO:
        {
            glm::vec3 scale(_zoom, _zoom, .0f);
            glm::vec3 translate(-_position.x, -_position.y, .0f);

            _matrix    = glm::scale(_matrix, scale);
            _matrix    = glm::translate(_matrix, translate);
            projection = _matrix;
            glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, glm::value_ptr(_matrix));
            _changed   = false;
        }
    };

    void move(vec2 vec)
    {
        _position   = {0.f, 0.f};
        _position.x = vec.x;
        _position.y = vec.y;
        _changed    = true;
    };
    void setPosition(vec2 vec)
    {
        _position.x = vec.x;
        _position.y = vec.y;
        _changed    = true;
    };
};