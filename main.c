#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define drawOneLine(x1, y1, x2, y2) \
    glBegin(GL_LINES); \
        glVertex2f(x1, y1); \
        glVertex2f(x2, y2); \
    glEnd();
    

#define NUM_STARS 600

typedef struct {
    float x, y, z;
} Star;

Star stars[NUM_STARS];

void initStars() {
    for (int i = 0; i < NUM_STARS; ++i) {
        // Wider X/Y range for perspective, and positive Z values (since camera looks down -Z)
        stars[i].x = ((float)rand() / RAND_MAX - 0.5f) * 32.0f; // -16 to 16
        stars[i].y = ((float)rand() / RAND_MAX - 0.5f) * 32.0f; // -16 to 16
        stars[i].z = (((float)rand()/RAND_MAX) * 32.0f - 16.0f); // -2 (near) to -50 (far)
    }
}

void updateStars(float speed) {
    for (int i = 0; i < NUM_STARS; ++i) {
        stars[i].z += speed; // Move toward camera (less negative)
        if (stars[i].z > 16.0f) { // If past near plane
            stars[i].x = ((float)rand() / RAND_MAX - 0.5f) * 32.0f;
            stars[i].y = ((float)rand() / RAND_MAX - 0.5f) * 32.0f;
            stars[i].z = -16.0f;
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
#define PIPE_RADIUS_OUTER 0.5
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
            //segStart = z_start;
            //segEnd = z_start + dashLength + gapHalf;
            continue; // Skip first segment for now
        } else if (s == numSegments - 1) {
            // Last segment: gap only on the left
            //segStart = z_end - dashLength - gapHalf;
            //segEnd = z_end;
            continue; // Skip last segment for now
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
        drawRainbowPipe(0.0f, pipeLength, pipeLength, 0.5f);
        glPopMatrix();
    }
}

float camX = 0.0f, camY = 0.0f, camZ = -10.0f;
float yaw = 0.0f, pitch = 0.0f;
float moveSpeed = 0.1f, mouseSensitivity = 0.2f;
int lastMouseX = 0, lastMouseY = 0;
int mouseCaptured = 0;

void drawCube(float sx, float sy, float sz) {
    // Draw a cube centered at the origin, scaled by sx, sy, sz
    glPushMatrix();
    glScalef(sx, sy, sz);
    glBegin(GL_QUADS);
    // Front
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f( 0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    // Back
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f,  0.5f, -0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    // Left
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f,  0.5f);
    glVertex3f(-0.5f,  0.5f, -0.5f);
    // Right
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f,  0.5f);
    glVertex3f(0.5f,  0.5f,  0.5f);
    glVertex3f(0.5f,  0.5f, -0.5f);
    // Top
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f( 0.5f, 0.5f, -0.5f);
    glVertex3f( 0.5f, 0.5f,  0.5f);
    glVertex3f(-0.5f, 0.5f,  0.5f);
    // Bottom
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f, -0.5f);
    glVertex3f( 0.5f, -0.5f,  0.5f);
    glVertex3f(-0.5f, -0.5f,  0.5f);
    glEnd();
    glPopMatrix();
}

