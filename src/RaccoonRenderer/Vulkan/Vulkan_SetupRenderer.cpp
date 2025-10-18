#include "Vulkan_SetupRenderer.hpp"

bool
RaccoonRendererInit (Patache::Engine * pEngine, const Patache::EngineCreateInfo & rInfo)
{
#if PATACHE_DEBUG == 1
  std::future<void> initImguiCore_Async
      = std::async (std::launch::async, InitImGuiCore, std::cref (pEngine->configuration),
                    std::ref (pEngine->debugInfo));
#endif

  vk::Result result;

#if PATACHE_DEBUG == 1
  bool vkEXTDebugUtilsExtensionIsEnabled = false;
#endif

  // Verify vulkan version
  {
    PFN_vkVoidFunction PFN_vkEnumerateInstanceVersion
        = vkGetInstanceProcAddr (nullptr, "vkEnumerateInstanceVersion");

    if (PFN_vkEnumerateInstanceVersion == nullptr)
      {
        std::future<void> err = std::async (
            std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
            "Your drivers are too old, your vulkan version is unsupported. (Vulkan 1.1 <=)",
            std::cref (pEngine->configuration));

        return false;
      }

    std::uint32_t supportedVersion = 0;
    result                         = vk::enumerateInstanceVersion (&supportedVersion);

    if (result != vk::Result::eSuccess)
      {
        std::future<void> returnVulkanErr = std::async (std::launch::async, Patache::WarningMessage,
                                                        "Cannot get the supported vulkan version");
      }
    else
      {
        if (supportedVersion < 4202496)
          {
            std::future<void> err = std::async (
                std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
                "Your drivers are too old, your vulkan version is unsupported. (Vulkan 1.1 <=)",
                std::cref (pEngine->configuration));

            return false;
          }
      }
  }

  // Vulkan Instance
  {
    const char * const pGameName = (rInfo.pGameName != nullptr) ? rInfo.pGameName : "Untitled Game";

    const vk::ApplicationInfo patacheEngineInfo{ .pApplicationName   = pGameName,
                                                 .applicationVersion = rInfo.gameVersion,
                                                 .pEngineName        = PATACHE_ENGINE_NAME,
                                                 .engineVersion      = PATACHE_ENGINE_VERSION_VK,
                                                 .apiVersion         = VK_API_VERSION_1_2 };

    // The validation layers are activated with USE_VVL=ON.
    // They are for the development and testing of this backend.
    // Layers
#if PATACHE_DEBUG == 1 && defined(PATACHE_USE_VVL)
    pEngine->debugInfo.ppLayersVK = new const char *[PATACHE_LAYER_VK_COUNT](nullptr);

    pEngine->debugInfo.ppLayersVK[0] = "VK_LAYER_KHRONOS_validation";

    // List Layers
    {
      std::future<void> returnVulkanList
          = std::async (std::launch::async, Patache::VulkanList, pEngine->debugInfo.ppLayersVK,
                        PATACHE_LAYER_VK_COUNT, "Layers");
    }
#endif

    // Get Extensions
    std::uint32_t instanceExtensionCount = 0, extensionCountSDL = 0;

    const char * const * pArrayInstanceExtensionsSDL
        = SDL_Vulkan_GetInstanceExtensions (&extensionCountSDL);

    instanceExtensionCount += extensionCountSDL;

#if PATACHE_DEBUG == 1
#if defined(PATACHE_USE_VVL)
    ++instanceExtensionCount; // VK_EXT_layer_settings is no reported o enumerated
#endif
#endif

    // Search extension instance
    vk::ExtensionProperties * pInstanceExtensionProperties = nullptr;
    std::uint32_t             propertyCount                = 0;

    result = vk::enumerateInstanceExtensionProperties (nullptr, &propertyCount, nullptr);

    if (result != vk::Result::eSuccess)
      {
        std::future<void> returnVulkanErr = std::async (
            std::launch::async, Patache::WarningMessage,
            "Cannot get the instance extensions count. continuing without some extensions");

        goto SKIP_OTHER_EXTENSIONS;
      }

    pInstanceExtensionProperties = new vk::ExtensionProperties[propertyCount];

    result = vk::enumerateInstanceExtensionProperties (nullptr, &propertyCount,
                                                       pInstanceExtensionProperties);

    if (result != vk::Result::eSuccess)
      {
        std::future<void> returnVulkanErr
            = std::async (std::launch::async, Patache::WarningMessage,
                          "Cannot get the instance extensions. continuing without some extensions");

        if (pInstanceExtensionProperties != nullptr)
          delete[] pInstanceExtensionProperties;

        goto SKIP_OTHER_EXTENSIONS;
      }

#if PATACHE_DEBUG == 1
#if defined(PATACHE_USE_VVL)
    for (std::uint32_t i = 0; i < propertyCount; ++i)
      {
        if (std::strcmp (pInstanceExtensionProperties[i].extensionName,
                         vk::EXTDebugUtilsExtensionName)
            == 0)
          {
            ++instanceExtensionCount;
            vkEXTDebugUtilsExtensionIsEnabled = true;
          }
      }
#endif
#endif

    delete[] pInstanceExtensionProperties;

  SKIP_OTHER_EXTENSIONS:

    // Enable instance extensions
    const char ** ppAllExtensionInstance = nullptr;

    if (pArrayInstanceExtensionsSDL != nullptr)
      {
        ppAllExtensionInstance = new const char *[instanceExtensionCount](nullptr);

        PATACHE_MEMCPY (ppAllExtensionInstance, pArrayInstanceExtensionsSDL,
                        extensionCountSDL * sizeof (const char *),
                        extensionCountSDL * sizeof (const char *));

#if PATACHE_DEBUG == 1
        // extensionCountSDL is the last element in the index of SDL extensions. here is used as a
        // index for other extensions.
        if (ppAllExtensionInstance[extensionCountSDL] != nullptr)
          {
            ++extensionCountSDL;
          }

        if (vkEXTDebugUtilsExtensionIsEnabled)
          {
            ppAllExtensionInstance[extensionCountSDL] = vk::EXTDebugUtilsExtensionName;
          }

#if defined(PATACHE_USE_VVL)
        ppAllExtensionInstance[(++extensionCountSDL)] = vk::EXTLayerSettingsExtensionName;
#endif
#endif

#if PATACHE_DEBUG == 1
        // Copy to debug info
        pEngine->debugInfo.instanceExtensionsCountVK = instanceExtensionCount;
        pEngine->debugInfo.ppInstanceExtensionsVK
            = new const char *[instanceExtensionCount](nullptr);

        PATACHE_MEMCPY (pEngine->debugInfo.ppInstanceExtensionsVK, ppAllExtensionInstance,
                        instanceExtensionCount * sizeof (const char *),
                        instanceExtensionCount * sizeof (const char *));
#endif

        std::future<void> vulkanList
            = std::async (std::launch::async, Patache::VulkanList, ppAllExtensionInstance,
                          instanceExtensionCount, "Instance Extensions");
      }
    else
      {
        std::future<void> fatalErr = std::async (
            std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
            "Cannot find vulkan surface extensions", std::cref (pEngine->configuration));

        return false;
      }

      // Configuration of Validation Layers
#if PATACHE_DEBUG == 1 && defined(PATACHE_USE_VVL)
    static constexpr vk::Bool32 enableSetting = VK_TRUE;

    constexpr vk::LayerSettingEXT settings[] = { {
                                                     .pLayerName   = "VK_LAYER_KHRONOS_validation",
                                                     .pSettingName = "thread_safety",
                                                     .type       = vk::LayerSettingTypeEXT::eBool32,
                                                     .valueCount = 1,
                                                     .pValues    = &enableSetting,
                                                 },
                                                 {
                                                     .pLayerName   = "VK_LAYER_KHRONOS_validation",
                                                     .pSettingName = "validate_sync",
                                                     .type       = vk::LayerSettingTypeEXT::eBool32,
                                                     .valueCount = 1,
                                                     .pValues    = &enableSetting,
                                                 },
                                                 {
                                                     .pLayerName   = "VK_LAYER_KHRONOS_validation",
                                                     .pSettingName = "validate_best_practices",
                                                     .type       = vk::LayerSettingTypeEXT::eBool32,
                                                     .valueCount = 1,
                                                     .pValues    = &enableSetting,
                                                 } };

#define EXTENT_STRUCT &layerSettingsInfo
    const vk::LayerSettingsCreateInfoEXT layerSettingsInfo{ .settingCount = 3,
                                                            .pSettings    = settings };
#endif

#if !defined(EXTENT_STRUCT)
#define EXTENT_STRUCT nullptr
#endif

    // Create Instance
    const vk::InstanceCreateInfo instanceInfo{
      .pNext            = EXTENT_STRUCT,
      .pApplicationInfo = &patacheEngineInfo,
#if PATACHE_DEBUG == 1 && defined(PATACHE_USE_VVL)
      // Using Layers
      .enabledLayerCount   = PATACHE_LAYER_VK_COUNT,
      .ppEnabledLayerNames = pEngine->debugInfo.ppLayersVK,
#else
      // Not Using Layers
      .enabledLayerCount   = 0,
      .ppEnabledLayerNames = nullptr,
#endif
      .enabledExtensionCount   = instanceExtensionCount,
      .ppEnabledExtensionNames = ppAllExtensionInstance,
    };

    result = vk::createInstance (&instanceInfo, nullptr, &pEngine->vulkan.instance);

#if defined(EXTENT_STRUCT)
#undef EXTENT_STRUCT
#endif

    delete[] ppAllExtensionInstance;
    ppAllExtensionInstance = nullptr;

    if (result == vk::Result::eErrorIncompatibleDriver)
      {
        char errorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

        PATACHE_STRNCPY (errorText,
                         "You do not have Vulkan API compatible drivers or your "
                         "GPU does not support the Vulkan API. ",
                         PATACHE_ERROR_TEXT_SIZE - 1, PATACHE_ERROR_TEXT_SIZE);

        PATACHE_STRNCAT (errorText, vk::to_string (result).c_str (), PATACHE_ERROR_TEXT_SIZE - 1,
                         PATACHE_ERROR_TEXT_SIZE);

        std::future<void> returnVulkanErr = std::async (
            std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
            errorText, std::cref (pEngine->configuration));

        return false;
      }

    if (result != vk::Result::eSuccess)
      {
        std::future<void> returnVulkanCheck
            = std::async (std::launch::async, Patache::VulkanCheck, "Instance", result);

        return false;
      }
  }

#if PATACHE_DEBUG == 1
  // Debug Utils Messenger Info
  if (vkEXTDebugUtilsExtensionIsEnabled)
    {
      // Get function address
      pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT> (
          pEngine->vulkan.instance.getProcAddr ("vkCreateDebugUtilsMessengerEXT"));

      if (pfnVkCreateDebugUtilsMessengerEXT == nullptr)
        {
          std::future<void> err = std::async (std::launch::async, Patache::ErrorMessage,
                                              "Cannot get address of function "
                                              "vkCreateDebugUtilsMessengerEXT");

          goto EXIT_CREATE_DEBUG_UTILS_MESSENGER;
        }

      // Debug Messenger Info
      const vk::DebugUtilsMessengerCreateInfoEXT debugMessengerInfo{
        .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                           | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                       | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
                       | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
        .pfnUserCallback = &DebugMessageFunc
      };

      result = pEngine->vulkan.instance.createDebugUtilsMessengerEXT (
          &debugMessengerInfo, nullptr, &pEngine->vulkan.debugMessenger);

      if (result != vk::Result::eSuccess)
        {
          std::future<void> returnVulkanCheck = std::async (
              std::launch::async, Patache::VulkanCheck, "Debug Utils Messenger", result);

          return false;
        }
    }
EXIT_CREATE_DEBUG_UTILS_MESSENGER:
#endif

  /*
  The GPU with the highest score wins to be chosen later as the logic device
  and also the physical device is used to obtain information about its
  characteristics.

  Discreet video cards are preferred.
  Geometry shading support is obligatory.
  The larger the maximum image size the GPU can handle, the better.
  */

  // Search a Physical Device
  {
    std::uint32_t gpuCount = 0;

    result = pEngine->vulkan.instance.enumeratePhysicalDevices (&gpuCount, nullptr);

    if (result != vk::Result::eSuccess)
      {
        std::future<void> returnVulkanCheck
            = std::async (std::launch::async, Patache::VulkanCheck,
                          "Enumerate Physical Devices - Obtaining the count", result);

        return false;
      }

    if (gpuCount <= 0)
      {
        std::future<void> err = std::async (
            std::launch::async, Patache::FatalErrorMessage, "Patache engine - Raccoon Renderer",
            "No vulkan compatible device found", std::cref (pEngine->configuration));

        return false;
      }

    vk::PhysicalDevice * pTmpPhysicalDevices = new vk::PhysicalDevice[gpuCount](VK_NULL_HANDLE);

    result = pEngine->vulkan.instance.enumeratePhysicalDevices (&gpuCount, pTmpPhysicalDevices);

    if (gpuCount == 1)
      {
        // If you only have one compatible GPU, I will take it directly.
        fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD, PATACHE_TERM_COLOR_PATACHE,
                              "Raccoon Renderer", PATACHE_TERM_RESET, PATACHE_TERM_BOLD,
                              " : Only one vulkan compatible device found", PATACHE_TERM_RESET);

        if (result != vk::Result::eSuccess)
          {
            std::future<void> returnVulkanCheck = std::async (
                std::launch::async, Patache::VulkanCheck, "Enumerate Physical Devices", result);

            std::future<void> err = std::async (
                std::launch::async, Patache::FatalErrorMessage, "Patache Engine - RaccoonRenderer",
                "Failed to obtain a physical device", std::cref (pEngine->configuration));

            delete[] pTmpPhysicalDevices;
            pTmpPhysicalDevices = nullptr;

            return false;
          }

        pEngine->vulkan.physicalDevice = pTmpPhysicalDevices[0];
        delete[] pTmpPhysicalDevices;
        pTmpPhysicalDevices = nullptr;

        const vk::PhysicalDeviceFeatures deviceFeatures
            = pEngine->vulkan.physicalDevice.getFeatures ();

        // I need the GPU to have geometryShader
        if (!deviceFeatures.geometryShader)
          {
            std::future<void> err = std::async (std::launch::async, Patache::ErrorMessage,
                                                "Your GPU Dont Support Geometry Shader");

            pEngine->vulkan.physicalDevice = VK_NULL_HANDLE;

            return false;
          }

        fast_io::io::println (PATACHE_FASTIO_BUFFOUT);

        goto EXIT_CREATE_DEVICE;
      }

    // Search one device from multiple devices
    fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD, PATACHE_TERM_COLOR_PATACHE,
                          "Raccoon Renderer", PATACHE_TERM_RESET, PATACHE_TERM_BOLD, " : Found ",
                          gpuCount, " vulkan compatible devices", PATACHE_TERM_RESET);

    std::uint32_t * pGpuScore = new std::uint32_t[gpuCount](0);

    vk::PhysicalDeviceProperties2 deviceProperties;
    vk::PhysicalDeviceFeatures2   deviceFeatures;

    for (std::uint32_t i = 0; i < gpuCount; ++i)
      {
        deviceProperties = pTmpPhysicalDevices[i].getProperties2 ();
        deviceFeatures   = pTmpPhysicalDevices[i].getFeatures2 ();

        switch (deviceProperties.properties.deviceType)
          {
          case vk::PhysicalDeviceType::eDiscreteGpu:
            pGpuScore[i] += 1000;
            break;
          case vk::PhysicalDeviceType::eIntegratedGpu:
            pGpuScore[i] += 50;
            break;

          case vk::PhysicalDeviceType::eCpu:
          case vk::PhysicalDeviceType::eOther:
          case vk::PhysicalDeviceType::eVirtualGpu:
            break;
          }

        // Maximum possible texture size.
        pGpuScore[i] += deviceProperties.properties.limits.maxImageDimension2D;

        // I need the GPU to have geometryShader
        if (!deviceFeatures.features.geometryShader)
          {
            pGpuScore[i] = 0;

            std::future<void> err = std::async (std::launch::async, Patache::ErrorMessage,
                                                "This GPU Dont Support Geometry "
                                                "Shader, your points will be 0");
          }

        fast_io::io::println (
            PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_DIM, PATACHE_TERM_COLOR_GRAY0, "  [",
            vk::to_string (deviceProperties.properties.deviceType), "] ", PATACHE_TERM_RESET,
            fast_io::mnp::os_c_str (deviceProperties.properties.deviceName), " ", pGpuScore[i]);
      }

    fast_io::io::println (PATACHE_FASTIO_BUFFOUT);

    std::uint32_t highestScore = 0, gpuIndexWinner = 0;

    for (std::uint32_t i = 0; i < gpuCount; ++i)
      if (pGpuScore[i] > highestScore)
        {
          highestScore   = pGpuScore[i];
          gpuIndexWinner = i;
        }

    // Select device
    pEngine->vulkan.physicalDevice = pTmpPhysicalDevices[gpuIndexWinner];

    delete[] pTmpPhysicalDevices;
    pTmpPhysicalDevices = nullptr;
    delete[] pGpuScore;
    pGpuScore = nullptr;
  }
