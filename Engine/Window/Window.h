//
// Created by Owner on 2/18/2022.
//

#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include "../../VKInclude/vulkan/vulkan.h"
#include "../../GLFW/include/glfw3.h"
#include "../Instance/Instance.h"
#include "../Infos/Infos.hpp"

namespace ws {
    // Window Wrapper Class
    class Window {
    private:
        // Window Object
        GLFWwindow* m_Window;
        // Vulkan Surface Object
        VkSurfaceKHR m_Surface;
    public:
        // Initializes Window And Surface Object, @instace Is ws::Instance Class, @width is width of window, @height is height of window
        Window(ws::Instance& instance, u32 width, u32 height);
        // Returns True If Window Is Currently Open
        bool isOpen();
        // Swaps Framebuffers Of Window
        void swapBuffers();
        // Polls Events Gathered By Window
        void pollEvents();
        // Returns Window Object
        GLFWwindow* get();
        // Returns Surface Object
        VkSurfaceKHR getSurface();
        // Returns VkExtent2D Of Window's Size
        VkExtent2D size() const;
        // Returns 2 Coordinates Of Cursors Current Position
        v2<int> getMousePos();
        // Returns If @keycode Is Pressed
        bool keyDown(int keycode);
    };
}


#endif //ENGINE_WINDOW_H