void drawBlockyPerson(float x, float y, float z, float scale) {
    glPushMatrix();
    glTranslatef(x, y, z);

    // Torso
    glColor3f(0.2f, 0.2f, 0.8f);
    drawCube(scale * 0.6f, scale * 1.0f, scale * 0.3f);

    // Head
    glColor3f(1.0f, 0.8f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, scale * 0.75f + scale * 0.25f, 0.0f);
    drawCube(scale * 0.5f, scale * 0.5f, scale * 0.5f);
    glPopMatrix();

    // Left Arm
    glColor3f(0.2f, 0.2f, 0.8f);
    glPushMatrix();
    glTranslatef(-scale * 0.45f, scale * 0.3f, 0.0f);
    drawCube(scale * 0.2f, scale * 0.6f, scale * 0.2f);
    glPopMatrix();

    // Right Arm
    glPushMatrix();
    glTranslatef(scale * 0.45f, scale * 0.3f, 0.0f);
    drawCube(scale * 0.2f, scale * 0.6f, scale * 0.2f);
    glPopMatrix();

    // Left Leg
    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix();
    glTranslatef(-scale * 0.18f, -scale * 0.8f, 0.0f);
    drawCube(scale * 0.18f, scale * 0.6f, scale * 0.18f);
    glPopMatrix();

    // Right Leg
    glPushMatrix();
    glTranslatef(scale * 0.18f, -scale * 0.8f, 0.0f);
    drawCube(scale * 0.18f, scale * 0.6f, scale * 0.18f);
    glPopMatrix();

    glPopMatrix();
}

typedef struct {
    float x, y, z;
} Waypoint;

#define NUM_WAYPOINTS 4
Waypoint waypoints[NUM_WAYPOINTS] = {
    {0.0f, 0.0f, -10.0f},   // starA
    {0.0f, 0.0f, -25.0f},   // starB
    {5.0f, 5.0f, -40.0f},   // starC
    {20.0f, 5.0f, -40.0f}   // starD
};

typedef enum {
    SCENE_SPACE,
    SCENE_LAND
} SceneType;

SceneType currentScene = SCENE_SPACE;

void drawLandMap() {
    glColor3f(0.2f, 0.8f, 0.2f); // Green land
    float size = 20.0f;
    float step = 0.5f; // Smaller step = smoother terrain
    float heightScale = 2.0f;

    for (float x = -size; x < size; x += step) {
        glBegin(GL_TRIANGLE_STRIP);
        for (float z = -size; z <= size; z += step) {
            // Height function: hills using sine/cosine
            float y1 = sinf(0.2f * x) * cosf(0.2f * z) * heightScale;
            float y2 = sinf(0.2f * (x + step)) * cosf(0.2f * z) * heightScale;

            glColor3f(0.2f + 0.1f * y1, 0.8f - 0.1f * y1, 0.2f); // Vary color by height
            glVertex3f(x, y1, z);

            glColor3f(0.2f + 0.1f * y2, 0.8f - 0.1f * y2, 0.2f);
            glVertex3f(x + step, y2, z);
        }
        glEnd();
    }
}

void drawBuildingsOnLand() {
    // Example: 3 buildings at different positions and sizes
    glColor3f(0.6f, 0.6f, 0.6f); // Gray building
    drawCube(1.0f, 3.0f, 1.0f); // Will be positioned below

    glPushMatrix();
    glTranslatef(3.0f, 1.5f, 2.0f); // (x, y, z)
    drawCube(2.0f, 3.0f, 2.0f);
    glPopMatrix();

    glColor3f(0.8f, 0.5f, 0.2f); // Brown building
    glPushMatrix();
    glTranslatef(-4.0f, 1.0f, -3.0f);
    drawCube(1.5f, 2.0f, 1.5f);
    glPopMatrix();

    glColor3f(0.3f, 0.3f, 0.7f); // Blue building
    glPushMatrix();
    glTranslatef(6.0f, 2.0f, -5.0f);
    drawCube(1.0f, 4.0f, 1.0f);
    glPopMatrix();
}

