#include <SDL3/SDL.h>
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define drawOneLine(x1, y1, x2, y2) \
    glBegin(GL_LINES); \
        glVertex2f(x1, y1); \
        glVertex2f(x2, y2); \
    glEnd();
    

#define NUM_STARS 300

typedef struct {
    float x, y, z;
} Star;

Star stars[NUM_STARS];

void initStars() {
    for (int i = 0; i < NUM_STARS; ++i) {
        // Wider X/Y range for perspective, and positive Z values (since camera looks down -Z)
        stars[i].x = ((float)rand() / RAND_MAX - 0.5f) * 4.0f; // -2 to 2
        stars[i].y = ((float)rand() / RAND_MAX - 0.5f) * 4.0f; // -2 to 2
        stars[i].z = -((float)rand() / RAND_MAX * 48.0f + 2.0f); // -2 (near) to -50 (far)
    }
}

void updateStars(float speed) {
    for (int i = 0; i < NUM_STARS; ++i) {
        stars[i].z += speed; // Move toward camera (less negative)
        if (stars[i].z > -2.0f) { // If past near plane
            stars[i].x = ((float)rand() / RAND_MAX - 0.5f) * 4.0f;
            stars[i].y = ((float)rand() / RAND_MAX - 0.5f) * 4.0f;
            stars[i].z = -50.0f;
        }
    }
}

void drawStars() {
    glBegin(GL_POINTS);
    for (int i = 0; i < NUM_STARS; ++i) {
        float brightness = 1.0f - (stars[i].z / -50.0f);
        glColor3f(brightness, brightness, brightness);
        glVertex3f(stars[i].x, stars[i].y, stars[i].z);
    }
    glEnd();
}

#define STAR_POINTS 10
#define STAR_RADIUS_OUTER 1.0f
#define STAR_RADIUS_INNER 0.5f
#define STAR_DEPTH 0.2f

void draw3DStar(float x_center, float y_center, float z_center) {
    float front_z = z_center + STAR_DEPTH / 2.0f;
    float back_z  = z_center - STAR_DEPTH / 2.0f;

    // --- Draw front face ---
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(x_center, y_center, front_z);
    for (int i = 0; i <= STAR_POINTS; ++i) {
        int idx = i % STAR_POINTS;
        float radius = (idx % 2 == 0) ? STAR_RADIUS_OUTER : STAR_RADIUS_INNER;
        float angle = 0.5f * (float)M_PI + 2.0f * (float)M_PI * idx / STAR_POINTS;
        float x = x_center + radius * cosf(angle);
        float y = y_center + radius * sinf(angle);
        glColor3f(1.0f, 0.5f, 0.0f);
        glVertex3f(x, y, front_z);
    }
    glEnd();

    // --- Draw back face ---
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(x_center, y_center, back_z);
    for (int i = 0; i <= STAR_POINTS; ++i) {
        int idx = i % STAR_POINTS;
        float radius = (idx % 2 == 0) ? STAR_RADIUS_OUTER : STAR_RADIUS_INNER;
        float angle = 0.5f * (float)M_PI + 2.0f * (float)M_PI * idx / STAR_POINTS;
        float x = x_center + radius * cosf(angle);
        float y = y_center + radius * sinf(angle);
        glColor3f(1.0f, 0.5f, 0.0f);
        glVertex3f(x, y, back_z);
    }
    glEnd();

    // --- Draw sides ---
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= STAR_POINTS; ++i) {
        int idx = i % STAR_POINTS;
        float radius = (idx % 2 == 0) ? STAR_RADIUS_OUTER : STAR_RADIUS_INNER;
        float angle = 0.5f * (float)M_PI + 2.0f * (float)M_PI * idx / STAR_POINTS;
        float x = x_center + radius * cosf(angle);
        float y = y_center + radius * sinf(angle);
        glColor3f(1.0f, 0.7f, 0.2f);
        glVertex3f(x, y, front_z);
        glVertex3f(x, y, back_z);
    }
    glEnd();
}

