// window/window.cpp

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "window.h"

GLFWwindow* Window::window;

int Window::initalize(int width, int height, const char* title) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW Window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
    }

    // Setup rendering
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.5F, 0.8F, 1.0F, 0.0F);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);

    // Setup camera
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Setup perspective
    gluPerspective(70, width / (float) height, 0.05f, 1000);
    glMatrixMode(GL_MODELVIEW);

    glViewport(0, 0, width, height);

    return 0;
}

void Window::terminate() {
    glfwTerminate();
}

bool Window::isCloseRequested() {
    return glfwWindowShouldClose(window);
}

void Window::setCloseRequested(bool flag) {
    glfwSetWindowShouldClose(window, flag);
}

void Window::swapBuffers() {
    glfwSwapBuffers(window);
}

void Window::update() {
    if (glfwGetWindowAttrib(window, GLFW_VISIBLE)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(window);
    }
}