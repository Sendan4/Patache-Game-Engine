#if defined(DEBUG)
#pragma once
#include <cstdint>
#include <string>

#include <vulkan/vulkan.hpp>
#include <SDL_syswm.h>

namespace Patata {
    struct EngineInfo {
        ~EngineInfo(void) {
            // Vulkan Instance Extensions
            if (VkInstanceExtensions != nullptr)
                delete[]  VkInstanceExtensions;

            // Vulkan Device Extensions
            if (VkDeviceExtensions != nullptr)
                delete[]  VkDeviceExtensions;
        }
        // Window
        #if defined(__linux__)
        std::string Desktop = "";
        std::string SessionType = "";
        #endif
        SDL_SysWMinfo WindowInfo;
        uint32_t WindowCreationFlags = 0;

        // RaccoonRenderer
        // Vulkan
        std::string VkVersionInUse = "";
        // Device
        std::string VkDeviceName = "";
        uint32_t VkDeviceVendorId = 0;
        std::string VkDeviceType = "";
        // Driver
        std::string VkDriverName = "";
        std::string VkDriverId = "";
        std::string VkDriverInfo = "";
        std::string VkDriverVersion = "";
        // Swapchain
        vk::PresentModeKHR VkSwapchainPresentMode;
        vk::Format VkSwapchainImageColorFormat;
        vk::ColorSpaceKHR VkSwapchainImageColorSpace;
        // Extensions
        const char ** VkInstanceExtensions = nullptr;
        uint32_t VkInstanceExtensionsCount = 0;
        const char ** VkDeviceExtensions = nullptr;
        uint32_t VkDeviceExtensionsCount = 0;

        // RaccoonSound
    };
}
#endif
