#include "glfw_window.h"
#include "logger.h"

#include <stdexcept>

GlfwWrapper::GlfwWrapper() {
    if (!glfwInit()) {
		LOG_MSG(LL::ERROR) << "Failed to initialize glfw\n";
	    exit(EXIT_FAILURE);
	}
}

void
GlfwWrapper::init(int width, int height, const std::string& title) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

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
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void
GlfwWrapper::shutdown() {
	if (window != nullptr) {
		glfwDestroyWindow(window);
		window = nullptr;
	}
}

GlfwWrapper::~GlfwWrapper() {
    glfwTerminate();
}