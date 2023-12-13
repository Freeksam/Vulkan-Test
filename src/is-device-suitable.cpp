// is-device-suitable.cpp

#include "include.h"
#include "application.h"

// This function has it's own source file for easier editing of it's parameters. When additions to
// the other source files have been made that include new Vulkan features, those features will have to
// be include here.

bool Application::isDeviceSuitable(VkPhysicalDevice device) {

    // Determine if a given GPU has the required features for this program
    // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families
    // Section - "Base Suitabilities Checks"

    // The following makes sure that the device can process the commands we want to use

    QueueFamilyIndices indices = findQueueFamilies(device);

    return indices.isComplete();

}