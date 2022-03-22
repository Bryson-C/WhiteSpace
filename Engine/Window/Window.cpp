//
// Created by Owner on 2/18/2022.
//

#include "Window.h"

namespace ws {
    static void         resizeCallback(GLFWwindow*, int, int) {

    }

                        Window::Window(ws::Instance& instance, uint32_t width, uint32_t height) {
        auto StartTime = clock();
        m_Window = glfwCreateWindow(width, height, instance.getName(), nullptr, nullptr);
        glfwCreateWindowSurface(instance.get(), m_Window, nullptr, &m_Surface);
        glfwSetFramebufferSizeCallback(m_Window, resizeCallback);
        printf("Window: %lims\n", clock()-StartTime);
    }
    bool                Window::isOpen() { return !glfwWindowShouldClose(m_Window); }
    void                Window::swapBuffers() { glfwSwapBuffers(m_Window); }
    void                Window::pollEvents() { glfwPollEvents(); }
    GLFWwindow*         Window::get() { return m_Window; }
    VkSurfaceKHR        Window::getSurface() { return m_Surface; }
    VkExtent2D          Window::size() const {
        VkExtent2D extent;
        glfwGetFramebufferSize(m_Window, reinterpret_cast<int *>(&extent.width), reinterpret_cast<int *>(&extent.height));
        return extent;
    }
    v2<int>             Window::getMousePos() {
        double x,y;
        glfwGetCursorPos(m_Window, &x, &y);
        return {(int)x,(int)y};
    }
    bool                Window::keyDown(int keycode) {
        return glfwGetKey(m_Window, keycode);
    }
}