EXIT_CREATE_DEVICE:

  VmaAllocatorCreateFlags vmaAllocatorInfoFlags = 0;

  /*
  Search a queue of graphics from the selected physical device.
  Create a logical device from the selected physical device.
  */
  {
    float queuePriority = 1.0f;

    // Graphics Queue
    {
      std::uint32_t queueCount = 0;

      pEngine->vulkan.physicalDevice.getQueueFamilyProperties (&queueCount, nullptr);

      fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD, PATACHE_TERM_COLOR_PATACHE,
                            "Raccoon Renderer", PATACHE_TERM_RESET, PATACHE_TERM_BOLD, " : Found ",
                            queueCount, " queues", PATACHE_TERM_RESET);

      vk::QueueFamilyProperties * pQueueFamilyProperties
          = new vk::QueueFamilyProperties[queueCount];

      pEngine->vulkan.physicalDevice.getQueueFamilyProperties (&queueCount, pQueueFamilyProperties);

      for (std::uint32_t i = 0; i < queueCount; ++i)
        {
#if PATACHE_DEBUG == 1
          PATACHE_VARTYPE_STRING_PTR pVarTypeRealName = nullptr;

          PATACHE_GET_VARTYPE_STRING (pVarTypeRealName, pQueueFamilyProperties[i].queueFlags);
#endif

          fast_io::io::println (PATACHE_FASTIO_BUFFOUT,
#if PATACHE_DEBUG == 1
                                PATACHE_FASTIO_SHOW_VARTYPE_STRING (pVarTypeRealName, 3, 2, 3),
#endif
                                PATACHE_TERM_BOLD, "Index [", PATACHE_TERM_RESET, i,
                                PATACHE_TERM_BOLD, "] : ", PATACHE_TERM_RESET,
                                vk::to_string (pQueueFamilyProperties[i].queueFlags));

          PATACHE_FREE_VARTYPE_STRING (pVarTypeRealName);
        }

      std::uint32_t i = 0;
      for (i = 0; i < queueCount; ++i)
        if (pQueueFamilyProperties[i].queueFlags
            & (vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer))
          {
            pEngine->vulkan.graphicsQueueFamilyIndex = i;

            fast_io::io::println (
                PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD, PATACHE_TERM_COLOR_PATACHE,
                "Raccoon Renderer", PATACHE_TERM_RESET, PATACHE_TERM_BOLD, " : Found index ",
                pEngine->vulkan.graphicsQueueFamilyIndex,
                " that contains a (graphics and transfer) queue", PATACHE_TERM_RESET);

            break;
          }

      if (i == UINT32_MAX)
        {
          std::future<void> returnVulkanErr = std::async (
              std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
              "No Queue found for graphics", std::cref (pEngine->configuration));

          return false;
        }

#if PATACHE_DEBUG == 1
      // Copy to debug info
      pEngine->debugInfo.queuePriorityVK = queuePriority;
      pEngine->debugInfo.queueFlagsVK
          = pQueueFamilyProperties[pEngine->vulkan.graphicsQueueFamilyIndex].queueFlags;
#endif

      delete[] pQueueFamilyProperties;
      pQueueFamilyProperties = nullptr;
    }

    // Logical Device
    {
      // Search extensions
      vk::ExtensionProperties * pExtensionsProperties = nullptr;
      std::uint32_t             propertyCount         = 0;

      // Count
      result = pEngine->vulkan.physicalDevice.enumerateDeviceExtensionProperties (
          nullptr, &propertyCount, nullptr);

      if (result != vk::Result::eSuccess)
        {
          std::future<void> returnVulkanErr = std::async (
              std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
              "No found device extension count", std::cref (pEngine->configuration));

          return false;
        }

      pExtensionsProperties = new vk::ExtensionProperties[propertyCount];

      // Device extensions
      result = pEngine->vulkan.physicalDevice.enumerateDeviceExtensionProperties (
          nullptr, &propertyCount, pExtensionsProperties);

      if (result != vk::Result::eSuccess)
        {
          std::future<void> returnVulkanErr = std::async (
              std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
              "No found device extensions", std::cref (pEngine->configuration));

          return false;
        }

        // Find extensions
#if defined(_WIN64) && defined(VK_USE_PLATFORM_WIN32_KHR)
#define DEVICE_EXTENSION_COUNT_SEARCH 7 // ++VK_KHR_external_memory_win32
#else
#define DEVICE_EXTENSION_COUNT_SEARCH 6
#endif

      std::uint8_t deviceExtensionCount = 1;
      bool         deviceExtensionIndices[DEVICE_EXTENSION_COUNT_SEARCH]{ false };

#if defined(DEVICE_EXTENSION_COUNT_SEARCH)
#undef DEVICE_EXTENSION_COUNT_SEARCH
#endif

      for (std::uint32_t i = 0; i < propertyCount; ++i)
        {
          if (std::strcmp (pExtensionsProperties[i].extensionName, vk::KHRMaintenance4ExtensionName)
              == 0)
            {
              deviceExtensionIndices[0] = true;
              ++deviceExtensionCount;
            }

          if (std::strcmp (pExtensionsProperties[i].extensionName,
                           vk::KHRDynamicRenderingExtensionName)
              == 0)
            {
              deviceExtensionIndices[1] = true;
              ++deviceExtensionCount;
            }

          if (std::strcmp (pExtensionsProperties[i].extensionName, vk::KHRMaintenance5ExtensionName)
              == 0)
            {
              deviceExtensionIndices[2] = true;
              ++deviceExtensionCount;
            }

          if (std::strcmp (pExtensionsProperties[i].extensionName, vk::EXTMemoryBudgetExtensionName)
              == 0)
            {
              deviceExtensionIndices[3] = true;
              ++deviceExtensionCount;
            }

          if (std::strcmp (pExtensionsProperties[i].extensionName,
                           vk::EXTMemoryPriorityExtensionName)
              == 0)
            {
              deviceExtensionIndices[4] = true;
              ++deviceExtensionCount;
            }

          if (std::strcmp (pExtensionsProperties[i].extensionName,
                           vk::AMDDeviceCoherentMemoryExtensionName)
              == 0)
            {
              deviceExtensionIndices[5] = true;
              ++deviceExtensionCount;
            }

#if defined(_WIN64) && defined(VK_USE_PLATFORM_WIN32_KHR)
          if (std::strcmp (pExtensionsProperties[i].extensionName,
                           vk::KHRExternalMemoryWin32ExtensionName)
              == 0)
            {
              deviceExtensionIndices[6] = true;
              ++deviceExtensionCount;
            }
#endif
        }

      // Add device extensions
      std::uint8_t deviceAddExtensionIndexCount = 0;

      const char ** ppDeviceExtensions = new const char *[deviceExtensionCount](nullptr);

      ppDeviceExtensions[deviceAddExtensionIndexCount] = vk::KHRSwapchainExtensionName;

      if (deviceExtensionIndices[0])
        {
          ppDeviceExtensions[(++deviceAddExtensionIndexCount)] = vk::KHRMaintenance4ExtensionName;
          vmaAllocatorInfoFlags |= VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE4_BIT;
        }

      if (deviceExtensionIndices[1])
        {
          ppDeviceExtensions[(++deviceAddExtensionIndexCount)]
              = vk::KHRDynamicRenderingExtensionName;

          if (deviceExtensionIndices[2])
            {
              ppDeviceExtensions[(++deviceAddExtensionIndexCount)]
                  = vk::KHRMaintenance5ExtensionName;
              vmaAllocatorInfoFlags |= VMA_ALLOCATOR_CREATE_KHR_MAINTENANCE5_BIT;
            }
        }

      if (deviceExtensionIndices[3])
        {
          ppDeviceExtensions[(++deviceAddExtensionIndexCount)] = vk::EXTMemoryBudgetExtensionName;
          vmaAllocatorInfoFlags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
        }

      if (deviceExtensionIndices[4])
        {
          ppDeviceExtensions[(++deviceAddExtensionIndexCount)] = vk::EXTMemoryPriorityExtensionName;
          vmaAllocatorInfoFlags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT;
        }

      if (deviceExtensionIndices[5])
        {
          ppDeviceExtensions[(++deviceAddExtensionIndexCount)]
              = vk::AMDDeviceCoherentMemoryExtensionName;

          vmaAllocatorInfoFlags |= VMA_ALLOCATOR_CREATE_AMD_DEVICE_COHERENT_MEMORY_BIT;
        }

#if defined(_WIN64) && defined(VK_USE_PLATFORM_WIN32_KHR)
      if (deviceExtensionIndices[6])
        {
          ppDeviceExtensions[(++deviceAddExtensionIndexCount)]
              = vk::KHRExternalMemoryWin32ExtensionName;

          vmaAllocatorInfoFlags |= VMA_ALLOCATOR_CREATE_KHR_EXTERNAL_MEMORY_WIN32_BIT;
        }
#endif

#if PATACHE_DEBUG == 1
      // Copy to debug info
      pEngine->debugInfo.ppDeviceExtensionsVK = new const char *[deviceExtensionCount];

      PATACHE_MEMCPY (pEngine->debugInfo.ppDeviceExtensionsVK, ppDeviceExtensions,
                      sizeof (ppDeviceExtensions) * deviceExtensionCount,
                      sizeof (ppDeviceExtensions) * deviceExtensionCount);

      pEngine->debugInfo.deviceExtensionsCountVK = deviceExtensionCount;
#endif

      {
        fast_io::io::println (PATACHE_FASTIO_BUFFOUT);

        std::future<void> vulkanList
            = std::async (std::launch::async, Patache::VulkanList, ppDeviceExtensions,
                          deviceExtensionCount, "Device Extensions");
      }

      // Logical device creation
      const vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
        .queueFamilyIndex = pEngine->vulkan.graphicsQueueFamilyIndex,
        .queueCount       = 1,
        .pQueuePriorities = &queuePriority,
      };

      const vk::DeviceCreateInfo deviceInfo{
        .queueCreateInfoCount    = 1,
        .pQueueCreateInfos       = &deviceQueueCreateInfo,
        .enabledExtensionCount   = deviceExtensionCount,
        .ppEnabledExtensionNames = ppDeviceExtensions,
      };

      result = pEngine->vulkan.physicalDevice.createDevice (&deviceInfo, nullptr,
                                                            &pEngine->vulkan.device);

      if (result != vk::Result::eSuccess)
        {
          std::future<void> returnVulkanCheck
              = std::async (std::launch::async, Patache::VulkanCheck, "Logical Device", result);

          std::future<void> returnVulkanErr = std::async (
              std::launch::async, Patache::FatalErrorMessage, "Patache Engine - Raccoon Renderer",
              "Logical device creation failed", std::cref (pEngine->configuration));

          return false;
        }

      // Graphic queue associated with the logic device
      pEngine->vulkan.queue
          = pEngine->vulkan.device.getQueue (pEngine->vulkan.graphicsQueueFamilyIndex, 0);

      delete[] ppDeviceExtensions;
      ppDeviceExtensions = nullptr;

      delete[] pExtensionsProperties;
      pExtensionsProperties = nullptr;
    }
  }

  // VMA Allocator
  std::future<bool> createAllocator_Async
      = std::async (std::launch::async, [&vmaAllocatorInfoFlags, &pEngine] (void) -> bool {
          vk::Result result;

          VmaVulkanFunctions vulkanFunctions{};
          vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
          vulkanFunctions.vkGetDeviceProcAddr   = &vkGetDeviceProcAddr;

          VmaAllocatorCreateInfo vmaAllocatorInfo{ .flags          = vmaAllocatorInfoFlags,
                                                   .physicalDevice = pEngine->vulkan.physicalDevice,
                                                   .device         = pEngine->vulkan.device,
                                                   .preferredLargeHeapBlockSize = 0,
                                                   .pAllocationCallbacks        = nullptr,
                                                   .pDeviceMemoryCallbacks      = nullptr,
                                                   .pHeapSizeLimit              = nullptr,
                                                   .pVulkanFunctions            = &vulkanFunctions,
                                                   .instance         = pEngine->vulkan.instance,
                                                   .vulkanApiVersion = VK_API_VERSION_1_3,
#if VMA_EXTERNAL_MEMORY
                                                   .pTypeExternalMemoryHandleTypes = nullptr
#endif
          };

          result = static_cast<vk::Result> (
              vmaCreateAllocator (&vmaAllocatorInfo, &pEngine->vulkan.allocator));

          if (result != vk::Result::eSuccess)
            {
              std::future<void> returnVulkanCheck
                  = std::async (std::launch::async, Patache::VulkanCheck, "allocator", result);

              std::future<void> returnVulkanErr
                  = std::async (std::launch::async, Patache::FatalErrorMessage,
                                "Patache Engine - Raccoon Renderer", "Allocator creation fail",
                                std::cref (pEngine->configuration));

              return false;
            }

          return true;
        });

