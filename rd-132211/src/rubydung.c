// rubydung.c — entry point, window+gl init, camera, picking, main loop

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>

#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include "common.h"

#include "level/level.h"
#include "level/levelrenderer.h"
#include "player.h"
#include "timer.h"
#include "hitresult.h"

static GLFWwindow*   window;
static Level         level;
static LevelRenderer levelRenderer;
static Player        player;
static Timer         timer;

static int prevLeft  = GLFW_RELEASE;
static int prevRight = GLFW_RELEASE;
static int prevEnter = GLFW_RELEASE;

static const int WIN_WIDTH  = 1024;
static const int WIN_HEIGHT = 768;

static GLfloat fogColor[4] = { 14.0f/255.0f, 11.0f/255.0f, 10.0f/255.0f, 1.0f };

static int      isHitNull = 1;
static HitResult hitResult;

static void tick(Player* player, GLFWwindow* window);

/* --- input & GL state helpers ------------------------------------------------ */

static void keyCallback(GLFWwindow* w, int key, int scancode, int action, int mods) {
    (void)scancode; (void)mods;
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(w, GLFW_TRUE);
    }
}

static void initFog(void) {
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, -10.0f);
    glFogf(GL_FOG_END,   20.0f);
    glFogfv(GL_FOG_COLOR, fogColor);
    glDisable(GL_FOG);
}

/* --- boot/shutdown ----------------------------------------------------------- */

static int init(Level* lvl, LevelRenderer* lr, Player* p) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 0;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Game", NULL, NULL);
    if (!window) {
        glfwTerminate();
        fprintf(stderr, "Failed to create GLFW window\n");
        return 0;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0); // uncapped

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "GLEW init failed: %s\n", glewGetErrorString(err));
        return 0;
    }

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_LIGHTING);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.5f, 0.8f, 1.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window, 0, 0);
    glfwSetKeyCallback(window, keyCallback);

    Level_init(lvl, 256, 256, 64);
    LevelRenderer_init(lr, lvl);
    calcLightDepths(lvl, 0, 0, lvl->width, lvl->height);

    Player_init(p, lvl);
    Timer_init(&timer, 60.0f);

    return 1;
}

static void destroy(Level* lvl) {
    Level_save(lvl);
    Level_destroy(lvl);
    glfwDestroyWindow(window);
    glfwTerminate();
}

/* --- camera ------------------------------------------------------------------ */

static void moveCameraToPlayer(Player* p, float t) {
    glTranslatef(0.0f, 0.0f, -0.3f); // eye offset

    glRotatef(p->xRotation, 1.0f, 0.0f, 0.0f);
    glRotatef(p->yRotation, 0.0f, 1.0f, 0.0f);

    double x = p->prevX + (p->x - p->prevX) * t;
    double y = p->prevY + (p->y - p->prevY) * t;
    double z = p->prevZ + (p->z - p->prevZ) * t;

    glTranslated(-x, -y, -z);
}

static void setupCamera(Player* p, float t) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, (double)WIN_WIDTH / (double)WIN_HEIGHT, 0.05, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    moveCameraToPlayer(p, t);
}

/* --- picking ----------------------------------------------------------------- */

static void get_look_dir(const Player* p, double* dx, double* dy, double* dz) {
    const double yaw   = p->yRotation * M_PI / 180.0;
    const double pitch = p->xRotation * M_PI / 180.0;
    const double cp = cos(pitch), sp = sin(pitch);
    const double cy = cos(yaw),   sy = sin(yaw);
    *dx =  sy * cp;   // +X right
    *dy = -sp;        // +Y up
    *dz = -cy * cp;   // -Z forward
}

static int raycast_block(const Level* lvl,
                         double ox, double oy, double oz,
                         double dx, double dy, double dz,
                         double maxDist,
                         HitResult* out) {
    int x = (int)floor(ox);
    int y = (int)floor(oy);
    int z = (int)floor(oz);

    const int stepX = (dx > 0) - (dx < 0);
    const int stepY = (dy > 0) - (dy < 0);
    const int stepZ = (dz > 0) - (dz < 0);

    double tMaxX = (dx != 0.0) ? (((stepX > 0 ? (x + 1) : x) - ox) / dx) : DBL_MAX;
    double tMaxY = (dy != 0.0) ? (((stepY > 0 ? (y + 1) : y) - oy) / dy) : DBL_MAX;
    double tMaxZ = (dz != 0.0) ? (((stepZ > 0 ? (z + 1) : z) - oz) / dz) : DBL_MAX;

    double tDeltaX = (dx != 0.0) ? fabs(1.0 / dx) : DBL_MAX;
    double tDeltaY = (dy != 0.0) ? fabs(1.0 / dy) : DBL_MAX;
    double tDeltaZ = (dz != 0.0) ? fabs(1.0 / dz) : DBL_MAX;

    int face = -1;
    double t = 0.0;

    while (t <= maxDist) {
        if (x < 0 || y < 0 || z < 0 || x >= lvl->width || y >= lvl->depth || z >= lvl->height)
            return 0;

        if (Level_isSolidTile(lvl, x, y, z)) {
            if (out) hitresult_create(out, x, y, z, 0, (face < 0 ? 0 : face));
            return 1;
        }

        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                x += stepX; t = tMaxX; tMaxX += tDeltaX;
                face = (stepX > 0) ? 4 : 5;
            } else {
                z += stepZ; t = tMaxZ; tMaxZ += tDeltaZ;
                face = (stepZ > 0) ? 2 : 3;
            }
        } else {
            if (tMaxY < tMaxZ) {
                y += stepY; t = tMaxY; tMaxY += tDeltaY;
                face = (stepY > 0) ? 0 : 1;
            } else {
                z += stepZ; t = tMaxZ; tMaxZ += tDeltaZ;
                face = (stepZ > 0) ? 2 : 3;
            }
        }
    }
    return 0;
}

