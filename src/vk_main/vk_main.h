#ifndef __VK_MAIN_H__
#define __VK_MAIN_H__

#include <glfw_window.h>

#include <vulkan/vulkan.h>

#include <string>
#include <tuple>
#include <vector>
#include <optional>

namespace vulkan_impl {

inline std::string
retrieve_version_string(uint32_t version) {
	auto version_maj = VK_VERSION_MAJOR(version);
	auto version_min = VK_VERSION_MINOR(version);
	auto version_pat = VK_VERSION_PATCH(version);

	std::string version_str =
		std::to_string(version_maj) + '.' +
		std::to_string(version_min) + '.' +
		std::to_string(version_pat);
	
	return version_str;
}

struct VulkanMainConfig {
	uint16_t window_width					= 1280;
	uint16_t window_height					= 720;
	std::string window_title				= "Vulkan Window";
	std::string vulkan_app_name				= "Vulkan Application";
	uint32_t vulkan_app_version				= VK_MAKE_VERSION(1, 0, 0);
	std::string vulkan_engine_name			= "Vulkan Engine";
	uint32_t vulkan_engine_version			= VK_MAKE_VERSION(1, 0, 0);
	uint32_t vulkan_api_version				= VK_API_VERSION_1_2;
	std::vector<const char*>
		validation_layers 					= { "VK_LAYER_KHRONOS_validation" };
	bool enable_validation_layer			= true;
};

// Quick reference:

/*
 *		KHR suffix: Command comes from an extension
 *
 *		VkInstance				- Vulkan instance/context 
 *		VkPhysicalDevice		- Physical device, like a GPU
 *		VkDevice				- Vulkan's logical device, where things are executed
 *		VkBuffer				- Chunk of GPU visible memory
 *		VkImage					- A texture that you can read/write
 *		VkPipeline				- Holds the state of the GPU (shaders, etc.)
 *		VkRenderPass			- Holds information about images you are rendering to
 *		VkFrameBuffer			- Holds target image (a framebuffer...)
 *		VkCommandBuffer			- GPU commands
 *		VkQueue					- GPU's have queues to execute different commands
 *		VkDescriptorSet			- Basically a GPU-side pointer (connects shaders, textures)
 *		VkSwapchainKHR			- Holds image for screen (to render to visible window)
 *		VkSemaphore				- Synchronizes GPU to GPU execution of commands
 *		VkFence					- Synchronizes GPU to CPU execution of commands
 */

// Main Vulkan entry (Initialization code)
class VulkanMain {
public:
	VulkanMain(const VulkanMainConfig& config = {});
	void init();
	void fini();
private:
	struct QueueFamilyData {
		std::optional<uint32_t> graphics_family;

		bool is_complete() {
			return graphics_family.has_value();
		}
	};

	void vulkan_instance_init();
	std::vector<const char*> get_required_extensions();
	bool verify_validation_layer_support();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
	void init_debug_calls();
	void query_physical_devices();
	bool verify_device(VkPhysicalDevice device);
	QueueFamilyData verify_queue_families(VkPhysicalDevice device);

	VulkanMainConfig config;
	VkInstance vkm_instance { VK_NULL_HANDLE };
	VkDebugUtilsMessengerEXT debug_messenger { VK_NULL_HANDLE };
	VkPhysicalDevice vkm_physical_device { VK_NULL_HANDLE };
	VkDevice vkm_logical_device { VK_NULL_HANDLE };
	GlfwWrapper window;
};


} // namespace vulkan_impl

#endif // __VK_MAIN_H__