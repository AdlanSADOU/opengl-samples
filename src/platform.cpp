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

void GLAPIENTRY
MessageCallback(GLenum        source,
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
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 4);
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
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
    ShaderProgram shaderProgram;
    shaderProgram.attachShader(&vertShader);
    shaderProgram.attachShader(&fragShader);
    shaderProgram.compile();
    shaderProgram.bind();

    Camera2D camera;
    camera.create(WIDTH, HEIGHT, shaderProgram._programId);
    camera.setPosition(vec2(0, 0));

    Sprite sprite;
    Sprite otherSprite;

    sprite.create(100, 0, 10, 10, shaderProgram._programId);
    otherSprite.create(0, 0, 30, 50, shaderProgram._programId);

    sprite.setPosition(600, 600);

    //
    // Matrices Setup
    //
    glm::mat4 model       = glm::mat4(1.f);
    glm::mat4 view        = glm::mat4(1.0f);
    glm::mat4 translation = glm::mat4(1.0f);

    shaderProgram.setUniformMatrix4fv((char *)"view", view);
    shaderProgram.setUniformMatrix4fv((char *)"model", model);

    glClearColor(0.2, 0.2, 0.2, 1);

    //
    // Main Loop
    //
    float fps       = 0.f;
    float frameTime = 0.f;
    float time      = 0.f;
    float deltaTime = 0.f;

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

                //
                // Camera movement // Todo: obviously, this is rediculus, but enough for now
                //
                {
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

        //
        // Frames calc & limit
        //
        {
            static const int NUM_SAMPLES = 1000;
            float            frameTimes[NUM_SAMPLES];

            static int   currentFrame = 0.f;
            static float prevTicks    = SDL_GetTicks();

            float currentTicks = SDL_GetTicks();

            frameTime                              = currentTicks - prevTicks;
            frameTimes[currentFrame % NUM_SAMPLES] = frameTime;

            prevTicks = currentTicks;

            int count;

            currentFrame++;
            if (currentFrame < NUM_SAMPLES) {
                count = currentFrame;
            } else {
                count = NUM_SAMPLES;
            }

            float frameTimeAverage = 0;
            for (size_t i = 0; i < count; i++) {
                frameTimeAverage += frameTimes[i];
            }

            frameTimeAverage /= count;

            if (frameTimeAverage > 0) {
                fps = 1000.0f / frameTimeAverage;
            } else {
                fps = MAX_FPS;
            }

            static int frameCount = 0;
            ++frameCount;
            if (frameCount > 10) {
                // printf("%.2f\n", 1 - (frameTime / 100) + 1 - 1 - (frameTime / 100));
                frameCount = 0;
            }

            float frameTicks = SDL_GetTicks() - startTicks;
            if (1000.f / MAX_FPS > frameTicks) {
                if (running)
                    SDL_Delay(1000.f / MAX_FPS - frameTicks);
            }
            deltaTime = frameTime / 100;
        }

        static float cameraSpeed = 1;
        if (key_Left) {
            camera.move(vec2(-1.f * (cameraSpeed * deltaTime), 0.f));
        }
        if (key_Right) {
            camera.move(vec2((cameraSpeed * deltaTime), 0.f));
        }
        if (key_Up) {
            camera.move(vec2(0.f, -1.f * (cameraSpeed * deltaTime)));
        }
        if (key_Down) {
            camera.move(vec2(0.f, (cameraSpeed * deltaTime)));
        }

        static int speed = 10.f;
        if (key_W)
            sprite.move(0, -speed * deltaTime);
        if (key_A)
            sprite.move( -speed * deltaTime, 0);
        if (key_S)
            sprite.move(0, speed * deltaTime);
        if (key_D)
            sprite.move( speed * deltaTime, 0);

        camera.update();

        //
        // Render
        //
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        otherSprite.draw();
        sprite.draw();

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glcontext);
    return 0;
}