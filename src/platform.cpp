#if defined(_WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
#include <windows.h>
#endif
#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES      1

#define MATH_3D_IMPLEMENTATION

#include <GL/glew.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/vec3.hpp>   // glm::vec3
#include <glm/vec4.hpp>   // glm::vec4

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "cameras.h"
#include "shader.h"
#include "sprite.h"

#define PI_OVER_180      0.0174532925199432957692369076849f
#define HALFTURN_OVER_PI 57.2957795130823208767981548141f

#define DEG_TO_RAD(x) (x * PI_OVER_180)
#define RAD_TO_DEG(x) (x * 180_OVER_PI)

static bool      running       = 1;
static GLboolean should_rotate = GL_TRUE;
int              WIDTH         = 820;
int              HEIGHT        = 680;
int              MAX_FPS       = 60;

static GLfloat x        = 0;
static GLfloat y        = 0;
static GLfloat z        = -5.0;
static bool    key_Left = 0, key_Right = 0, key_Up = 0, key_Down = 0;
static bool    key_W = 0, key_A = 0, key_S = 0, key_D = 0;

ShaderProgram shaderProgram;

void GLAPIENTRY MessageCallback(GLenum        source,
                                GLenum        type,
                                GLuint        id,
                                GLenum        severity,
                                GLsizei       length,
                                const GLchar *message,
                                const void *  userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

//
//--------------------- Main
//
int main(int argc, char *argv[])
{
    int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Video initialization failed: %s\n",
                SDL_GetError());
        running = false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 6);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    glewExperimental = true;

    SDL_Window *  window    = SDL_CreateWindow("awesome", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, flags);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    // if (SDL_GL_SetSwapInterval(0) < 0) {
    //     printf("not supported!\n");
    // }

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW   %s\n", glewGetString(GLEW_VERSION));
    fprintf(stdout, "Status: Using OpenGL %s\n", glGetString(GL_VERSION));

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
    Shader vertShader;
    Shader fragShader;

    vertShader.create("src/vertexShader.vert", GL_VERTEX_SHADER);
    fragShader.create("src/fragmentShader.frag", GL_FRAGMENT_SHADER);

    // TODO:shaderprogram
    shaderProgram.attachShader(&vertShader);
    shaderProgram.attachShader(&fragShader);
    shaderProgram.compile();
    shaderProgram.bind();

    Camera2D camera;
    camera.create(WIDTH, HEIGHT, shaderProgram._programId);
    camera.setPosition(Vec2(0, 0));

    //
    // SCRATCH
    //
    Point mypoints(200, 200, 2);

    //
    // Matrices Setup
    //
    glm::mat4 model       = glm::mat4(1.f);
    glm::mat4 view        = glm::mat4(1.0f);
    glm::mat4 translation = glm::mat4(1.0f);

    shaderProgram.setUniformMatrix4fv((char *)"view", view);
    shaderProgram.setUniformMatrix4fv((char *)"model", model);

    GLint locI = glGetUniformLocation(shaderProgram._programId, "i");

    glClearColor(0.2, 0.2, 0.2, 1);

    glEnable(GL_BLEND); // you enable blending function
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //
    // Main Loop
    //

    while (running) {
        static float rotationAngle = 0.01f;
        float        startTicks    = SDL_GetTicks();

        SDL_Event event = {};
        while (SDL_PollEvent(&event)) {
            SDL_Keycode key = SDL_GetKeyFromScancode(event.key.keysym.scancode);

            switch (event.type) {
            case SDL_KEYDOWN: {
                if (key == SDL_GetKeyFromName("Escape")) {
                    running = false;
                    SDL_Quit();
                }
                if (key == SDL_GetKeyFromName("W")) {
                    key_W = true;
                }
                if (key == SDL_GetKeyFromName("A")) {
                    key_A = true;
                }
                if (key == SDL_GetKeyFromName("S")) {
                    key_S = true;
                }
                if (key == SDL_GetKeyFromName("D")) {
                    key_D = true;
                }

                if (key == SDL_GetKeyFromName("A")) {
                    rotationAngle -= 2.f;
                    printf("rotSpeed: %.2f\n", rotationAngle);
                }
                if (key == SDL_GetKeyFromName("E")) {
                    rotationAngle += 2.f;
                    printf("rotSpeed: %.2f\n", rotationAngle);
                }

                if (key == SDL_GetKeyFromName("Left")) {
                    key_Left = true;
                }
                if (key == SDL_GetKeyFromName("Right")) {
                    key_Right = true;
                }
                if (key == SDL_GetKeyFromName("Up")) {
                    key_Up = true;
                }
                if (key == SDL_GetKeyFromName("Down")) {
                    key_Down = true;
                }

            } break;
            case SDL_KEYUP: {
                {
                    if (key == SDL_GetKeyFromName("W")) {
                        key_W = false;
                    }
                    if (key == SDL_GetKeyFromName("A")) {
                        key_A = false;
                    }
                    if (key == SDL_GetKeyFromName("S")) {
                        key_S = false;
                    }
                    if (key == SDL_GetKeyFromName("D")) {
                        key_D = false;
                    }
                }

                if (key == SDL_GetKeyFromName("Left")) {
                    key_Left = false;
                }
                if (key == SDL_GetKeyFromName("Right")) {
                    key_Right = false;
                }
                if (key == SDL_GetKeyFromName("Up")) {
                    key_Up = false;
                }
                if (key == SDL_GetKeyFromName("Down")) {
                    key_Down = false;
                }

            } break;

            case SDL_QUIT: {
            } break;
            }
        }

        //
        // Update
        //

        camera.update();

        //
        // Render
        //
        static float i = 0;
        glUniform1f(locI, i++);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mypoints.Draw();

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glcontext);
    return 0;
}