#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "level/levelrenderer.h"
#include "level/level.h"
#include "player.h"
#include "timer.h"
#include "hitresult.h"

GLFWwindow* window;
Level level;
LevelRenderer levelRenderer;
Player player;
Timer timer;

const int width = 1024;
const int height = 768;

GLfloat fogColor[4] = {14 / 255.0f, 11 / 255.0f, 10 / 255.0f, 1.0f};

int viewportBuffer[16];
int selectBuffer[2000];
HitResult hitResult;
int isHitNull = 1;

void tick(Player* player, GLFWwindow* window);

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void initFog() {
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, -10);
    glFogf(GL_FOG_END, 20);
    glFogfv(GL_FOG_COLOR, fogColor);
    glDisable(GL_FOG);
}

int init(Level* level, LevelRenderer* levelRenderer, Player* player) {
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 0; // Return 0 on failure
    }

    // Set GLFW window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(width, height, "RubyDung", NULL, NULL);
    if (!window) {
        glfwTerminate();
        fprintf(stderr, "Failed to create GLFW window\n");
        return 0; // Return 0 on failure
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);

    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set the clear color
    glClearColor(0.5f, 0.8f, 1.0f, 0.0f);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Grab the mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the key callback function
    glfwSetKeyCallback(window, keyCallback);

    // Initialize Level
    Level_init(level, 256, 256, 64);

    // Initialize LevelRenderer
    LevelRenderer_init(levelRenderer, level);

    Player_init(player, level);

    // Initialize the timer
    Timer_init(&timer, 60.0);

    return 1; // Return 1 on success
}

void destroy(Level* level) {
    // Save level
    Level_save(level);

    // Destroy Level
    Level_destroy(level);

    // Terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
}

void moveCameraToPlayer(Player* player, float partialTicks) {
    // Debug
    //printf("Debug: Player Position: (%.2f, %.2f, %.2f)\n", player->x, player->y, player->z);
    //printf("Debug: Mouse Look: (xRotation=%.2f, yRotation=%.2f)\n", player->xRotation, player->yRotation);

    // Eye height
    glTranslatef(0.0f, 0.0f, -0.3f);

    // Rotate camera
    glRotatef(player->xRotation, 1.0f, 0.0f, 0.0f);
    glRotatef(player->yRotation, 0.0f, 1.0f, 0.0f);

    // Smooth movement
    double x = player->prevX + (player->x - player->prevX) * partialTicks;
    double y = player->prevY + (player->y - player->prevY) * partialTicks;
    double z = player->prevZ + (player->z - player->prevZ) * partialTicks;

    // Move camera to player's location
    glTranslated(-x, -y, -z);
}

void setupCamera(Player* player, float partialTicks) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(70, (double)width / (double)height, 0.05F, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    moveCameraToPlayer(player, partialTicks);
}

void setupPickCamera(Player* player, float partialTicks, double x, double y) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glGetIntegerv(GL_VIEWPORT, viewportBuffer);
    gluPickMatrix(x, y, 5.0, 5.0, viewportBuffer);
    gluPerspective(70.0, (double)width / (double)height, 0.05f, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    moveCameraToPlayer(player, partialTicks);
}

void pick(float partialTicks) {
    int selectBufferPos = 0;
    //glSelectBuffer(sizeof(selectBuffer), selectBuffer);
    glRenderMode(GL_SELECT);
    setupPickCamera(&player, partialTicks, width / 2, height / 2);
    //LevelRenderer_renderPick(&level, &levelRenderer);  <--- throws a fucking segfault TODO
    int hits = glRenderMode(GL_RENDER);
    long closest = 0L;
    int names[10];
    int hitNameCount = 0;

    for (int i = 0; i < hits; i++) {
        int nameCount = selectBuffer[selectBufferPos++];
        long minZ = selectBuffer[selectBufferPos++];
        selectBufferPos++;
        int j;

        if (minZ >= closest && i != 0) {
            for (j = 0; j < nameCount; j++) {
                selectBufferPos++;
            }
        } else {
            closest = minZ;
            hitNameCount = nameCount;

            for (int j = 0; j < nameCount; j++) {
                names[j] = selectBuffer[selectBufferPos++];
            }
        }
    }

    if (hitNameCount > 0) {
        //hitresult_create(&hitResult, names[0], names[1], names[2], names[3], names[4]);
        isHitNull = 0;
    } else {
        isHitNull = 1;
    }

}

void render(Level level, LevelRenderer levelRenderer, Player* player, GLFWwindow* window, float partialTicks) {
    // Get mouse motion
    double motionX, motionY;
    glfwGetCursorPos(window, &motionX, &motionY);
    motionX *= 0.2;  // Adjust sensitivity
    motionY *= -0.2;

    pick(partialTicks);

    // Rotate the camera using the mouse motion input
    Player_turn(player, window, motionX, motionY);

    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setup normal player camera
    setupCamera(player, partialTicks);

    // Setup fog
    initFog();

    // Render bright tiles
    LevelRenderer_render(&levelRenderer, 0);

    // Enable fog to render shadow
    glEnable(GL_FOG);

    // Render dark tiles in shadow
    LevelRenderer_render(&levelRenderer, 1);

    // Finish rendering
    glDisable(GL_TEXTURE_2D);

    if (!isHitNull) {
        LevelRenderer_renderHit(&levelRenderer, &hitResult);
    }

    // Update the display
    glfwSwapBuffers(window);
}

void run(Level* level, LevelRenderer* levelRenderer, Player* player) {
    // Initialize the game
    if (!init(level, levelRenderer, player)) {
        fprintf(stderr, "Failed to initialize RubyDung\n");
        destroy(level);
        exit(EXIT_FAILURE);
    }

    int frames = 0;
    long long lastTime = currentTimeMillis();

    // Start the game loop
    while (!glfwWindowShouldClose(window)) {
        // Framerate limit
        glfwPollEvents();

        // Update the timer
        Timer_advanceTime(&timer);
    
        // Call the tick to reach updates 20 per second
        for (int i = 0; i < timer.ticks; ++i) {
            tick(player, window);
        }

        // Render the game
        render(*level, *levelRenderer, player, window, timer.partialTicks);

        frames++;

        while (currentTimeMillis() >= lastTime + 1000LL) {
            printf("%d fps\n", frames);

            lastTime += 1000LL;
            frames = 0;
        }

        // Update the display
        glfwSwapBuffers(window);
    }

    // Destroy I/O and save game
    destroy(level);
}

int main(void) {
    // Run the game
    run(&level, &levelRenderer, &player);

    return EXIT_SUCCESS;
}

void tick(Player* player, GLFWwindow* window) {
    // Your existing tick function
    Player_tick(player, window);
}
