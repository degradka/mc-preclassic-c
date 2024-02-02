// rubydung.cpp

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "window/window.h"
#include "window/events.h"

int WIDTH = 1024;
int HEIGHT = 768;

int main() {
    Window::initalize(WIDTH, HEIGHT, "RubyDung");
    Events::initialize();

    Events::setMouseGrabbed(true);

    while (!Window::isCloseRequested()) {
        Events::pollEvents();
        if (Events::justPressed(GLFW_KEY_ESCAPE)) {
            Window::setCloseRequested(true);
        }

        Window::update();
    }
    Window::terminate();
    return 0;
}