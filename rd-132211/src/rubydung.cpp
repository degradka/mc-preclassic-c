// rubydung.cpp

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "window/window.h"
#include "window/events.h"

#include "level/level.h"
#include "level/levelrenderer.h"

#include "player.h"

int WIDTH = 1024;
int HEIGHT = 768;

void moveCameraToPlayer(Player& player) {
    // Eye height
    glTranslatef(0.0f, 0.0f, -0.3f);

    // Rotate camera
    glRotatef(player.xRotation, 1.0f, 0.0f, 0.0f);
    glRotatef(player.yRotation, 0.0f, 1.0f, 0.0f);

    // Move camera to player's location
    glTranslatef(-player.x, -player.y, -player.z);
}

int main() {
    Window::initalize(WIDTH, HEIGHT, "RubyDung");
    Events::initialize();

    Events::setMouseGrabbed(true);

    // Create level and level renderer
    Level level(256, 64, 256);
    LevelRenderer levelRenderer(&level);

    Player player(&level);

    while (!Window::isCloseRequested()) {
        Events::pollEvents();
        if (Events::justPressed(GLFW_KEY_ESCAPE)) {
            Window::setCloseRequested(true);
        }

        // Get mouse motion
        float motionX = Events::getMouseMotionX();
        float motionY = Events::getMouseMotionY();

        // Rotate the player using the mouse motion input
        player.turn(motionX, motionY);

        // Clear color and depth buffer and reset camera
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        // Move camera to middle of level
        moveCameraToPlayer(player);

        // Render level chunks
        levelRenderer.render();

        Window::swapBuffers();
    }
    Window::terminate();
    return 0;
}