#if PATACHE_DEBUG == 1
  std::future<bool> createImguiDescriptorPool_Async
      = std::async (std::launch::async, CreateImguiDescriptorPool, std::ref (pEngine->vulkan));
  std::future<bool> createImguiPipelineCache_Async
      = std::async (std::launch::async, CreateImguiPipelineCache, std::ref (pEngine->vulkan));

  initImguiCore_Async.wait ();
#endif

  // Create a surface for the window to draw on
  if (!SDL_Vulkan_CreateSurface (pEngine->pGameWindow, pEngine->vulkan.instance, nullptr,
                                 reinterpret_cast<VkSurfaceKHR *> (&pEngine->vulkan.surface)))
    {
      fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_RESET, PATACHE_TERM_BOLD,
                            "SDL Create Window Surface : ", PATACHE_TERM_RESET,
                            PATACHE_TERM_COLOR_YELLOW, "Fail", PATACHE_TERM_RESET);

      std::future<void> fatalErr
          = std::async (std::launch::async, Patache::FatalErrorMessage, "SDL", SDL_GetError (),
                        std::cref (pEngine->configuration));

      return false;
    }

  Patache::SwapchainInfo swapchainInfo{};
  if (!CreateSwapchain (pEngine, swapchainInfo))
    return false;

#if PATACHE_DEBUG == 1
  // saving the data for display in imgui
  pEngine->debugInfo.swapchainPresentModeVK      = swapchainInfo.presentMode;
  pEngine->debugInfo.swapchainImageColorFormatVK = swapchainInfo.imageColorFormat;
  pEngine->debugInfo.swapchainImageColorSpaceVK  = swapchainInfo.imageColorSpace;
  {
    vk::SurfaceCapabilitiesKHR surfaceCapabilities;

    result = pEngine->vulkan.physicalDevice.getSurfaceCapabilitiesKHR (pEngine->vulkan.surface,
                                                                       &surfaceCapabilities);

    pEngine->debugInfo.minImageCountVK = surfaceCapabilities.minImageCount;
  }
