#include <glfw_window.h>
#include <logger.h>

#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>

constexpr auto TRIANGLE_WINDOW_WIDTH 	= 1280;
constexpr auto TRIANGLE_WINDOW_HEIGHT 	= 720;

const std::vector<const char*> validation_layers = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
	constexpr bool enable_validation_layers = false;
#else
	constexpr bool enable_validation_layers = true;
#endif

class VulkanTriangle {
public:
	void triangle_init() {
		// GLFW Window Initialization
		window.init(TRIANGLE_WINDOW_WIDTH, TRIANGLE_WINDOW_HEIGHT,
			"Vulkan Triangle");
		
		vulkan_instance_init();

		window.glfw_loop();
	}

	void triangle_fini() {
		vulkan_instance_fini();

		window.shutdown();
	}
private:
	struct QueueIndices {
		uint32_t graphics_ident;
	};

	void vulkan_instance_init() {
		if (enable_validation_layers) {
			if (!check_validation_layer_support()) {
				throw std::runtime_error("Requested validation layers are not available");
			}
		}

		// General app information, technically optional
		VkApplicationInfo app_info = {};
		// Structure type (in this case it is Application Info)
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		// Pointer to an extension-specific structure, not needed
		app_info.pNext = nullptr;
		// Name of application
		app_info.pApplicationName = "Vulkan Triangle";
		// Version number of our application
		app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		// Name of the engine used to create our application
		app_info.pEngineName = "No Engine This Time";
		// Version number of our engine
		app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		// HAS TO BE the highest version of Vulkan we are designed to use,
		// We'll use the highest one currently available (nothing stopping us).
		app_info.apiVersion = VK_API_VERSION_1_2;

		// Retrieve some information on extensions
		// (Vulkan, being cross platform, requires extensions to interface
		// with the window system, so here, we get them from our window
		// system (aka GLFW)).
		uint32_t glfw_extension_count = 0;
		const char **glfw_extensions = nullptr;
		// Ping glfw for the info
		glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

		// Query supported extensions and compare them with the required
		vulkan_query_supported_extensions(glfw_extensions, glfw_extension_count);

		// Required, communicate instance information, like extensions
		VkInstanceCreateInfo create_info = {};
		// Structure type (in this case Instance Info)
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		// Pointer to an extension-specific structure, not needed
		create_info.pNext = nullptr;
		// Reserved for future use
		// create_info.flags = 
		// Pass in our application information
		create_info.pApplicationInfo = &app_info;

		if (enable_validation_layers) {
			// Number of validation layers to enable
			create_info.enabledLayerCount = static_cast<uint32_t> (validation_layers.size());
			// Names of validation layers to enable for our vulkan instance
			create_info.ppEnabledLayerNames = validation_layers.data();
		}
		else {
			create_info.enabledLayerCount = 0;
			create_info.ppEnabledLayerNames = nullptr;
		}

		// Number of global extensions to enable
		create_info.enabledExtensionCount = glfw_extension_count;
		// Names of global extensions to enable
		create_info.ppEnabledExtensionNames = glfw_extensions;

		// Now, we have all the information to create the Vulkan instance.
		auto result = vkCreateInstance(&create_info, nullptr, &vk_instance);

		if (result != VK_SUCCESS) {
			LOG_MSG(LL::ERROR) << "Failed to create Vulkan instance, code: " << result << '\n';
			throw std::runtime_error("Failed to create vulkan instance");
		}

		// Select our graphics device
		vulkan_device_select();
	}

	void vulkan_query_supported_extensions(const char ** const glfw_required, uint32_t count) {
		uint32_t extension_count = 0;
		// Get extension number (amount of them)
		vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

		std::vector<VkExtensionProperties> extension_list(extension_count);

		// Populate vector with extension list
		vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_list.data());

