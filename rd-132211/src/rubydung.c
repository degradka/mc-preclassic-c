#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "level/levelrenderer.h"
#include "level/level.h"
#include "player.h"
#include "timer.h"

GLFWwindow* window;
Timer timer;

void tick(Player* player, GLFWwindow* window);

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int init(Level* level, LevelRenderer* levelRenderer, Player* player) {
    int width = 1024;
    int height = 768;

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

    // Enable VSync
    glfwSwapInterval(1);

    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set the clear color
    glClearColor(0.5f, 0.8f, 1.0f, 0.0f);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Set up perspective
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, width / (float)height, 0.05F, 1000);
    glMatrixMode(GL_MODELVIEW);

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
    Timer_init(&timer, 20.0);

    return 1; // Return 1 on success
}

void destroy(Level* level) {
    // Destroy Level
    Level_destroy(level);

    // Terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
}

void moveCameraToPlayer(Player* player) {
    printf("Player Position: (%.2f, %.2f, %.2f)\n", player->x, player->y, player->z);
    printf("Mouse Look: (xRotation=%.2f, yRotation=%.2f)\n", player->xRotation, player->yRotation);

    // Eye height
    glTranslatef(0.0f, 0.0f, -0.3f);

    // Rotate camera
    glRotatef(player->xRotation, 1.0f, 0.0f, 0.0f);
    glRotatef(player->yRotation, 0.0f, 1.0f, 0.0f);

    // Move camera to player's location
    glTranslated(-player->x, -player->y, -player->z);
}

void render(Level level, LevelRenderer levelRenderer, Player player) {
    // Get mouse motion
    double motionX, motionY;
    glfwGetCursorPos(window, &motionX, &motionY);
    motionX *= 0.1;  // Adjust sensitivity
    motionY *= 0.1;

    // Rotate the camera using the mouse motion input
    Player_turn(&player, motionX, motionY);

    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset camera
    glLoadIdentity();

    // Move camera to player's position
    moveCameraToPlayer(&player);

    // Render level chunks
    LevelRenderer_render(&levelRenderer);

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
        render(*level, *levelRenderer, *player);

        // Update the display
        glfwSwapBuffers(window);
    }

    // Destroy I/O and save game
    destroy(level);
}

int main(void) {
    Level level;
    LevelRenderer levelRenderer;
    Player player;

    // Run the game
    run(&level, &levelRenderer, &player);

    return EXIT_SUCCESS;
}

void tick(Player* player, GLFWwindow* window) {
    // Your existing tick function
    Player_tick(player, window);
}