#endif

  if (!CreateImageView (pEngine->vulkan, swapchainInfo))
    return false;

  /*
  Depthbuffer will not be useful for now.
  It will be useful for advanced 2D effects and even more so for 3D
  performance.
  */

  // Render Pass
  {
    const vk::AttachmentDescription colorAttachmentDescriptionInfo{
      .format         = swapchainInfo.imageColorFormat,
      .samples        = vk::SampleCountFlagBits::e1,
      .loadOp         = vk::AttachmentLoadOp::eClear,
      .storeOp        = vk::AttachmentStoreOp::eStore,
      .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
      .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
      .initialLayout  = vk::ImageLayout::eUndefined,
      .finalLayout    = vk::ImageLayout::ePresentSrcKHR
    };

    constexpr vk::AttachmentReference colorAttachmentRef{
      .layout = vk::ImageLayout::eColorAttachmentOptimal
    };

    const vk::SubpassDescription subpassDescriptionInfo{ .pipelineBindPoint
                                                         = vk::PipelineBindPoint::eGraphics,
                                                         .colorAttachmentCount = 1,
                                                         .pColorAttachments = &colorAttachmentRef };

    const vk::RenderPassCreateInfo renderPassInfo{ .attachmentCount = 1,
                                                   .pAttachments = &colorAttachmentDescriptionInfo,
                                                   .subpassCount = 1,
                                                   .pSubpasses   = &subpassDescriptionInfo,
                                                   .dependencyCount = 0 };

    result = pEngine->vulkan.device.createRenderPass (&renderPassInfo, nullptr,
                                                      &pEngine->vulkan.renderPass);

    if (result != vk::Result::eSuccess)
      {
        std::future<void> returnVulkanCheck
            = std::async (std::launch::async, Patache::VulkanCheck, "Render Pass", result);

        return false;
      }
  }

  // Command Pool
  std::future<bool> createCommandPool_Async
      = std::async (std::launch::async, [&pEngine] (void) -> bool {
          const vk::CommandPoolCreateInfo commandPoolInfo{
            .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            .queueFamilyIndex = pEngine->vulkan.graphicsQueueFamilyIndex,
          };

          vk::Result result = pEngine->vulkan.device.createCommandPool (
              &commandPoolInfo, nullptr, &pEngine->vulkan.commandPool);

          if (result != vk::Result::eSuccess)
            {
              std::future<void> returnVulkanCheck
                  = std::async (std::launch::async, Patache::VulkanCheck, "Command Pool", result);

              return false;
            }

          return true;
        });

  std::future<bool> createFrameBuffer_Async
      = std::async (std::launch::async, CreateFrameBuffer, std::ref (pEngine->vulkan));

  createCommandPool_Async.wait ();
  if (!createCommandPool_Async.get ())
    return false;

  std::future<bool> createCommandBuffer_Async
      = std::async (std::launch::async, CreateCommandBuffer, std::ref (pEngine->vulkan));

  std::future<bool> createFence_Async
      = std::async (std::launch::async, CreateFence, std::ref (pEngine->vulkan));

  std::future<bool> createSemaphores_Async
      = std::async (std::launch::async, CreateSemaphores, std::ref (pEngine->vulkan));

  std::future<bool> createPipeline_Async = std::async (
      std::launch::async, [&pEngine] (void) -> bool {
        // Load Shaders
        char shaderDirectoryStr[2056]{ 0 };

        PATACHE_STRNCPY (shaderDirectoryStr, SDL_GetBasePath (), 2055, 2056);
        PATACHE_STRNCAT (shaderDirectoryStr, "Shaders/", 2055, 2056);

        fast_io::dir_file shaderDirectory (shaderDirectoryStr);

        fast_io::native_file_loader vertexShader (at (shaderDirectory), "Triangle.vert.spv");

        fast_io::native_file_loader fragmentShader (at (shaderDirectory), "Triangle.frag.spv");

        if (vertexShader.data () == nullptr)
          return false;

        if (fragmentShader.data () == nullptr)
          return false;

        vk::ShaderModule vertexShaderModule{ VK_NULL_HANDLE },
            fragmentShaderModule{ VK_NULL_HANDLE };

        // Create Shader Modules
        const vk::ShaderModuleCreateInfo shaderModuleInfo[2]{
          { .codeSize = vertexShader.size (),
            .pCode    = reinterpret_cast<const std::uint32_t *> (vertexShader.data ()) },
          { .codeSize = fragmentShader.size (),
            .pCode    = reinterpret_cast<const std::uint32_t *> (fragmentShader.data ()) }
        };

        // Vertex Shader Module
        vk::Result result = pEngine->vulkan.device.createShaderModule (
            &shaderModuleInfo[0], nullptr, &vertexShaderModule);

        if (result != vk::Result::eSuccess)
          {
            std::future<void> returnVulkanCheck = std::async (
                std::launch::async, Patache::VulkanCheck, "Vertex Shader Module", result);

            return false;
          }

        // Fragment Shader Module
        result = pEngine->vulkan.device.createShaderModule (&shaderModuleInfo[1], nullptr,
                                                            &fragmentShaderModule);

        if (result != vk::Result::eSuccess)
          {
            std::future<void> returnVulkanCheck = std::async (
                std::launch::async, Patache::VulkanCheck, "Fragment Shader Module", result);

            return false;
          }

        // Create Pipeline
        const vk::PipelineShaderStageCreateInfo shaderStageInfo[2]{
          { .stage  = vk::ShaderStageFlagBits::eVertex,
            .module = vertexShaderModule,
            .pName  = "main" },
          { .stage  = vk::ShaderStageFlagBits::eFragment,
            .module = fragmentShaderModule,
            .pName  = "main" }
        };

        // Vertex Input & Input Assembly
        const vk::VertexInputBindingDescription bindingDescription{
          .binding   = 0,
          .stride    = sizeof (Patache::Vertex2D),
          .inputRate = vk::VertexInputRate::eVertex
        };

        const vk::VertexInputAttributeDescription vertexInputAttrib[2]{
          // Position
          { .location = 0,
            .binding  = 0,
            .format   = vk::Format::eR32G32Sfloat,
            .offset   = offsetof (Patache::Vertex2D, pos) },
          // Color
          { .location = 1,
            .binding  = 0,
            .format   = vk::Format::eR32G32B32Sfloat,
            .offset   = offsetof (Patache::Vertex2D, color) }
        };

        const vk::PipelineVertexInputStateCreateInfo vertexInputStateInfo{
          .vertexBindingDescriptionCount   = 1,
          .pVertexBindingDescriptions      = &bindingDescription,
          .vertexAttributeDescriptionCount = 2,
          .pVertexAttributeDescriptions    = vertexInputAttrib
        };

        constexpr vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo{
          .topology = vk::PrimitiveTopology::eTriangleList, .primitiveRestartEnable = VK_FALSE
        };

        // Dynamic State
        static constexpr vk::DynamicState dynamicStates[2]{ vk::DynamicState::eViewport,
                                                            vk::DynamicState::eScissor };

        const vk::PipelineDynamicStateCreateInfo dynamicStateInfo{ .dynamicStateCount = 2,
                                                                   .pDynamicStates
                                                                   = dynamicStates };

        // Viewport and Scissor
        pEngine->vulkan.viewport.x     = 0;
        pEngine->vulkan.viewport.y     = 0;
        pEngine->vulkan.viewport.width = static_cast<float> (pEngine->vulkan.swapchainExtent.width);
        pEngine->vulkan.viewport.height
            = static_cast<float> (pEngine->vulkan.swapchainExtent.height);
        pEngine->vulkan.viewport.minDepth = 0.0f;
        pEngine->vulkan.viewport.maxDepth = 1.0f;

        pEngine->vulkan.scissor.offset = vk::Offset2D (0, 0);
        pEngine->vulkan.scissor.extent = pEngine->vulkan.swapchainExtent;

        const vk::PipelineViewportStateCreateInfo viewportStateInfo{ .viewportCount = 1,
                                                                     .pViewports
                                                                     = &pEngine->vulkan.viewport,
                                                                     .scissorCount = 1,
                                                                     .pScissors
                                                                     = &pEngine->vulkan.scissor };

        // Rasterizer
        constexpr const vk::PipelineRasterizationStateCreateInfo rasterizerStateInfo{
          .depthClampEnable        = VK_FALSE,
          .rasterizerDiscardEnable = VK_FALSE,
          .polygonMode             = vk::PolygonMode::eFill,
          .cullMode                = vk::CullModeFlagBits::eBack,
          .frontFace               = vk::FrontFace::eClockwise,
          .depthBiasEnable         = VK_FALSE,
          .depthBiasConstantFactor = 0.0f,
          .depthBiasClamp          = 0.0f,
          .depthBiasSlopeFactor    = 0.0f,
          .lineWidth               = 1.0f
        };

        // MSAA Multisampling
        const vk::PipelineMultisampleStateCreateInfo multisamplingStateInfo{
          .rasterizationSamples  = vk::SampleCountFlagBits::e1,
          .sampleShadingEnable   = VK_FALSE,
          .minSampleShading      = 1.0f,
          .pSampleMask           = nullptr,
          .alphaToCoverageEnable = VK_FALSE,
          .alphaToOneEnable      = VK_FALSE
        };

        // Depth and stencil testing
        // Not Yet

        // Color Blending
        constexpr const vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{
          .blendEnable         = VK_FALSE,
          .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
          .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
          .colorBlendOp        = vk::BlendOp::eAdd,
          .srcAlphaBlendFactor = vk::BlendFactor::eOne,
          .dstAlphaBlendFactor = vk::BlendFactor::eZero,
          .alphaBlendOp        = vk::BlendOp::eAdd,
          .colorWriteMask      = vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eG
                            | vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eA
        };

        const vk::PipelineColorBlendStateCreateInfo colorBlendStateInfo{
          .logicOpEnable   = VK_FALSE,
          .logicOp         = vk::LogicOp::eCopy,
          .attachmentCount = 1,
          .pAttachments    = &colorBlendAttachmentState
        };

        const vk::PipelineLayoutCreateInfo pipelineLayoutInfo{ .setLayoutCount         = 0,
                                                               .pSetLayouts            = nullptr,
                                                               .pushConstantRangeCount = 0,
                                                               .pPushConstantRanges    = nullptr };

        result = pEngine->vulkan.device.createPipelineLayout (&pipelineLayoutInfo, nullptr,
                                                              &pEngine->vulkan.pipelineLayout);
        if (result != vk::Result::eSuccess)
          {
            std::future<void> returnVulkanCheck
                = std::async (std::launch::async, Patache::VulkanCheck, "Pipeline Layout", result);

            return false;
          }

        const vk::GraphicsPipelineCreateInfo graphicsPipelineInfo{
          .stageCount          = 2,
          .pStages             = shaderStageInfo,
          .pVertexInputState   = &vertexInputStateInfo,
          .pInputAssemblyState = &inputAssemblyStateInfo,
          .pViewportState      = &viewportStateInfo,
          .pRasterizationState = &rasterizerStateInfo,
          .pMultisampleState   = &multisamplingStateInfo,
          .pDepthStencilState  = nullptr,
          .pColorBlendState    = &colorBlendStateInfo,
          .pDynamicState       = &dynamicStateInfo,
          .layout              = pEngine->vulkan.pipelineLayout,
          .renderPass          = pEngine->vulkan.renderPass
        };

        result = pEngine->vulkan.device.createGraphicsPipelines (
            VK_NULL_HANDLE, 1, &graphicsPipelineInfo, nullptr, &pEngine->vulkan.graphicsPipeline);

        if (result != vk::Result::eSuccess)
          {
            std::future<void> returnVulkanCheck = std::async (
                std::launch::async, Patache::VulkanCheck, "Graphics Pipeline", result);

            return false;
          }

        pEngine->vulkan.device.destroyShaderModule (vertexShaderModule);
        pEngine->vulkan.device.destroyShaderModule (fragmentShaderModule);

        return true;
      });

  std::future<void> vulkanInfo_Async
      = std::async (std::launch::async, VulkanInfo, std::ref (pEngine), std::ref (swapchainInfo));