void drawBuildingWithWindows(
    float bx, float by, float bz,
    float sx, float sy, float sz,
    float windowW, float windowH,
    float r, float g, float b // Building color
) {
    float windowGap = 0.1f; // Gap between windows

    glColor3f(r, g, b); // Building color
    glPushMatrix();
    glTranslatef(bx, by, bz);
    drawCube(sx, sy, sz);

    // Calculate number of windows that fit per face
    int numWindowsY = (int)((sy + windowGap) / (windowH + windowGap));
    int numWindowsX = (int)((sx + windowGap) / (windowW + windowGap));
    int numWindowsZ = (int)((sz + windowGap) / (windowW + windowGap));

    float totalWinHeight = numWindowsY * windowH + (numWindowsY - 1) * windowGap;
    float totalWinWidthX = numWindowsX * windowW + (numWindowsX - 1) * windowGap;
    float totalWinWidthZ = numWindowsZ * windowW + (numWindowsZ - 1) * windowGap;

    float startX = -sx/2 + (sx - totalWinWidthX) / 2 + windowW/2;
    float startY = -sy/2 + (sy - totalWinHeight) / 2 + windowH/2;
    float startZ = -sz/2 + (sz - totalWinWidthZ) / 2 + windowW/2;
    float zFront = sz/2 + 0.01f;
    float zBack  = -sz/2 - 0.01f;
    float xRight = sx/2 + 0.01f;
    float xLeft  = -sx/2 - 0.01f;

    glColor3f(0.2f, 0.6f, 0.9f); // Window color

    // Front face (z = sz/2)
    for (int row = 0; row < numWindowsY; ++row) {
        for (int col = 0; col < numWindowsX; ++col) {
            float wx = startX + col * (windowW + windowGap);
            float wy = startY + row * (windowH + windowGap);
            glBegin(GL_QUADS);
            glVertex3f(wx - windowW/2, wy - windowH/2, zFront);
            glVertex3f(wx + windowW/2, wy - windowH/2, zFront);
            glVertex3f(wx + windowW/2, wy + windowH/2, zFront);
            glVertex3f(wx - windowW/2, wy + windowH/2, zFront);
            glEnd();
        }
    }

    // Back face (z = -sz/2)
    for (int row = 0; row < numWindowsY; ++row) {
        for (int col = 0; col < numWindowsX; ++col) {
            float wx = startX + col * (windowW + windowGap);
            float wy = startY + row * (windowH + windowGap);
            glBegin(GL_QUADS);
            glVertex3f(wx - windowW/2, wy - windowH/2, zBack);
            glVertex3f(wx + windowW/2, wy - windowH/2, zBack);
            glVertex3f(wx + windowW/2, wy + windowH/2, zBack);
            glVertex3f(wx - windowW/2, wy + windowH/2, zBack);
            glEnd();
        }
    }

    // Right face (x = sx/2)
    for (int row = 0; row < numWindowsY; ++row) {
        for (int col = 0; col < numWindowsZ; ++col) {
            float wz = startZ + col * (windowW + windowGap);
            float wy = startY + row * (windowH + windowGap);
            glBegin(GL_QUADS);
            glVertex3f(xRight, wy - windowH/2, wz - windowW/2);
            glVertex3f(xRight, wy - windowH/2, wz + windowW/2);
            glVertex3f(xRight, wy + windowH/2, wz + windowW/2);
            glVertex3f(xRight, wy + windowH/2, wz - windowW/2);
            glEnd();
        }
    }

    // Left face (x = -sx/2)
    for (int row = 0; row < numWindowsY; ++row) {
        for (int col = 0; col < numWindowsZ; ++col) {
            float wz = startZ + col * (windowW + windowGap);
            float wy = startY + row * (windowH + windowGap);
            glBegin(GL_QUADS);
            glVertex3f(xLeft, wy - windowH/2, wz - windowW/2);
            glVertex3f(xLeft, wy - windowH/2, wz + windowW/2);
            glVertex3f(xLeft, wy + windowH/2, wz + windowW/2);
            glVertex3f(xLeft, wy + windowH/2, wz - windowW/2);
            glEnd();
        }
    }

    glPopMatrix();
}

