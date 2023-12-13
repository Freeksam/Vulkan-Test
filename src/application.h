// application.h

#pragma once

#include "queue-family-indices.h"
#include <vector>

class Application {

public:

	void run();

private:

	// Declarations for functions should be done best reflecting order of execution
	// If a member function calls other functions half way through its own execution, 
	// that function should be declared before the member function using it is.

	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;
	
	VkDevice device;
	VkSurfaceKHR surface;
	GLFWwindow* window;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue;

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
		VkDebugUtilsMessageTypeFlagsEXT messageType, 
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, 
		void* pUserData);

	void initWindow();

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	std::vector<const char*> getRequiredExtensions();
	bool checkValidationLayerSupport();
	void createInstance();

	void setupDebugMessenger();

	void createSurface();
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
	void pickPhysicalDevice();
	void createLogicalDevice();
	void initVulkan();
	
	void mainLoop();

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	void cleanup();
	
};