#if PATACHE_DEBUG == 1
  createImguiPipelineCache_Async.wait ();
  if (!createImguiPipelineCache_Async.get ())
    return false;

  createImguiDescriptorPool_Async.wait ();
  if (!createImguiDescriptorPool_Async.get ())
    return false;

  InitImGuiVulkan (pEngine);
#endif

  /*
  CreateDepthBuffer_Async.wait();
  if (!CreateDepthBuffer_Async.get()) return false;
  */

  createFrameBuffer_Async.wait ();
  if (!createFrameBuffer_Async.get ())
    return false;

  createCommandBuffer_Async.wait ();
  if (!createCommandBuffer_Async.get ())
    return false;

  createAllocator_Async.wait ();
  if (!createAllocator_Async.get ())
    return false;

  std::future<bool> createBuffer_Async
      = std::async (std::launch::async, [] (void) -> bool { return true; });

  createFence_Async.wait ();
  if (!createFence_Async.get ())
    return false;

  createSemaphores_Async.wait ();
  if (!createSemaphores_Async.get ())
    return false;

  createPipeline_Async.wait ();
  if (!createPipeline_Async.get ())
    return false;

  createBuffer_Async.wait ();
  if (!createBuffer_Async.get ())
    return false;

  return true;
}

#if PATACHE_DEBUG == 1
#include <imgui_impl_vulkan.h>
#endif