void drawTree(float x, float y, float z, float trunkHeight, float trunkWidth, float leafSize) {
    // Draw trunk
    glColor3f(0.55f, 0.27f, 0.07f); // Brown
    glPushMatrix();
    glTranslatef(x, y + trunkHeight/2, z);
    drawCube(trunkWidth, trunkHeight, trunkWidth);
    glPopMatrix();

    // Draw leaves (canopy)
    glColor3f(0.1f, 0.7f, 0.2f); // Green
    glPushMatrix();
    glTranslatef(x, y + trunkHeight + leafSize/2, z);
    drawCube(leafSize, leafSize, leafSize);
    glPopMatrix();
}

void drawChristmasTree(float x, float y, float z, float trunkHeight, float trunkWidth, float leafHeight, float leafWidth) {
    // Draw trunk
    glColor3f(0.55f, 0.27f, 0.07f); // Brown
    glPushMatrix();
    glTranslatef(x, y + trunkHeight/2, z);
    drawCube(trunkWidth, trunkHeight, trunkWidth);
    glPopMatrix();

    // Draw 3 triangle layers for leaves
    glColor3f(0.1f, 0.7f, 0.2f); // Green
    for (int i = 0; i < 3; ++i) {
        float layerY = y + trunkHeight + i * (leafHeight * 0.6f);
        float layerW = leafWidth * (1.0f - 0.2f * i);
        float layerH = leafHeight * (1.0f - 0.15f * i);

        glBegin(GL_TRIANGLES);
        // Front face
        glVertex3f(x, layerY + layerH, z);
        glVertex3f(x - layerW/2, layerY, z + layerW/2);
        glVertex3f(x + layerW/2, layerY, z + layerW/2);
        // Right face
        glVertex3f(x, layerY + layerH, z);
        glVertex3f(x + layerW/2, layerY, z + layerW/2);
        glVertex3f(x + layerW/2, layerY, z - layerW/2);
        // Back face
        glVertex3f(x, layerY + layerH, z);
        glVertex3f(x + layerW/2, layerY, z - layerW/2);
        glVertex3f(x - layerW/2, layerY, z - layerW/2);
        // Left face
        glVertex3f(x, layerY + layerH, z);
        glVertex3f(x - layerW/2, layerY, z - layerW/2);
        glVertex3f(x - layerW/2, layerY, z + layerW/2);
        glEnd();
    }
}

