#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <stdio.h>
#include <math.h>
// #ifndef M_PI
// #define M_PI 3.14159265358979323846
// #endif

#define drawOneLine(x1, y1, x2, y2) \
    glBegin(GL_LINES); \
        glVertex2f(x1, y1); \
        glVertex2f(x2, y2); \
    glEnd();
    
static GLubyte frontIndices[] = {4,5,6,7};
static GLubyte rightIndices[] = {1,2,6,5};
static GLubyte bottomIndices[] = {0,1,5,4};
static GLubyte backIndices[] = {0,3,2, 1};
static GLubyte leftIndices[] = {0,4,7,3};
static GLubyte topIndices[] = {2,3,7,6};

//make 3d vertices for cube
// static GLfloat vertices[] = {
//     -1.0f, -1.0f, -1.0f, // 0
//      1.0f, -1.0f, -1.0f, // 1
//      1.0f,  1.0f, -1.0f, // 2
//     -1.0f,  1.0f, -1.0f, // 3
//     -1.0f, -1.0f,  1.0f, // 4
//      1.0f, -1.0f,  1.0f, // 5
//      1.0f,  1.0f,  1.0f, // 6
//     -1.0f,  1.0f,  1.0f  // 7
// };

//make 3d vertices for star
static GLfloat star[] = {
     0.0f,  1.0f, 0.0f, // 0
     1.0f,  1.0f, 0.0f, // 1
     2.0f,  0.5f, 0.0f, // 2
     2.0f, -1.0f, 0.0f, // 3
     0.0f,  0.0f, 0.0f, // 4
    -2.0f, -1.0f, 0.0f, // 5
    -1.0f,  0.0f, 0.0f, // 6
     2.0f,  0.5f, 0.0f  // 7
    -1.0f,  1.0f, 0.0f, // 8
};

/*x: 0.000000, y: 1.000000
x: -0.951057, y: 0.309017
x: -0.587785, y: -0.809017
x: 0.587785, y: -0.809017
x: 0.951057, y: 0.309017*/

// glVertex3f(0.0,0.2,0.0);
// glVertex3f(0.1,0.1,0.0);
// glVertex3f(0.2,0.05,0.0);
// glVertext3f(0.1,0.0,0.0);
// glVertex3f(0.2,-0.1,0.0);
// glVertex3f(0.0,0.0,0.0);
// glVertex3f(-0.2,-0.1,0.0);
// glVertex3f(-0.1,0.0,0.0);
// glVertex3f(-0.2,0.05,0.0);
// glVertex3f(-0.1,0.1,0.0);

static GLfloat colors[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f
};

// void reshape(int width, int height) {
//     glViewport(0, 0, width, height);
//     glLoadIdentity();
//     glOrtho(0.0, width, 0.0, height, -1.0, 1.0);
// }

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL Init failed: %s\n", SDL_GetError());
        return 1;
    }

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
    // reshape(w, h);


    // make viewport -2 to 2
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2.0, 2.0, -2.0, 2.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);

    
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
    float angle = 0.0f;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                running = 0;
            }
            // if (e.type == SDL_EVENT_WINDOW_RESIZED) {
            //     w = e.window.data1;
            //     h = e.window.data2;
            //     reshape(w, h);
            // }
            if (e.type == SDL_EVENT_KEY_DOWN) {
                if (e.key.key == SDLK_ESCAPE) {
                    running = 0;
                }
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //glTranslatef(-1.0f, 0.0f, 0.0f); // Move cube into view
        glRotatef(angle, 1.0f, 1.0f, 0.0f); // Rotate cube

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, star);

        // glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, frontIndices);
        // glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, rightIndices);
        // glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, bottomIndices);
        // glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, backIndices);
        // glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, leftIndices);
        // glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, topIndices);
       
        // Example using GL_LINE_LOOP
        //glDrawArrays(GL_LINE_LOOP, 0, 9); // Draw the star shape
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 5; ++i) {
            double angle1 = 0.5 * M_PI + 2 * M_PI * i / 5;
            double x1 = 1.0 * cos(angle1);
            double y1 = 1.0 * sin(angle1);

            double angle2 = 1.5 * M_PI + 2 * M_PI * ((i + 3) % 5) / 5;
            double x2 = 0.5 * cos(angle2);
            double y2 = 0.5 * sin(angle2);

            // Rainbow color for each point
            float t1 = (float)i / 5.0f;
            float t2 = (float)((i + 1) % 5) / 5.0f;
            // Simple HSV to RGB approximation for rainbow
            glColor3f(0.5f + 0.5f * cos(6.2831f * t1), 0.5f + 0.5f * cos(6.2831f * t1 + 2.094f), 0.5f + 0.5f * cos(6.2831f * t1 + 4.188f));
            glVertex3f(x1, y1, 0.0f);
            glColor3f(0.5f + 0.5f * cos(6.2831f * t2), 0.5f + 0.5f * cos(6.2831f * t2 + 2.094f), 0.5f + 0.5f * cos(6.2831f * t2 + 4.188f));
            glVertex3f(x2, y2, 0.0f);
        }
        glEnd();

        glDisableClientState(GL_VERTEX_ARRAY);

        angle += 0.0f; // Animate rotation
        
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