		// Function to compare supported extensions with the required extensions
		auto cmp_exts = [](const char * required_ext, std::vector<VkExtensionProperties> *
			supported_exts) -> bool {
			ASSERT(supported_exts != nullptr);
			for (const auto& e : *supported_exts) {
				if (strcmp(e.extensionName, required_ext) == 0) {
					return true;
				}
			}

			return false;
		};
		// Supported Vulkan extensions (from vector)
		LOG_MSG(LL::INFO) << "Supported Vulkan extensions: \n";
		for (const auto& extension : extension_list) {
			std::cout << '\t' << extension.extensionName << '\n';
		}
		// Required vulkan extensions (from glfw)
		LOG_MSG(LL::INFO) << "Required Vulkan extensions: \n";
		for (uint32_t i = 0; i < count; ++i) {
			std::cout << '\t' << glfw_required[i] << '\n';
		}
		// Ensure we support all the REQUIRED extensions
		for (uint32_t i = 0; i < count; ++i) {
			if (!cmp_exts(glfw_required[i], &extension_list)) {
				LOG_MSG(LL::ERROR) <<
					"The current platform does not support the required Vulkan extension("
					<< glfw_required[i] << ")\n";
				throw std::runtime_error("Platform does not support required Vulkan extension");
			}
		}
	}

	bool check_validation_layer_support() {
		uint32_t layer_count;
		vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

		std::vector<VkLayerProperties> available_layers(layer_count);
		vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

		auto layer_find = [](const char *layer_name, std::vector<VkLayerProperties>
			*validation_layers) -> bool {
			ASSERT(validation_layers != nullptr);
			for (const auto& e : *validation_layers) {
				if (strcmp(e.layerName, layer_name) == 0) {
					return true;
				}
			}

			return false;
		};

		for (auto layer_name : validation_layers) {
			if (!layer_find(layer_name, &available_layers)) {
				return false;
			}
		}

		return true;
	}

	void vulkan_device_select() {
		uint32_t device_count = 0;
		// Get device number
		vkEnumeratePhysicalDevices(vk_instance, &device_count, nullptr);

		if (device_count == 0) {
			throw std::runtime_error("Failed to find device with Vulkan support");
		}

		// Get device list
		std::vector<VkPhysicalDevice> devices(device_count);
		vkEnumeratePhysicalDevices(vk_instance, &device_count, devices.data());

		// Iterate through devices to find a suitable one
		for (const auto& device : devices) {
			if (vulkan_verify_device(device)) {
				vk_physical_device = device;
				break;
			}
		}

		if (vk_physical_device == VK_NULL_HANDLE) {
			throw std::runtime_error("Failed to find a suitable device");
		}

		VkPhysicalDeviceProperties device_properties;
		vkGetPhysicalDeviceProperties(vk_physical_device, &device_properties);

		LOG_MSG(LL::INFO) << "Graphics device info: " << '\n';
		std::cout << '\t' << device_properties.deviceName << '\n';

	}

	bool vulkan_verify_device(VkPhysicalDevice device) {
		/*VkPhysicalDeviceProperties device_properties;
		VkPhysicalDeviceFeatures device_features;

		vkGetPhysicalDeviceProperties(device, &device_properties);
		vkGetPhysicalDeviceFeatures(device, &device_features);

		bool sst = (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
			device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ||
			device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU);*/
		UNUSED(device);
		return true;
	}

	QueueIndices vulkan_find_queue(VkPhysicalDevice device) {
		UNUSED(device);
		QueueIndices indices;

		return indices;
	}

	void vulkan_instance_fini() {
		vkDestroyInstance(vk_instance, nullptr);
	}

	VkInstance vk_instance { VK_NULL_HANDLE };
	VkPhysicalDevice vk_physical_device { VK_NULL_HANDLE };
	GlfwWrapper window;
};

int main() {
	VulkanTriangle vulkan_triangle;
	vulkan_triangle.triangle_init();
	vulkan_triangle.triangle_fini();

	return 0;
}