void drawStreet(float y, float z, float length, float width) {
    glColor3f(0.2f, 0.2f, 0.2f); // Dark gray for street
    glBegin(GL_QUADS);
        glVertex3f(-length/2, y, z - width/2);
        glVertex3f( length/2, y, z - width/2);
        glVertex3f( length/2, y, z + width/2);
        glVertex3f(-length/2, y, z + width/2);
    glEnd();
}

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL Init failed: %s\n", SDL_GetError());
        return 1;
    }

    int w = 1280, h = 720;
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
    glFrustum(-frustumWidth, frustumWidth, -frustumHeight, frustumHeight, 0.5 , 50.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);    
    
    initStars();

    int running = 1;
    SDL_Event e;

    /*
    //creating our FBO to store the stars
    GLuint fbo, tex;    
    glGenBuffers(1, &fbo);
    glBindBuffer(GL_FRAMEBUFFER, fbo);    
    
    // Create texture
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Attach texture to FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);    
    // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    //    printf("FBO not complete!\n");
    // }     
    
    // Unbind FBO to render to screen by default
    glBindBuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    */
    
    bool bForward=0 , bBackward=0, bLeft=0, bRight=0, bUp=0, bDown=0;
    float playerX = 0.0f, playerY = 0.0f, playerZ = -9.0f;
    int currentWaypoint = 0;
    int nextWaypoint = 1;
    float travelT = 0.0f;
    int traveling = 1;
    float travelSpeed = 0.2f; // Adjust for speed
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
                switch (e.key.key) {
                    case SDLK_W: bForward = true; break; // Forward
                    case SDLK_S: bBackward = true; break; // Backward
                    case SDLK_A: bLeft = true; break; // Left
                    case SDLK_D: bRight = true; break; // Right                    
                    case SDLK_ESCAPE: running = 0; break; // Exit
                    case SDLK_SPACE: bUp = true; break; // Up
                    case SDLK_LCTRL: bDown = true; break; // Down
                    case SDLK_UP:    playerZ -= 0.1f; break;
                    case SDLK_DOWN:  playerZ += 0.1f; break;
                    case SDLK_LEFT:  playerX -= 0.1f; break;
                    case SDLK_RIGHT: playerX += 0.1f; break;
                    case SDLK_TAB:
                        if (currentScene == SCENE_SPACE)
                            currentScene = SCENE_LAND;
                        else
                            currentScene = SCENE_SPACE;
                        break;
                }                                
            }
            if (e.type == SDL_EVENT_KEY_UP) {
                switch (e.key.key) {
                    case SDLK_W: bForward = false; break; // Forward
                    case SDLK_S: bBackward = false; break; // Backward
                    case SDLK_A: bLeft = false; break; // Left
                    case SDLK_D: bRight = false; break; // Right
                    case SDLK_SPACE: bUp = false; break; // Up
                    case SDLK_LCTRL: bDown = false; break; // Down
                }
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
        { //handle movement per frame
            if (bForward) {
                // move forward based on where you looking at (using all 3 angles)
                camX += moveSpeed * sinf(yaw * M_PI / 180.0f);
                camZ -= moveSpeed * cosf(yaw * M_PI / 180.0f);
                camY -= moveSpeed * sinf(pitch * M_PI / 180.0f); // Adjust Y based on pitch            
            }
            if (bBackward) {
                camX -= moveSpeed * sinf(yaw * M_PI / 180.0f);
                camZ += moveSpeed * cosf(yaw * M_PI / 180.0f);
                camY += moveSpeed * sinf(pitch * M_PI / 180.0f); // Adjust Y based on pitch            
            }
            if (bLeft) {
                camX -= moveSpeed * cosf(yaw * M_PI / 180.0f);
                camZ -= moveSpeed * sinf(yaw * M_PI / 180.0f);
            }
            if (bRight) {
                camX += moveSpeed * cosf(yaw * M_PI / 180.0f);
                camZ += moveSpeed * sinf(yaw * M_PI / 180.0f);
            }
            if (bUp) {
                camY += moveSpeed; // Move up
            }
            if (bDown) {
                camY -= moveSpeed; // Move down
            }
        }

        //make alpha have an effect in glClear
        // Clear the screen and depth buffer        
                
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
        //glClear(GL_DEPTH_BUFFER_BIT);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        /*
        glTranslatef( 0.0f , 0.0f , -1.0f );
        glEnable(GL_BLEND);        
        glDepthMask(GL_FALSE);
        //draw a filled 2D quad covering the whole screen         
        glColor4f(0.0f, 0.0f, 0.0f, 1.0f); // Black background
        glBegin(GL_QUADS);            
            glVertex2f(-4.0f, -4.0f);
            glVertex2f( 4.0f, -4.0f);
            glVertex2f( 4.0f,  4.0f);
            glVertex2f(-4.0f,  4.0f);
        glEnd();
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glLoadIdentity();
        */

        glRotatef(pitch, 1.0f, 0.0f, 0.0f);
        glRotatef(yaw,   0.0f, 1.0f, 0.0f);
                
        glPointSize(3.0f);
        if (currentScene == SCENE_SPACE) {
            updateStars(0.15f); drawStars();  
        }      
        
        glTranslatef(-camX, -camY, -camZ);

        
        if (currentScene == SCENE_SPACE) {
        
            draw3DStar(0.0f, 0.0f, -10.0f); // Draw at local z=0
            draw3DStar(0.0f, 0.0f, -25.0f); // Draw at local z=0
            draw3DStar(5.0f, 5.0f, -40.0f); // Draw at local z=0
            draw3DStar(20.0f, 5.0f, -40.0f); // Draw at local z=0
            
            if (traveling) {
                travelT += travelSpeed * 0.016f;
                if (travelT > 1.0f) {
                    travelT = 1.0f;
                    traveling = 0; // Stop at the end of this segment

                    // Move to next segment if possible
                    if (nextWaypoint < NUM_WAYPOINTS - 1) {
                        currentWaypoint = nextWaypoint;
                        nextWaypoint++;
                        travelT = 0.0f;
                        traveling = 1; // Start traveling to next
                    }
                }
                playerX = waypoints[currentWaypoint].x + (waypoints[nextWaypoint].x - waypoints[currentWaypoint].x) * travelT;
                playerY = waypoints[currentWaypoint].y + (waypoints[nextWaypoint].y - waypoints[currentWaypoint].y) * travelT;
                playerZ = waypoints[currentWaypoint].z + (waypoints[nextWaypoint].z - waypoints[currentWaypoint].z) * travelT;
            }
            drawBlockyPerson(playerX, playerY, playerZ, 0.1f);

            // Then draw the pipe with depth mask off
            glDepthMask(GL_FALSE);
            drawPipeBetweenStars(0.0f, 0.0f, -25.0f, 0.0f, 0.0f, -10.0f);
            drawPipeBetweenStars(5.0f, 5.0f, -40.0f, 0.0f, 0.0f, -25.0f);
            drawPipeBetweenStars(20.0f, 5.0f, -40.0f, 5.0f, 5.0f, -40.0f);  
            glDepthMask(GL_TRUE);
        } 
        else if (currentScene == SCENE_LAND) {
           // glLoadIdentity();
           // glTranslatef(0.0f, -5.0f, -25.0f); // Move camera up and back
           // glRotatef(45, 1.0f, 0.0f, 0.0f);   // Tilt down to look at the land
            drawLandMap();
            drawStreet(0.0f, 0.0f, 40.0f, 2.0f); // y=0, z=0, length=40, width=2
            //drawBuildingsOnLand();
            drawBuildingWithWindows(0.0f, 5.0f, 0.0f, 2.0f, 10.0f, 5.0f, 0.2f, 0.2f, 0.6f, 0.6f, 0.6f); // Gray
            drawBuildingWithWindows(5.0f, 2.5f, 5.0f, 2.0f, 5.0f, 2.0f, 0.2f, 0.2f, 0.8f, 0.5f, 0.2f);   // Brown
            drawBuildingWithWindows(-5.0f, 7.5f, -5.0f, 2.0f, 15.0f, 2.0f, 0.2f, 0.2f, 0.3f, 0.3f, 0.7f); // Blue
            drawBuildingWithWindows(10.0f, 3.0f, -10.0f, 1.0f, 6.0f, 1.0f, 0.2f, 0.2f, 0.8f, 0.8f, 0.2f); // Yellow
            drawBuildingWithWindows(-10.0f, 4.0f, 10.0f, 1.5f, 8.0f, 1.5f, 0.2f, 0.2f, 0.0f, 0.0f, 0.0f); // Black

            // Add some trees
            // drawTree(2.0f, 0.0f, 2.0f, 7.0f, 0.2f, 0.8f);
            // drawTree(-3.0f, 0.0f, -4.0f, 9.2f, 0.25f, 1.0f);
            // drawTree(7.0f, 0.0f, -6.0f, 5.8f, 0.18f, 0.6f);

            // Add some Christmas trees
            drawChristmasTree(2.0f, 0.0f, 2.0f, 2.0f, 0.2f, 1.2f, 1.2f);
            drawChristmasTree(-3.0f, 0.0f, -4.0f, 2.2f, 0.25f, 1.5f, 1.5f);
            drawChristmasTree(7.0f, 0.0f, -6.0f, 1.8f, 0.18f, 1.0f, 1.0f);
        }
        // glDisableClientState(GL_VERTEX_ARRAY);

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