static void pick(float t) {
    double x = player.prevX + (player.x - player.prevX) * t;
    double y = player.prevY + (player.y - player.prevY) * t;
    double z = player.prevZ + (player.z - player.prevZ) * t;

    double dx, dy, dz;
    get_look_dir(&player, &dx, &dy, &dz);

    // nudge origin to match the 0.3 view-space translate
    x += dx * 0.3; y += dy * 0.3; z += dz * 0.3;

    const int reachBlocks = 3; // axis-aligned reach cube

    HitResult hr;
    if (raycast_block(&level, x, y, z, dx, dy, dz, 100.0, &hr)) {
        int px = (int)floor(player.x);
        int py = (int)floor(player.y);
        int pz = (int)floor(player.z);
        if (abs(hr.x - px) <= reachBlocks &&
            abs(hr.y - py) <= reachBlocks &&
            abs(hr.z - pz) <= reachBlocks) {
            hitResult = hr;
            isHitNull = 0;
            return;
        }
    }
    isHitNull = 1;
}

/* --- input actions ----------------------------------------------------------- */

static void handleSaveKey(GLFWwindow* w) {
    int enter = glfwGetKey(w, GLFW_KEY_ENTER);
    if (enter == GLFW_PRESS && prevEnter == GLFW_RELEASE) {
        Level_save(&level);
    }
    prevEnter = enter;
}

static void handleBlockClicks(GLFWwindow* w) {
    int left  = glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT);
    int right = glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_RIGHT);

    if (right == GLFW_PRESS && prevRight == GLFW_RELEASE && !isHitNull) {
        level_setTile(&level, hitResult.x, hitResult.y, hitResult.z, 0);
    }

    if (left == GLFW_PRESS && prevLeft == GLFW_RELEASE && !isHitNull) {
        int nx = 0, ny = 0, nz = 0;
        switch (hitResult.f) {
            case 0: ny = -1; break; // bottom
            case 1: ny =  1; break; // top
            case 2: nz = -1; break; // -Z
            case 3: nz =  1; break; // +Z
            case 4: nx = -1; break; // -X
            case 5: nx =  1; break; // +X
        }
        level_setTile(&level, hitResult.x + nx, hitResult.y + ny, hitResult.z + nz, 1);
    }

    prevLeft  = left;
    prevRight = right;
}

/* --- frame ------------------------------------------------------------------- */

static void render(Level* lvl, LevelRenderer* lr, Player* p, GLFWwindow* w, float t) {
    (void)w;
    (void)lvl;

    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    my *= -1.0;
    Player_turn(p, window, (float)mx, (float)my);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setupCamera(p, t);
    initFog();

    LevelRenderer_render(lr, 0);    // lit layer
    glEnable(GL_FOG);
    LevelRenderer_render(lr, 1);    // shadow layer

    glDisable(GL_TEXTURE_2D);
    if (!isHitNull) LevelRenderer_renderHit(lr, &hitResult);

    glfwSwapBuffers(window);
}

/* --- main loop --------------------------------------------------------------- */

static void run(Level* lvl, LevelRenderer* lr, Player* p) {
    if (!init(lvl, lr, p)) {
        fprintf(stderr, "Failed to initialize game\n");
        destroy(lvl);
        exit(EXIT_FAILURE);
    }

    int frames = 0;
    long long last = currentTimeMillis();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        Timer_advanceTime(&timer);
        for (int i = 0; i < timer.ticks; ++i) tick(p, window);

        pick(timer.partialTicks);
        handleBlockClicks(window);
        handleSaveKey(window);

        render(lvl, lr, p, window, timer.partialTicks);

        frames++;
        while (currentTimeMillis() >= last + 1000LL) {
#ifndef NDEBUG
            printf("%d fps\n", frames);
#endif
            last += 1000LL;
            frames = 0;
        }
    }

    destroy(lvl);
}

int main(void) {
    run(&level, &levelRenderer, &player);
    return EXIT_SUCCESS;
}

static void tick(Player* p, GLFWwindow* w) {
    (void)w;
    Player_tick(p, window);
}
