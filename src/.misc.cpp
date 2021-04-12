
#include <GL/glew.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>

//
// Frames calc & limit
//
void frames()
{
    float fps       = 0.f;
    float frameTime = 0.f;
    float time      = 0.f;
    float deltaTime = 0.f;

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

    // if (frameTimeAverage > 0) {
    //     fps = 1000.0f / frameTimeAverage;
    // } else {
    //     fps = MAX_FPS;
    // }

    // static int frameCount = 0;
    // ++frameCount;
    // if (frameCount > 10) {
    //     frameCount = 0;
    // }

    // float frameTicks = SDL_GetTicks() - startTicks;
    // if (1000.f / MAX_FPS > frameTicks) {
    //     if (running)
    //         SDL_Delay(1000.f / MAX_FPS - frameTicks);
    // }
    // deltaTime = frameTime / 100;
}

static void renderCube(SDL_Window *window)
{
    /* Our angle of rotation. */

    /*
     * EXERCISE:
     * Replace this awful mess with vertex
     * arrays and a call to glDrawElements.
     *
     * EXERCISE:
     * After completing the above, change
     * it to use compiled vertex arrays.
     *
     * EXERCISE:
     * Verify my windings are correct here ;).
     */
    static GLfloat v0[] = {-1.0f, -1.0f, 1.0f};
    static GLfloat v1[] = {1.0f, -1.0f, 1.0f};
    static GLfloat v2[] = {1.0f, 1.0f, 1.0f};
    static GLfloat v3[] = {-1.0f, 1.0f, 1.0f};
    static GLfloat v4[] = {-1.0f, -1.0f, -1.0f};
    static GLfloat v5[] = {1.0f, -1.0f, -1.0f};
    static GLfloat v6[] = {1.0f, 1.0f, -1.0f};
    static GLfloat v7[] = {-1.0f, 1.0f, -1.0f};

    static GLubyte red[]    = {255, 0, 0, 255};
    static GLubyte green[]  = {0, 255, 0, 255};
    static GLubyte blue[]   = {0, 0, 255, 255};
    static GLubyte white[]  = {255, 255, 255, 255};
    static GLubyte yellow[] = {0, 255, 255, 255};
    static GLubyte black[]  = {0, 0, 0, 255};
    static GLubyte orange[] = {255, 255, 0, 255};
    static GLubyte purple[] = {255, 0, 255, 0};

    glBegin(GL_TRIANGLES);

    glColor4ubv(red);
    glVertex3fv(v0);
    glColor4ubv(green);
    glVertex3fv(v1);
    glColor4ubv(blue);
    glVertex3fv(v2);

    glColor4ubv(red);
    glVertex3fv(v0);
    glColor4ubv(blue);
    glVertex3fv(v2);
    glColor4ubv(white);
    glVertex3fv(v3);

    glColor4ubv(green);
    glVertex3fv(v1);
    glColor4ubv(black);
    glVertex3fv(v5);
    glColor4ubv(orange);
    glVertex3fv(v6);

    glColor4ubv(green);
    glVertex3fv(v1);
    glColor4ubv(orange);
    glVertex3fv(v6);
    glColor4ubv(blue);
    glVertex3fv(v2);

    glColor4ubv(black);
    glVertex3fv(v5);
    glColor4ubv(yellow);
    glVertex3fv(v4);
    glColor4ubv(purple);
    glVertex3fv(v7);

    glColor4ubv(black);
    glVertex3fv(v5);
    glColor4ubv(purple);
    glVertex3fv(v7);
    glColor4ubv(orange);
    glVertex3fv(v6);

    glColor4ubv(yellow);
    glVertex3fv(v4);
    glColor4ubv(red);
    glVertex3fv(v0);
    glColor4ubv(white);
    glVertex3fv(v3);

    glColor4ubv(yellow);
    glVertex3fv(v4);
    glColor4ubv(white);
    glVertex3fv(v3);
    glColor4ubv(purple);
    glVertex3fv(v7);

    glColor4ubv(white);
    glVertex3fv(v3);
    glColor4ubv(blue);
    glVertex3fv(v2);
    glColor4ubv(orange);
    glVertex3fv(v6);

    glColor4ubv(white);
    glVertex3fv(v3);
    glColor4ubv(orange);
    glVertex3fv(v6);
    glColor4ubv(purple);
    glVertex3fv(v7);

    glColor4ubv(green);
    glVertex3fv(v1);
    glColor4ubv(red);
    glVertex3fv(v0);
    glColor4ubv(yellow);
    glVertex3fv(v4);

    glColor4ubv(green);
    glVertex3fv(v1);
    glColor4ubv(yellow);
    glVertex3fv(v4);
    glColor4ubv(black);
    glVertex3fv(v5);

    glEnd();
}

static void renderTriangle()
{

    // fill "indices" as needed

    // Generate a buffer for the indices
    // GLfloat vertices[] = {};             // 8 of vertex coords
    // GLubyte indices[]  = {};

    static GLfloat v0[] = {-1.0f, 1.0f, 1.0f};
    static GLfloat v1[] = {-1.0f, -1.0f, 1.0f};
    static GLfloat v2[] = {1.0f, 1.0f, 1.0f};

    static GLfloat v3[] = {-1.0f, -1.0f, 1.0f};
    static GLfloat v4[] = {1.0f, -1.0f, 1.0f};
    static GLfloat v5[] = {1.0f, 1.0f, 1.0f};

    static GLubyte red[]   = {255, 0, 0, 255};
    static GLubyte green[] = {0, 255, 0, 255};
    static GLubyte blue[]  = {0, 0, 255, 255};

    glBegin(GL_TRIANGLES);

    glColor4ubv(red);
    glVertex3fv(v0);

    glColor4ubv(green);
    glVertex3fv(v1);

    glColor4ubv(blue);
    glVertex3fv(v2);

    glColor4ubv(green);
    glVertex3fv(v3);

    glColor4ubv(red);
    glVertex3fv(v4);

    glColor4ubv(blue);
    glVertex3fv(v5);

    glEnd();
}

void render()

{ // Render
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    // glTranslatef(x, y, -10);
    // glRotatef(rotationAngle, 0.0, 1.0, 0.0);
    renderTriangle();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1, -3, -10);
    // renderCube(window);
    glPopMatrix();
}