void
RaccoonRendererClose (Patache::VulkanBackend & rVulkan)
{
  if (rVulkan.device == VK_NULL_HANDLE || rVulkan.instance == VK_NULL_HANDLE)
    return;

  vk::Result result = rVulkan.device.waitIdle ();

  if (result != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck
          = std::async (std::launch::async, Patache::VulkanCheck, "Device Wait Idle", result);
    }

  if (rVulkan.queue == VK_NULL_HANDLE)
    return;

  result = rVulkan.queue.waitIdle ();

  if (result != vk::Result::eSuccess)
    std::future<void> returnVulkanCheck
        = std::async (std::launch::async, Patache::VulkanCheck, "Queue Wait Idle", result);

  // Swapchain
  // Color
  for (std::uint8_t i = 0; i < rVulkan.swapchainImageCount; ++i)
    {
      rVulkan.device.destroyFramebuffer (rVulkan.pSwapchainFrameBuffers[i]);
      rVulkan.device.destroyImageView (rVulkan.pSwapchainColorImageViews[i]);
    }

  delete[] rVulkan.pSwapchainFrameBuffers;
  rVulkan.pSwapchainFrameBuffers = nullptr;
  delete[] rVulkan.pSwapchainColorImageViews;
  rVulkan.pSwapchainColorImageViews = nullptr;
  delete[] rVulkan.pSwapchainImages;
  rVulkan.pSwapchainImages = nullptr;

  rVulkan.device.destroySwapchainKHR (rVulkan.swapchain);

  // Buffer
  /*
    if (rVulkan.renderBuffer != nullptr)
      {
        for (std::uint8_t i = 0; i < rVulkan.swapchainImageCount; ++i)
          rVulkan.device.destroyBuffer (rVulkan.renderBuffer[i], nullptr);

        delete[] rVulkan.renderBuffer;
        rVulkan.renderBuffer = nullptr;
      }

    if (rVulkan.renderBufferMemory != nullptr)
      {
        for (std::uint8_t i = 0; i < rVulkan.swapchainImageCount; ++i)
          rVulkan.device.freeMemory (rVulkan.renderBufferMemory[i], nullptr);

        delete[] rVulkan.renderBufferMemory;
        rVulkan.renderBufferMemory = nullptr;
      }

    if (rVulkan.stagingBuffer != VK_NULL_HANDLE)
      {
        rVulkan.device.destroyBuffer (rVulkan.stagingBuffer, nullptr);
        rVulkan.device.freeMemory (rVulkan.stagingBufferMemory, nullptr);
      }
  */

  // Imgui
#if PATACHE_DEBUG == 1
  ImGuiIO * pIO = nullptr;

  if (ImGui::GetCurrentContext () != nullptr)
    {
      pIO = &ImGui::GetIO ();

      if (pIO->BackendPlatformName != nullptr)
        ImGui_ImplVulkan_Shutdown ();
    }

  rVulkan.device.destroyDescriptorPool (rVulkan.imguiDescriptorPool);
  rVulkan.device.destroyPipelineCache (rVulkan.imguiPipelineCache);
#endif

  // Pipeline
  rVulkan.device.destroyPipelineCache (rVulkan.graphicsPipelineCache);
  rVulkan.device.destroyPipelineLayout (rVulkan.pipelineLayout);
  rVulkan.device.destroyPipeline (rVulkan.graphicsPipeline);

  rVulkan.device.destroyRenderPass (rVulkan.renderPass);

  // Primitivas de sincronizacion
  for (std::uint32_t i = 0; i < rVulkan.swapchainImageCount; ++i)
    {
      rVulkan.device.destroySemaphore (rVulkan.pImageAvailableSemaphores[i]);
      rVulkan.device.destroySemaphore (rVulkan.pImageFinishedSemaphores[i]);
      rVulkan.device.destroyFence (rVulkan.pInFlightFences[i]);
    }

  delete[] rVulkan.pImageAvailableSemaphores;
  rVulkan.pImageAvailableSemaphores = nullptr;
  delete[] rVulkan.pImageFinishedSemaphores;
  rVulkan.pImageFinishedSemaphores = nullptr;
  delete[] rVulkan.pInFlightFences;
  rVulkan.pInFlightFences = nullptr;

  rVulkan.device.destroyCommandPool (rVulkan.commandPool);
  delete[] rVulkan.pCmd;
  rVulkan.pCmd = nullptr;

  rVulkan.device.destroy ();

  rVulkan.instance.destroySurfaceKHR (rVulkan.surface);

#if PATACHE_DEBUG == 1
  if (pfnVkCreateDebugUtilsMessengerEXT != nullptr)
    {
      pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT> (
          rVulkan.instance.getProcAddr ("vkDestroyDebugUtilsMessengerEXT"));

      if (pfnVkDestroyDebugUtilsMessengerEXT == nullptr)
        std::future<void> err = std::async (std::launch::async, Patache::ErrorMessage,
                                            "Cannot get address of function "
                                            "vkDestroyDebugUtilsMessengerEXT");
      else
        rVulkan.instance.destroyDebugUtilsMessengerEXT (rVulkan.debugMessenger);
    }
#endif

  vmaDestroyAllocator (rVulkan.allocator);

  rVulkan.instance.destroy ();
}
