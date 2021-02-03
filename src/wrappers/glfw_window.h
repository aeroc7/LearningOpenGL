#ifndef _GLFW_WINDOW_H_
#define _GLFW_WINDOW_H_

#include <GLFW/glfw3.h>

#include <string>

class GlfwWrapper {
public:
	GlfwWrapper();
	void init(int width, int height, const std::string& title);
	void shutdown();
	void glfw_loop();
	~GlfwWrapper();
private:
	GLFWwindow * window { nullptr };
};

#endif // _GLFW_WINDOW_H_