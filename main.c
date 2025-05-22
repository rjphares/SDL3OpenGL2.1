#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <stdio.h>

#define drawOneLine(x1, y1, x2, y2) \
    glBegin(GL_LINES); \
        glVertex2f(x1, y1); \
        glVertex2f(x2, y2); \
    glEnd();

    static GLint vertices[] = {
        0, 25,
        100, 325,
        175, 25, 
        175, 390,
        0, 25,
        325, 390};

    static GLfloat colors[] = {
        1.0, 0.2, 0.2,
        0.2, 0.2, 1.0,
        0.8, 1.0, 0.2,
        0.75, 0.75, 0.75,
        0.35, 0.35, 0.35,
        0.5, 0.5, 0.5};

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glLoadIdentity();
    glOrtho(0.0, width, 0.0, height, -1.0, 1.0);
}

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL Init failed: %s\n", SDL_GetError());
        return 1;
    }

    // int w = 96*3, h = 96;
    int w = 400, h = 400;
    SDL_Window* window = SDL_CreateWindow("OpenGL 2.1 + SDL3 (C)",
                                          w, h,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        fprintf(stderr, "OpenGL context creation failed: %s\n", SDL_GetError());
        return 1;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return 1;
    }

    printf("OpenGL version: %s\n", glGetString(GL_VERSION));

    // Set the initial orthographic projection to match the window size
    reshape(w, h);

    // if (glUniformMatrix4x3fv) {
    //     printf("glUniformMatrix4x3fv is available. OpenGL 2.1 is supported.\n");
    // } else {
    //     printf("glUniformMatrix4x3fv is not available. OpenGL 2.1 is not supported.\n");
    // }

    // if (glSpecializeShader) {
    //     printf("glSpecializeShader is available. OpenGL 4.6 is supported.\n");
    // } else {
    //     printf("glSpecializeShader is not available. OpenGL 4.6 is not supported.\n");
    // }

    int running = 1;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                running = 0;
            }
            if (e.type == SDL_EVENT_WINDOW_RESIZED) {
                w = e.window.data1;
                h = e.window.data2;
                reshape(w, h);
            }
            if (e.type == SDL_EVENT_KEY_DOWN) {
                if (e.key.key == SDLK_ESCAPE) {
                    running = 0;
                }
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(0.0f, 0.0f, 0.0f);
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(2, GL_INT, 0, vertices);
        glColorPointer(3, GL_FLOAT, 0, colors);

        glBegin(GL_TRIANGLES);
        glArrayElement(2);
        glArrayElement(3);
        glArrayElement(5);
        glEnd();

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
