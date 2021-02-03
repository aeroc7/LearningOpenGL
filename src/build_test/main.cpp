#include <glfw_window.h>
#include <logger.h>

#include <vulkan/vulkan.h>

int main() {
	GlfwWrapper window;

	window.init(1280, 720, "Vulkan Build Test");

	uint32_t extensions_num = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensions_num, nullptr);

	LOG_MSG(LL::INFO) << extensions_num << '\n';

	window.glfw_loop();

	return 0;
}