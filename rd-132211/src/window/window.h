// window/window.h

#ifndef WINDOW_WINDOW_H_
#define WINDOW_WINDOW_H_

class GLFWwindow;

class Window {
public:
    static GLFWwindow* window;
    static int initalize(int width, int height, const char* title);
    static void terminate();

    static bool isCloseRequested();
    static void setCloseRequested(bool flag);
    static void swapBuffers();

    static void update();
};

#endif // WINDOW_WINDOW_H_