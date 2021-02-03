#include <glfw_window.h>
#include <logger.h>

#include <vulkan/vulkan.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

int main() {
    GlfwWrapper window;

    window.init(1280, 720, "Vulkan Build Test");

    uint32_t extensions_num = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensions_num, nullptr);

    LOG_MSG(LL::INFO) << extensions_num << '\n';

    window.glfw_loop();

    return 0;
}