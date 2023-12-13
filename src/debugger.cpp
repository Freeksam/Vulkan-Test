// debugger.cpp

#include "include.h"
#include "application.h"
#include "debugger.h"
#include <stdexcept>
#include <iostream>

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

bool Application::checkValidationLayerSupport() {

    // The following protected members are protected so that initialize-vulkan.cpp may use them.
    // Serves no functional use aside from organization so that I don't have one massive main.cpp

    // Essentially, explicit error declaration support ensuring that validationLayers - if enabled - are working as intended

    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);

    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

void Application::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {

    // The following function populates the creation information for the Debug Messanger instance. It is seperated from the
    // setupDebugMessenger function so that it may be used in the scope of vkCreateInstance and vkDestroyInstance, per the
    // extension documentation: https://github.com/KhronosGroup/Vulkan-Docs/blob/main/appendices/VK_EXT_debug_utils.adoc#examples

    createInfo = {};

    // The message severity parameter has VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT left out (disabled) to 
    // avoid recieving general, annoying, and mostly useless debug info

    createInfo.messageSeverity = (
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT

        );

    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional
}

void Application::setupDebugMessenger() {

    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL Application::debugCallback(

    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,

    // Specifies severity of the message, one of:
    //
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT      Diagnostic message
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT         Informational message like the creation of a resource
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT      Message about behavior that is not necessarily an error, but very likely a bug in your application
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT        Message about behavior that is invalid and may cause crashes

    VkDebugUtilsMessageTypeFlagsEXT messageType,

    // Specifies type of the message, one of:

    // VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT          Some event has happened that is unrelated to the specification or performance
    // VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT       Something has happened that violates the specification or indicates a possible mistake
    // VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT      Potential non - optimal use of Vulkan

    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,

    // The pCallbackData parameter refers to a VkDebugUtilsMessengerCallbackDataEXT struct containing the details of the message itself,
    // the most important members being:
    //
    // pMessage         The debug message as a null - terminated string
    // pObjects         Array of Vulkan object handles related to the message
    // objectCount      Number of objects in array

    void* pUserData

    // The pUserData parameter contains a pointer that was specified during the setup of the callback, with which custom information can be passed

) {

    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void Application::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {

    // This is a great example of the utility of the "auto" keyword.
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    /*
    could instead be:


    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

    But that would be incredibly redundant. In essence, use auto if the variable's declaration is casted explicitly anyways, so you don't have to
    explicitly declare it's type multiple times when it's type is obvious.
    */

    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}