#ifndef _GLFW_WINDOW_H_
#define _GLFW_WINDOW_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class GlfwWrapper {
public:
	void init(int width, int height, const std::string& title);
	void shutdown();
	void glfw_loop();
private:
	GLFWwindow * window { nullptr };
};

#endif // _GLFW_WINDOW_H_