#include "vk_main.h"

int main() {
	vulkan_impl::VulkanMain main_prog;

	main_prog.init();
	main_prog.fini();
	
	return 0;
}