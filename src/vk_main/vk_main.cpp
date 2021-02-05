#include "vk_main.h"

#include <glfw_window.h>
#include <logger.h>

#include <GLFW/glfw3.h>

namespace vulkan_impl {

/*		Engine Initialization Steps:
 * 
 * 			1.) Create the Vulkan instance (VkInstance)
 * 			2.) Query VkPhysicalDevice list to find a suitable device
 * 			3.) Create VkDevice (logical device) from the physical device
 * 			4.) Get VkQueue handles from the created VkDevice
 * 
 */

VkResult
CreateDebugUtilsMessengerEXT(VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
	const VkAllocationCallbacks *pAllocator,
	VkDebugUtilsMessengerEXT *pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
		vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void
DestroyDebugUtilsMessengerEXT(VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
		vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

VulkanMain::VulkanMain(const VulkanMainConfig& config) {
	this->config = config;
}

void
VulkanMain::init() {
	window.init(config.window_width, config.window_height,
		config.window_title);

	// Validation layer support ON by default
	if (config.enable_validation_layer) {
		if (!verify_validation_layer_support()) {
			QUICK_EXIT("Could not find requested validation layer in supported layers");
		}
	}

	// Init our vulkan instance
	vulkan_instance_init();
	// Custom logging
	init_debug_calls();
	// Query device list
	query_physical_devices();
}

void
VulkanMain::vulkan_instance_init() {
	// Print off supported extensions
	uint32_t sup_exts_num = 0;

	vkEnumerateInstanceExtensionProperties(nullptr, &sup_exts_num, nullptr);

	std::vector<VkExtensionProperties> extension_names(sup_exts_num);
	vkEnumerateInstanceExtensionProperties(nullptr, &sup_exts_num, extension_names.data());

	LOG_MSG_NI() << "Supported Vulkan extensions: \n";

	for (const auto& e : extension_names) {
		LOG_MSG_NI() << '\t' << e.extensionName << '\n';
	}

	// Get required GLFW extensions
	auto reqd_exts = get_required_extensions();

	//Nice log message to show the required extensions
	LOG_MSG_NI() << "Required Vulkan extensions: \n";
	for (size_t i = 0; i < reqd_exts.size(); ++i) {
		LOG_MSG_NI() << '\t' << reqd_exts[i] << '\n';
	}

	// Setup our application info
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = config.vulkan_app_name.c_str();
	app_info.applicationVersion = config.vulkan_app_version;
	app_info.pEngineName = config.vulkan_engine_name.c_str();
	app_info.engineVersion = config.vulkan_engine_version;
	app_info.apiVersion = config.vulkan_api_version;

	// Setup our instance information
	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;

	if (config.enable_validation_layer) {
		create_info.enabledLayerCount = static_cast<uint32_t> (config.validation_layers.size());
		create_info.ppEnabledLayerNames = config.validation_layers.data();
	} else {
		create_info.enabledLayerCount = 0;
	}
	create_info.enabledExtensionCount = static_cast<uint32_t> (reqd_exts.size());
	create_info.ppEnabledExtensionNames = reqd_exts.data();

	// Create the Vulkan instance
	auto status = vkCreateInstance(&create_info, nullptr, &vkm_instance);

	if (status != VK_SUCCESS) {
		QUICK_EXIT(std::string("Failed to create Vulkan instance with error " +
			std::to_string(status)));
	}
}

std::vector<const char*>
VulkanMain::get_required_extensions() {
	uint32_t glfw_extension_count = 0;
	const char **glfw_extensions = nullptr;
	std::vector<const char*> ret_required_exts;

	glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

	for (uint32_t i = 0; i < glfw_extension_count; ++i) {
		ret_required_exts.push_back(glfw_extensions[i]);
	}

	if (config.enable_validation_layer) {
		ret_required_exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return ret_required_exts;
}

bool
VulkanMain::verify_validation_layer_support() {
	uint32_t avail_layer_count = 0;
	vkEnumerateInstanceLayerProperties(&avail_layer_count, nullptr);

	std::vector<VkLayerProperties> available_layers(avail_layer_count);
	vkEnumerateInstanceLayerProperties(&avail_layer_count, available_layers.data());

	for (const auto& cur_req_layer : config.validation_layers) {
		for (const auto& cur_a_layer : available_layers) {
			if (std::strcmp(cur_req_layer, cur_a_layer.layerName) == 0) {
				return true;
			}
		}
	}

	return false;
}

VKAPI_ATTR VkBool32 VKAPI_CALL
VulkanMain::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
	void *pUserData)
{
	ASSERT(pCallbackData != nullptr);
	ASSERT(pUserData != nullptr);

	auto msg_type_str = [](VkDebugUtilsMessageTypeFlagsEXT msg_type) -> std::string {
		switch (msg_type) {
			case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
				return "GENERAL";
			case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
				return "VIOLATION";
			case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
				return "PERFORMANCE";
			default:
				return "";
		}
	};

	switch (messageSeverity) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			LOG_MSG(LL::INFO) << "ERROR, " << msg_type_str(messageType) << ": " <<
				pCallbackData->pMessage << '\n';
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			LOG_MSG(LL::INFO) << "INFO, " << msg_type_str(messageType) << ": " <<
				pCallbackData->pMessage << '\n';
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			LOG_MSG(LL::INFO) << "WARNING, " << msg_type_str(messageType) << ": " <<
				pCallbackData->pMessage << '\n';
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			LOG_MSG(LL::INFO) << "VERBOSE, " << msg_type_str(messageType) << ": " <<
				pCallbackData->pMessage << '\n';
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
			break;
	}

	return VK_FALSE;
}

void
VulkanMain::init_debug_calls() {
	if (!config.enable_validation_layer) {
		return;
	}

	VkDebugUtilsMessengerCreateInfoEXT create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
	create_info.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		create_info.pfnUserCallback = debug_callback;
		create_info.pUserData = this;
	
	auto status = CreateDebugUtilsMessengerEXT(vkm_instance,
		&create_info, nullptr, &debug_messenger);
	
	if (status != VK_SUCCESS) {
		QUICK_EXIT("Failed to set up a Vulkan debug callback");
	}
}

void
VulkanMain::query_physical_devices() {
	uint32_t phys_device_count = 0;

	vkEnumeratePhysicalDevices(vkm_instance, &phys_device_count, nullptr);

	if (phys_device_count == 0) {
		QUICK_EXIT("Failed to find a device with Vulkan support");
	}

	std::vector<VkPhysicalDevice> phys_device_list(phys_device_count);
	vkEnumeratePhysicalDevices(vkm_instance, &phys_device_count, phys_device_list.data());

	for (const auto& device : phys_device_list) {
		if (verify_device(device)) {
			vkm_physical_device = device;
			break;
		}
	}

	if (vkm_physical_device == VK_NULL_HANDLE) {
		QUICK_EXIT("Failed to find a suitable device that supports Vulkan");
	}
}

bool
VulkanMain::verify_device(VkPhysicalDevice device) {
	ASSERT(device != VK_NULL_HANDLE);

	VkPhysicalDeviceProperties device_properties;
	vkGetPhysicalDeviceProperties(device, &device_properties);

	bool sst = (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
		device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ||
		device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU);

	auto qf_ret = verify_queue_families(device);
	
	if (!sst || !qf_ret.is_complete()) {
		return false;
	}
	
	LOG_MSG_NI() << "Vulkan Physical Device Information: \n";

	auto api_version_str = retrieve_version_string(device_properties.apiVersion);
	auto driver_version_str = retrieve_version_string(device_properties.driverVersion);
	auto device_name_str = device_properties.deviceName;

	LOG_MSG_NI() << "\tAPI Version: " << api_version_str << '\n';
	LOG_MSG_NI() << "\tDriver Version: " << driver_version_str << '\n';
	LOG_MSG_NI() << "\tDevice Name: " << device_name_str << '\n';

	return true;
}

VulkanMain::QueueFamilyData
VulkanMain::verify_queue_families(VkPhysicalDevice device) {
	uint32_t queue_family_count = 0;
	QueueFamilyData indices;

	vkGetPhysicalDeviceQueueFamilyProperties(device,
		&queue_family_count, nullptr);

	std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(device,
		&queue_family_count, queue_families.data());

	uint32_t i = 0;

	for (const auto& queue_family : queue_families) {
		if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphics_family = i;
			break;
		}

		++i;
	}

	return indices;
}

void
VulkanMain::fini() {
	if (config.enable_validation_layer) {
		DestroyDebugUtilsMessengerEXT(vkm_instance, debug_messenger, nullptr);
	}

	vkDestroyInstance(vkm_instance, nullptr);
	window.shutdown();
}

} // namespace vulkan_impl