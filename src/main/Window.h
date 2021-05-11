#ifndef PONG_WINDOW_H
#define PONG_WINDOW_H

#include "glad.h"

#include <GLFW/glfw3.h>

#include <cstdint>
#include <iostream>

class Window {
public:
    Window(std::uint32_t width, std::uint32_t height) {

        if (!glfwInit()) {
            std::cout << "Could not init GLFW";
            exit(EXIT_FAILURE);
        }

#if defined(__WIN32__)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#elif defined(__APPLE__)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    
        glfwWindowHint(GLFW_DEPTH_BITS, 16);
        glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

        glfwWindow = glfwCreateWindow(width, height, "pong", nullptr, nullptr);
        if (glfwWindow == nullptr) {
            glfwTerminate();
            std::cout << "Could not open window";
            exit(EXIT_FAILURE);
        }

        glfwMakeContextCurrent(glfwWindow);
        glfwSwapInterval(1);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD";
            exit(EXIT_FAILURE);
        }
    }

    GLFWwindow* getGlfwWindow() {
        return glfwWindow;
    }

    void setShouldClose() {
        glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);
    }

    bool shouldClose() {
        return glfwWindowShouldClose(glfwWindow);
    }

    void update() {
        glfwPollEvents();
        glfwSwapBuffers(glfwWindow);
    }

private:
    GLFWwindow* glfwWindow;
};

#endif // PONG_WINDOW_H