#define PIPE_SEGMENTS 64
//#define PIPE_LENGTH 3.0f
#define PIPE_RADIUS_OUTER 0.5f
//#define PIPE_RADIUS_INNER 0.3f

void drawRainbowPipe(float z_start, float z_end, int numSegments, float gapFraction) {
    glEnable(GL_BLEND); // Enable blending for transparency

    float totalLength = z_end - z_start;
    float segLength = totalLength / numSegments;
    float dashLength = segLength * (1.0f - gapFraction);
    float gapHalf = segLength * gapFraction * 0.5f;

    for (int s = 0; s < numSegments; ++s) {
        float segStart, segEnd;

        if (s == 0) {
            // First segment: gap only on the right
            segStart = z_start;
            segEnd = z_start + dashLength + gapHalf;
        } else if (s == numSegments - 1) {
            // Last segment: gap only on the left
            segStart = z_end - dashLength - gapHalf;
            segEnd = z_end;
        } else {
            // Middle segments: gap/2 on both sides
            float center = z_start + s * segLength + segLength * 0.5f;
            segStart = center - dashLength * 0.5f;
            segEnd = center + dashLength * 0.5f;
        }

        if (segEnd > z_end) segEnd = z_end;
        if (segStart < z_start) segStart = z_start;
        if (segEnd <= segStart) continue;

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= PIPE_SEGMENTS; ++i) {
            float t = (float)i / PIPE_SEGMENTS;
            float angle = t * 2.0f * (float)M_PI;
            float x = cosf(angle);
            float y = sinf(angle);

            glColor4f(0.2f, 0.8f, 1.0f, 0.5f); // Bright cyan, 50% transparent
            glVertex3f(x * PIPE_RADIUS_OUTER, y * PIPE_RADIUS_OUTER, segStart);
            glVertex3f(x * PIPE_RADIUS_OUTER, y * PIPE_RADIUS_OUTER, segEnd);
        }
        glEnd();
    }

    glDisable(GL_BLEND); // Optionally disable blending after drawing
}

void drawPipeBetweenStars(float x1, float y1, float z1, float x2, float y2, float z2) {
    // Compute direction vector
    float dx = x2 - x1;
    float dy = y2 - y1;
    float dz = z2 - z1;
    float length = sqrtf(dx*dx + dy*dy + dz*dz);

    if (length > 0.0001f) {
        // Offset amount (from star center to surface)
        float offset = STAR_RADIUS_OUTER;

        // Normalize direction
        float dir[3] = {dx/length, dy/length, dz/length};

        float sx = x1;
        float sy = y1;
        float sz = z1;
        float ex = x2;
        float ey = y2;
        float ez = z2;

        // New length for the pipe
        float pipeLength = sqrtf((ex-sx)*(ex-sx) + (ey-sy)*(ey-sy) + (ez-sz)*(ez-sz));

        // Find rotation axis (cross product of (0,0,1) and dir)
        float rx = -dir[1];
        float ry = dir[0];
        float rz = 0.0f;
        float axis_len = sqrtf(rx*rx + ry*ry);
        float angle = 0.0f;
        if (axis_len > 0.0001f) {
            rx /= axis_len;
            ry /= axis_len;
            angle = acosf(dir[2]) * 180.0f / (float)M_PI;
        } else {
            rx = 1.0f; ry = 0.0f;
            angle = 0.0f;
        }

        glPushMatrix();
        glTranslatef(sx, sy, sz);
        glRotatef(angle, rx, ry, rz);
        drawRainbowPipe(0.0f, pipeLength, fabsf(z1-z2), 0.5f);
        glPopMatrix();
    }
}

