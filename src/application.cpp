// application.cpp

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include "include.h"
#include "application.h"
#include "debugger.h"

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

void Application::run() {

    initWindow();
    Application::initVulkan();
    mainLoop();
    cleanup();

}

std::vector<const char*> Application::getRequiredExtensions() {

    // Setting up a debug messenger to handle debug messages explicitly rather than letting
    // Vulkan implicitly handle them (Vulkan is poopoo at debug messages by default, lump phao)

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    // Enables the VK_EXT_debug_utils extension if validation layers are enabled

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void Application::initWindow() {

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);          // Specifies that GLFW will not use an OpenGL context
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT,
        "Vulkan",    // Window title
        nullptr,     // Specifies a monitor
        nullptr      // Relevant only to OpenGL
    );

}

void Application::mainLoop() {

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void Application::cleanup() {

    vkDestroyDevice(device, nullptr);

    // All children of an instance must be destroyed before that instance is destroyed
    if (enableValidationLayers) {
        Application::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    

    glfwDestroyWindow(window);
    glfwTerminate();
}