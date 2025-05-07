#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <stdio.h>

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glLoadIdentity();
    glOrtho(0.0, width, 0.0, height, -1.0, 1.0);
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL Init failed: %s\n", SDL_GetError());
        return 1;
    }

    int w = 800, h = 800;;
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

    //change ortho to 0,0,800,600
    GLint mode;
    glGetIntegerv(GL_MATRIX_MODE, &mode);
    // Print the current matrix mode
    if (mode == GL_MODELVIEW) {
        printf("Current matrix mode: MODELVIEW\n");
    } else if (mode == GL_PROJECTION) {
        printf("Current matrix mode: PROJECTION\n");
    } else if (mode == GL_TEXTURE) {
        printf("Current matrix mode: TEXTURE\n");
    } else if (mode == GL_COLOR) {
        printf("Current matrix mode: COLOR\n");
    } else {
        printf("Unknown matrix mode: %d\n", mode);
    }

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
        }
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBegin(GL_TRIANGLES);
            glColor3f(1.0f, 0.0f, 0.0f);
            for (int i = 0; i <= w/100; i++) {
                glVertex2f(100.0*i, 0.0); // Bottom-left
                glVertex2f(100.0*i + 50.0, h); // Middle-top
                glVertex2f(100.0*i + 100, 0.0); // Bottom-right
            }
        glEnd();
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
