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
#include "level/frustum.h"
#include "level/tile/tile.h"
#include "textures.h"
#include "particle/particleengine.h"
#include "player.h"
#include "character/zombie.h"
#include "timer.h"
#include "hitresult.h"

#define MAX_MOBS 100

static GLFWwindow*    window;
static Level          level;
static LevelRenderer  levelRenderer;
static Player         player;
static Timer          timer;
static Zombie         mobs[MAX_MOBS];
static ParticleEngine particleEngine;

static int mobCount = 0;

static int prevLeft  = GLFW_RELEASE;
static int prevRight = GLFW_RELEASE;
static int prevEnter = GLFW_RELEASE;
static int prevNum1 = GLFW_RELEASE, prevNum2 = GLFW_RELEASE;
static int prevNum3 = GLFW_RELEASE, prevNum4 = GLFW_RELEASE;
static int prevG    = GLFW_RELEASE;

static int texTerrain = 0;

static Tessellator hudTess;
static int selectedTileId = 1;   // 1=rock, 3=dirt, 4=stoneBrick, 5=wood

static int gWinWidth  = 1024;
static int gWinHeight = 768;
static int gIsFullscreen = 1;

static GLfloat fogColorDaylight[4] = { 254.0f/255.0f, 251.0f/255.0f, 250.0f/255.0f, 1.0f };
static GLfloat fogColorShadow  [4] = {  14.0f/255.0f,  11.0f/255.0f,  10.0f/255.0f, 1.0f };

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

static void setupFog(int type) {
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP);

    if (type == 0) { // daylight
        glFogf(GL_FOG_DENSITY, 0.001f);
        glFogfv(GL_FOG_COLOR, fogColorDaylight);
        glDisable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);
    } else {         // shadow
        glFogf(GL_FOG_DENSITY, 0.06f);
        glFogfv(GL_FOG_COLOR, fogColorShadow);
        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);

        GLfloat ambient[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    }
}

/* --- boot/shutdown ----------------------------------------------------------- */

static int init(Level* lvl, LevelRenderer* lr, Player* p) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 0;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    window = glfwCreateWindow(mode->width, mode->height, "Game", monitor, NULL);
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

    Tile_registerAll();

    texTerrain = loadTexture("resources/terrain.png", GL_NEAREST);

    Level_init(lvl, 256, 256, 64);
    LevelRenderer_init(lr, lvl);
    calcLightDepths(lvl, 0, 0, lvl->width, lvl->height);

    Player_init(p, lvl);

    ParticleEngine_init(&particleEngine, lvl, (GLuint)texTerrain);

    mobCount = 0;
    for (int i = 0; i < 10 && i < MAX_MOBS; ++i) {
        Zombie_init(&mobs[mobCount], lvl, 128.0, 0.0, 129.0);
        Entity_resetPosition(&mobs[mobCount].base);
        mobCount++;
    }

    Timer_init(&timer, 20.0f);

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

    glRotatef(p->e.xRotation, 1.0f, 0.0f, 0.0f);
    glRotatef(p->e.yRotation, 0.0f, 1.0f, 0.0f);

    double x = p->e.prevX + (p->e.x - p->e.prevX) * t;
    double y = p->e.prevY + (p->e.y - p->e.prevY) * t;
    double z = p->e.prevZ + (p->e.z - p->e.prevZ) * t;

    glTranslated(-x, -y, -z);
}

static void setupCamera(Player* p, float t) {
    int fbw, fbh; glfwGetFramebufferSize(window, &fbw, &fbh);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, (double)fbw / (double)fbh, 0.05, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    moveCameraToPlayer(p, t);
}

static void setupOrthoCamera(void) {
    int fbw, fbh; glfwGetFramebufferSize(window, &fbw, &fbh);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, fbw, fbh, 0.0, 100.0, 300.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -200.0f);
}

