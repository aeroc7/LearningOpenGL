#ifndef __VK_MAIN_H__
#define __VK_MAIN_H__

#include <vulkan/vulkan.h>

namespace vulkan_impl {

// Quick reference:

/*
 *		KHR suffix: Command comes from an extension
 * 
 *      VkInstance              - Vulkan instance/context 
 *      VkPhysicalDevice        - Physical device, like a GPU
 *      VkDevice                - Vulkan's logical device, where things are executed
 *      VkBuffer                - Chunk of GPU visible memory
 *      VkImage                 - A texture that you can read/write
 *      VkPipeline              - Holds the state of the GPU (shaders, etc.)
 * 		VkRenderPass			- Holds information about images you are rendering to
 * 		VkFrameBuffer			- Holds target image (a framebuffer...)
 * 		VkCommandBuffer			- GPU commands
 * 		VkQueue					- GPU's have queues to execute different commands
 * 		VkDescriptorSet			- Basically a GPU-side pointer (connects shaders, textures)
 * 		VkSwapchainKHR			- Holds image for screen (to render to visible window)
 * 		VkSemaphore				- Synchronizes GPU to GPU execution of commands
 * 		VkFence					- Synchronizes GPU to CPU execution of commands
 */

// Main Vulkan entry (Initialization code)
class VulkanMain {
public:
	void init();
	void fini();
private:

};


} // namespace vulkan_impl

#endif // __VK_MAIN_H__