// debugger.h

#pragma once

#include "include.h"

// Enables validation layers if debug mode is enabled. Otherwise, (i.e., release, build, etc), validation
// will be disabled and unstable or random behavior might propogate given errors in the build code relating
// to Vulkan functions. Validation layers are Vulkan debug messages that help decipher errors in the code
// that still compile. Some validation layer errors will only happen on deconstruction, so make sure to check
// the console after closing the application window.

extern const std::vector<const char*> validationLayers;
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(
	VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger
);