static void drawGui(float partialTicks) {
    (void)partialTicks;
    glClear(GL_DEPTH_BUFFER_BIT);
    setupOrthoCamera();

    int fbw, fbh; glfwGetFramebufferSize(window, &fbw, &fbh);

    glPushMatrix();
    glTranslated(fbw - 48, 48.0, 0.0);
    glScalef(48.0f, 48.0f, 48.0f);
    glRotatef(30.0f, 1, 0, 0);
    glRotatef(45.0f, 0, 1, 0);
    glTranslatef(1.5f, -0.5f, -0.5f);

    glBindTexture(GL_TEXTURE_2D, texTerrain);
    glEnable(GL_TEXTURE_2D);

    Tessellator_init(&hudTess);
    const Tile* hand = gTiles[selectedTileId];
    if (hand && hand->render) {
        // render one block at (-2,0,0)
        hand->render(hand, &hudTess, &level, 0, -2, 0, 0);
    }
    Tessellator_flush(&hudTess);

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // Crosshair
    int cx = fbw / 2, cy = fbh / 2;
    glColor4f(1,1,1,1);
    Tessellator_init(&hudTess);
    // vertical
    Tessellator_vertex(&hudTess, (float)(cx + 1), (float)(cy - 8), 0.0f);
    Tessellator_vertex(&hudTess, (float)(cx + 0), (float)(cy - 8), 0.0f);
    Tessellator_vertex(&hudTess, (float)(cx + 0), (float)(cy + 9), 0.0f);
    Tessellator_vertex(&hudTess, (float)(cx + 1), (float)(cy + 9), 0.0f);
    // horizontal
    Tessellator_vertex(&hudTess, (float)(cx + 9), (float)(cy + 0), 0.0f);
    Tessellator_vertex(&hudTess, (float)(cx - 8), (float)(cy + 0), 0.0f);
    Tessellator_vertex(&hudTess, (float)(cx - 8), (float)(cy + 1), 0.0f);
    Tessellator_vertex(&hudTess, (float)(cx + 9), (float)(cy + 1), 0.0f);
    Tessellator_flush(&hudTess);
}

/* --- picking ----------------------------------------------------------------- */

static void get_look_dir(const Player* p, double* dx, double* dy, double* dz) {
    const double yaw   = p->e.yRotation * M_PI / 180.0;
    const double pitch = p->e.xRotation * M_PI / 180.0;
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
    double x = player.e.prevX + (player.e.x - player.e.prevX) * t;
    double y = player.e.prevY + (player.e.y - player.e.prevY) * t;
    double z = player.e.prevZ + (player.e.z - player.e.prevZ) * t;

    double dx, dy, dz;
    get_look_dir(&player, &dx, &dy, &dz);

    // nudge origin to match the 0.3 view-space translate
    x += dx * 0.3; y += dy * 0.3; z += dz * 0.3;

    const int reachBlocks = 3; // axis-aligned reach cube

    HitResult hr;
    if (raycast_block(&level, x, y, z, dx, dy, dz, 100.0, &hr)) {
        int px = (int)floor(player.e.x);
        int py = (int)floor(player.e.y);
        int pz = (int)floor(player.e.z);
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

static void handleGameplayKeys(GLFWwindow* w) {
    // Enter = save level
    int enter = glfwGetKey(w, GLFW_KEY_ENTER);
    if (enter == GLFW_PRESS && prevEnter == GLFW_RELEASE) {
        Level_save(&level);
    }
    prevEnter = enter;

    // 1..4 = select tile
    int n1 = glfwGetKey(w, GLFW_KEY_1);
    int n2 = glfwGetKey(w, GLFW_KEY_2);
    int n3 = glfwGetKey(w, GLFW_KEY_3);
    int n4 = glfwGetKey(w, GLFW_KEY_4);
    if (n1 == GLFW_PRESS && prevNum1 == GLFW_RELEASE) selectedTileId = 1;  // rock
    if (n2 == GLFW_PRESS && prevNum2 == GLFW_RELEASE) selectedTileId = 3;  // dirt
    if (n3 == GLFW_PRESS && prevNum3 == GLFW_RELEASE) selectedTileId = 4;  // stone brick
    if (n4 == GLFW_PRESS && prevNum4 == GLFW_RELEASE) selectedTileId = 5;  // wood
    prevNum1 = n1; prevNum2 = n2; prevNum3 = n3; prevNum4 = n4;

    // G = spawn zombie at player
    int g = glfwGetKey(w, GLFW_KEY_G);
    if (g == GLFW_PRESS && prevG == GLFW_RELEASE && mobCount < MAX_MOBS) {
        Zombie_init(&mobs[mobCount++], &level, player.e.x, player.e.y, player.e.z);
    }
    prevG = g;
}

static void handleBlockClicks(GLFWwindow* w) {
    int left  = glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT);
    int right = glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_RIGHT);

    if (right == GLFW_PRESS && prevRight == GLFW_RELEASE && !isHitNull) {
        // capture previous tile
        int prevId = Level_getTile(&level, hitResult.x, hitResult.y, hitResult.z);
        const Tile* prevTile = (prevId >= 0 && prevId < 256) ? gTiles[prevId] : NULL;

        // destroy the tile
        bool changed = level_setTile(&level, hitResult.x, hitResult.y, hitResult.z, 0);

        // spawn particles
        if (prevTile && changed) {
            Tile_onDestroy(prevTile, &level, hitResult.x, hitResult.y, hitResult.z, &particleEngine);
        }
    }

    if (left == GLFW_PRESS && prevLeft == GLFW_RELEASE && !isHitNull) {
        // place on the face we hit
        int nx = 0, ny = 0, nz = 0;
        switch (hitResult.f) {
            case 0: ny = -1; break; // bottom
            case 1: ny =  1; break; // top
            case 2: nz = -1; break; // -Z
            case 3: nz =  1; break; // +Z
            case 4: nx = -1; break; // -X
            case 5: nx =  1; break; // +X
        }
        level_setTile(&level,
                      hitResult.x + nx,
                      hitResult.y + ny,
                      hitResult.z + nz,
                      selectedTileId);
    }

    prevLeft  = left;
    prevRight = right;
}

