// initialize-vulkan.cpp

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <set>
#include "include.h"
#include "application.h"
#include "debugger.h"

void Application::initVulkan() {
    
    Application::createInstance();
    Application::setupDebugMessenger();
    Application::createSurface();
    Application::pickPhysicalDevice();
    Application::createLogicalDevice();

}

void Application::createInstance() {

    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // The following if-else statement not only describes validation layers if enabled, but simaltaneously
    // allows the existance of validation debug in this createInstance function.
    // 
    // "The debugCreateInfo variable is placed outside the if statement to ensure that it is not destroyed 
    // before the vkCreateInstance call. By creating an additional debug messenger this way it will automatically
    // be used during vkCreateInstance and vkDestroyInstance and cleaned up after that.
    //
    // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Validation_layers

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;

    }
    else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // The standard "pattern" for the creation of Vulkan objects is as follows:
        // - Pointer to the creation info struct
        // - Pointer to custom allocator callbacks
        // - Pointer to the variable that handles the object
    // Almost all Vulkan functions return a VkResult of either VK_SUCCESS or an error code, which can be taken
    // advantage of like so:

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

void Application::createSurface() {

    /* You could do this with platform specific notation, but GLFW has a special function anyways. It would look like this:
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = glfwGetWin32Window(window);
    createInfo.hinstance = GetModuleHandle(nullptr);

    if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    */

    // Creates a "VkSurfaceKHR," or an "object that represents an abstract type of surface to present rendered images to."
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }

}

QueueFamilyIndices Application::findQueueFamilies(VkPhysicalDevice device) {

    QueueFamilyIndices indices;
    int i = 0;

    // Retrieving the list of queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    // Checking for surface presentation support is slightly different than checking for Vulkan graphics support
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

    // Find a queue family that supports Vulkan graphics drawing and a queue family that supports presentation to a surface
    // It's very likely these two queue families will be the same, and I should probably add logic that prefers those over two seperate ones for performance
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) { indices.graphicsFamily = i; }
        if (presentSupport) { indices.presentFamily = i; }
        if (indices.isComplete()) { break; }
        i++;
    }
    return indices;
}

void Application::pickPhysicalDevice() {

    // The graphics card that we'll end up selecting will be stored in a VkPhysicalDevice handle that is added as a new class member. 
    // This object will be implicitly destroyed when the VkInstance is destroyed, so we won't need to do anything new in the cleanup function.

    // List compatible graphics cards
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    // If none found, throw an error, because the program would crash anyways
    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    // Array that holds all of the VkPhysicalDevice handles
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // List all physical devices found to the console. Should I even include it?
    // Only looks clean with validation layers off, will fix somehow soon?

    VkPhysicalDeviceProperties deviceProperties;
    int i = 1;

    for (const auto& device : devices) {
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        std::clog << "\nPHYSICAL DEVICE " << i << "\n"
        << "Device Name:      " << deviceProperties.deviceName << "\n"
        << "API Version:      " << deviceProperties.apiVersion << "\n"
        << "Driver Version:   " << deviceProperties.driverVersion << "\n"
        << "Vendor ID:        " << deviceProperties.vendorID << "\n"
        << "Device ID:        " << deviceProperties.deviceID << "\n";
        i++;
    }

    // Choose suitable device
    for (const auto& device : devices) {
        if (isDeviceSuitable(device)) {
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            std::cout << "\nSuitable Device Chosen: " << deviceProperties.deviceName << "\n\n";
            physicalDevice = device;
            break;
        }
    }
    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void Application::createLogicalDevice() {

    /* 
    The creation of a logical device involves specifying a bunch of details in structs again,
    of which the first one will be VkDeviceQueueCreateInfo. This structure describes the number 
    of queues we want for a single queue family. Right now we're only interested in a queue 
    with graphics capabilities.

    The currently available drivers will only allow you to create a small number of queues for each queue family and 
    you don't really need more than one. That's because you can create all of the command buffers on multiple threads 
    and then submit them all at once on the main thread with a single low-overhead call.
    */

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    VkQueue presentQueue;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    // If there are multiple queue families, a VkDeviceQueueCreateInfo struct must be created for them all.
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    // Vulkan lets you assign priorities to queues to influence the scheduling of command buffer execution using floating 
    // point numbers between 0.0 and 1.0. This is required even if there is only a single queue.
    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;

    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

    if (enableValidationLayers) {

        // Previous Vulkan versions made a distinction between instance and device specific validation layers.
        // While this is no longer the case, enabledLayerCount and ppEnabledLayerNames should still be set to maintain
        // backwards compatibility.

        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    // Instantiate the logical device
    
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    // We can use the vkGetDeviceQueue function to retrieve queue handles for each queue family. The parameters are the logical device, 
    // queue family, queue index and a pointer to the variable to store the queue handle in. Because we're only creating a single queue 
    // from this family, we'll simply use index 0.

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);

}