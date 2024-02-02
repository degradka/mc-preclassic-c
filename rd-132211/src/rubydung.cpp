// rubydung.cpp

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "window/window.h"
#include "window/events.h"

#include "level/level.h"
#include "level/levelrenderer.h"

int WIDTH = 1024;
int HEIGHT = 768;

int main() {
    Window::initalize(WIDTH, HEIGHT, "RubyDung");
    Events::initialize();

    Events::setMouseGrabbed(true);

    // Create level and level renderer
    Level level(256, 64, 256);
    LevelRenderer levelRenderer(&level);

    while (!Window::isCloseRequested()) {
        Events::pollEvents();
        if (Events::justPressed(GLFW_KEY_ESCAPE)) {
            Window::setCloseRequested(true);
        }

        // Reset camera
        glLoadIdentity();

        // Move camera to middle of level
        glTranslated(-level.width / 2.0, -level.depth / 2.0 - 3, -level.height / 2.0);

        // Render level chunks
        levelRenderer.render();

        Window::update();
    }
    Window::terminate();
    return 0;
}