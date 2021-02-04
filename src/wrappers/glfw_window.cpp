#include "glfw_window.h"
#include "logger.h"

#include <stdexcept>

void
GlfwWrapper::init(int width, int height, const std::string& title) {
	if (!glfwInit()) {
		LOG_MSG(LL::ERROR) << "Failed to initialize glfw\n";
		exit(EXIT_FAILURE);
	}
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	
	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	if (window == nullptr) {
		throw std::runtime_error("Failed to create GLFW window");
	}
	
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetWindowUserPointer(window, this);
}

void
GlfwWrapper::glfw_loop() {
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}

void
GlfwWrapper::shutdown() {
	if (window != nullptr) {
		glfwDestroyWindow(window);
		window = nullptr;
	}

	glfwTerminate();
}