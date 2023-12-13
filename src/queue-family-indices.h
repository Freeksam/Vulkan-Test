// queue-family-indices.h

#pragma once

#include <optional>

struct QueueFamilyIndices {
    // std::optional is a wrapper that contains no value until it is assigned. 
    std::optional<uint32_t> graphicsFamily; // Queue families that support Vulkan graphics
    std::optional<uint32_t> presentFamily; // Queue families that support presentation to a surface

    bool isComplete() {
        // Use has_value() to determine if an optional-type variable has a value
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};