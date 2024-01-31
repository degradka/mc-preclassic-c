#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "level/levelrenderer.h"
#include "level/level.h"

GLFWwindow* window;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int init(Level* level, LevelRenderer* levelRenderer) {
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
    Level_init(level, 256, 64, 256);

    // Initialize LevelRenderer
    LevelRenderer_init(levelRenderer, level);

    return 1; // Return 1 on success
}

void destroy(Level* level) {
    // Destroy Level
    Level_destroy(level);

    // Terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
}

void render(Level level, LevelRenderer levelRenderer) {
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset camera
    glLoadIdentity();

    // Move camera to middle of level
    glTranslated(-level.width / 2.0, -level.depth / 2.0 - 3.0, -level.height / 2.0);

    // Render level chunks
    LevelRenderer_render(&levelRenderer);

    // Update the display
    glfwSwapBuffers(window);
}

void run(Level* level, LevelRenderer* levelRenderer) {
    // Initialize the game
    if (!init(level, levelRenderer)) {
        fprintf(stderr, "Failed to initialize RubyDung\n");
        destroy(level);
        exit(EXIT_FAILURE);
    }

    // Start the game loop
    while (!glfwWindowShouldClose(window)) {
        // Framerate limit
        glfwPollEvents();

        // Render the game
        render(*level, *levelRenderer);

        // Update the display
        glfwSwapBuffers(window);
    }

    // Destroy I/O and save game
    destroy(level);
}

int main(void) {
    Level level;
    LevelRenderer levelRenderer;

    // Run the game
    run(&level, &levelRenderer);

    return EXIT_SUCCESS;
}