/* --- frame ------------------------------------------------------------------- */

static void render(Level* lvl, LevelRenderer* lr, Player* p, GLFWwindow* w, float t) {
    (void)w;
    (void)lvl;

    int fbw, fbh;
    glfwGetFramebufferSize(window, &fbw, &fbh);
    glViewport(0, 0, fbw, fbh);

    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    my *= -1.0;
    Player_turn(p, window, (float)mx, (float)my);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setupCamera(p, t);

    setupFog(0);
    LevelRenderer_render(lr, 0);    // lit layer

    // Zombies in sunlight (lit)
    for (int i = 0; i < mobCount; ++i) {
        const Zombie* z = &mobs[i];
        if (Entity_isLit(&z->base) && frustum_isVisible(&z->base.boundingBox)) {
            Zombie_render(z, t);
        }
    }

    ParticleEngine_render(&particleEngine, p, t, 0);

    setupFog(1);
    LevelRenderer_render(lr, 1);    // shadow layer

    // Zombies in shadow (not lit)
    for (int i = 0; i < mobCount; ++i) {
        const Zombie* z = &mobs[i];
        if (!Entity_isLit(&z->base) && frustum_isVisible(&z->base.boundingBox)) {
            Zombie_render(z, t);
        }
    }

    ParticleEngine_render(&particleEngine, p, t, 1);

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);

    if (!isHitNull) LevelRenderer_renderHit(lr, &hitResult);

    drawGui(t);

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
        handleGameplayKeys(window);

        LevelRenderer_updateDirtyChunks(&levelRenderer, &player);

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

    // random tile ticks (grass growth/decay lives here)
    Level_onTick(&level);

    ParticleEngine_onTick(&particleEngine);

    Player_onTick(p, window);
    for (int i = 0; i < mobCount; ) {
        Zombie_onTick(&mobs[i]);
        if (mobs[i].base.removed) {
            mobs[i] = mobs[mobCount - 1];
            mobCount--;
            continue;
        }
        i++;
    }
}