float camX = 0.0f, camY = 0.0f, camZ = -10.0f;
float yaw = 0.0f, pitch = 0.0f;
float moveSpeed = 0.1f, mouseSensitivity = 0.2f;
int lastMouseX = 0, lastMouseY = 0;
int mouseCaptured = 0;

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL Init failed: %s\n", SDL_GetError());
        return 1;
    }

    int w = 1200, h = 600;
    SDL_Window* window = SDL_CreateWindow("OpenGL 2.1 + SDL3 (C)",
                                          w, h,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // Try 4 or 8 for higher quality

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

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // adjust frustum to handle perspective
    float aspect = (float)w / (float)h;
    float frustumHeight = 1.0f;
    float frustumWidth = frustumHeight * aspect;
    glFrustum(-frustumWidth, frustumWidth, -frustumHeight, frustumHeight, 2.0, 50.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    
    initStars();

    int running = 1;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                running = 0;
            }
            //if (e.type == SDL_EVENT_WINDOW_RESIZED) {
            //     w = e.window.data1;
            //     h = e.window.data2;
            //     reshape(w, h);
            // }
            if (e.type == SDL_EVENT_KEY_DOWN) {
                if (e.key.key == SDLK_ESCAPE) {
                    running = 0;
                }
                if (e.key.key == SDLK_W) { // Forward
                    camX -= moveSpeed * sinf(yaw * M_PI / 180.0f);
                    camZ -= moveSpeed * cosf(yaw * M_PI / 180.0f);
                }
                if (e.key.key == SDLK_S) { // Backward
                    camX += moveSpeed * sinf(yaw * M_PI / 180.0f);
                    camZ += moveSpeed * cosf(yaw * M_PI / 180.0f);
                }
                if (e.key.key == SDLK_A) { // Left
                    camX -= moveSpeed * cosf(yaw * M_PI / 180.0f);
                    camZ += moveSpeed * sinf(yaw * M_PI / 180.0f);
                }
                if (e.key.key == SDLK_D) { // Right
                    camX += moveSpeed * cosf(yaw * M_PI / 180.0f);
                    camZ -= moveSpeed * sinf(yaw * M_PI / 180.0f);
                }
                if (e.key.key == SDLK_SPACE) camY += moveSpeed; // Up
                if (e.key.key == SDLK_LCTRL) camY -= moveSpeed; // Down
            }
            if (e.type == SDL_EVENT_MOUSE_MOTION && mouseCaptured) {
                int dx = e.motion.xrel;
                int dy = e.motion.yrel;
                yaw   += dx * mouseSensitivity;
                pitch += dy * mouseSensitivity;
                if (pitch > 89.0f) pitch = 89.0f;
                if (pitch < -89.0f) pitch = -89.0f;
            }
            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                SDL_SetWindowRelativeMouseMode(window, true); // Capture mouse
                mouseCaptured = 1;
            }
            if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                SDL_SetWindowRelativeMouseMode(window, false); // Release mouse
                mouseCaptured = 0;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(pitch, 1.0f, 0.0f, 0.0f);
        glRotatef(yaw,   0.0f, 1.0f, 0.0f);
        glTranslatef(-camX, -camY, -camZ);
        
        glPointSize(3.0f);
        updateStars(0.15f); drawStars();
        
        draw3DStar(0.0f, 0.0f, -10.0f); // Draw at local z=0
        draw3DStar(0.0f, 0.0f, -25.0f); // Draw at local z=0
        
        drawPipeBetweenStars(0.0f, 0.0f, -25.0f, 0.0f, 0.0f, -10.0f);

        glDisableClientState(GL_VERTEX_ARRAY);

        SDL_GL_SwapWindow(window);
        static int iPrevTime = 0;
        int iTime = SDL_GetTicks(); //returns ticks in milliseconds
        if (abs(iTime - iPrevTime) < 16) {
            SDL_Delay(16 - (iTime - iPrevTime)); // Limit to ~60 FPS
        }
        iPrevTime = iTime;

    }

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
