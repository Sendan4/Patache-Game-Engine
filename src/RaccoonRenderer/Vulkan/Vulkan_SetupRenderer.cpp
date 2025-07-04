#include "Vulkan_SetupRenderer.hpp"

bool
RaccoonRendererInit (Patache::Engine *                 Engine,
                     const Patache::EngineCreateInfo & Info)
{
#if PATACHE_DEBUG == 1
  std::future<void> InitImgui_Async = std::async (
      std::launch::async, InitImgui, std::cref (Engine->configuration));
#endif

  vk::Result Result;

  // Vulkan Instance
  {
    const char * const ApplicationName
        = (Info.gameName != nullptr) ? Info.gameName : "Untitled Game";

    const vk::ApplicationInfo PatacheEngineInfo{
      .pApplicationName   = ApplicationName,
      .applicationVersion = Info.gameVersion,
      .pEngineName        = PATACHE_ENGINE_NAME,
      .engineVersion      = PATACHE_ENGINE_VERSION_VK,
      .apiVersion         = VK_API_VERSION_1_3
    };

    // The validation layers are activated with USE_VVL=ON.
    // They are for the development and testing of this backend.
    // Layers
#if PATACHE_DEBUG == 1 && defined(PATACHE_USE_VVL)
    Engine->engineInfo.ppVkLayers = new const char *[PATACHE_VK_LAYER_COUNT];
    Engine->engineInfo.ppVkLayers[0] = "VK_LAYER_KHRONOS_validation";

    // List Layers
    {
      std::future<void> ReturnVulkanList = std::async (
          std::launch::async, Patache::Log::VulkanList,
          Engine->engineInfo.ppVkLayers, PATACHE_VK_LAYER_COUNT, "Layers");
    }
#endif

    // Get Extensions
    std::uint32_t        SDLExtensionCount = 0;
    const char * const * pSDLArrayInstanceExtensions
        = SDL_Vulkan_GetInstanceExtensions (&SDLExtensionCount);

    const char ** ppAllExtensionInstance = nullptr;

#if PATACHE_DEBUG == 1
#if defined(PATACHE_USE_VVL)
    // Adding VVL
    Engine->engineInfo.VkInstanceExtensionsCount = SDLExtensionCount + 2;
#else
    // Adding other extensions count
    Engine->engineInfo.VkInstanceExtensionsCount = SDLExtensionCount + 1;
#endif
    // Macro for the total count of extensions
#define EXTENSION_COUNT Engine->engineInfo.VkInstanceExtensionsCount // Debug
#else                                     // PATACHE_DEBUG
#define EXTENSION_COUNT SDLExtensionCount // Release
#endif                                    // PATACHE_DEBUG

    /*
     * Cuando se compila en el modo de depuracion se añaden 2 extensiones mas
     * Cuando se compila en el modo de lanzamiento no se utilizan extensiones
     * extra.
     * */

    if (pSDLArrayInstanceExtensions != nullptr)
      {
        ppAllExtensionInstance = new const char *[EXTENSION_COUNT];

        PATACHE_MEMCPY (ppAllExtensionInstance, pSDLArrayInstanceExtensions,
                        SDLExtensionCount * sizeof (const char *));

#if PATACHE_DEBUG == 1
        ppAllExtensionInstance[SDLExtensionCount]
            = vk::EXTDebugUtilsExtensionName;
#if defined(PATACHE_USE_VVL)
        ppAllExtensionInstance[SDLExtensionCount + 1]
            = vk::EXTLayerSettingsExtensionName;
#endif

        // Struct Patache EngineInfo
        Engine->engineInfo.ppVkInstanceExtensions
            = new const char *[EXTENSION_COUNT];

        for (std::uint8_t i = 0; i < EXTENSION_COUNT; ++i)
          Engine->engineInfo.ppVkInstanceExtensions[i]
              = ppAllExtensionInstance[i];
#endif

        std::future<void> VulkanList = std::async (
            std::launch::async, Patache::Log::VulkanList,
            ppAllExtensionInstance, EXTENSION_COUNT, "Instance Extensions");
      }
    else
      {
        std::future<void> FatalErr
            = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                          "Patache Engine - Raccoon Renderer",
                          "Cannot find vulkan surface extensions",
                          std::cref (Engine->configuration));

        return false;
      }

      // Configuration of Validation Layers
#if PATACHE_DEBUG == 1 && defined(PATACHE_USE_VVL)
    static constexpr vk::Bool32 EnableSetting = VK_TRUE;

    constexpr vk::LayerSettingEXT Settings[]
        = { {
                .pLayerName   = "VK_LAYER_KHRONOS_validation",
                .pSettingName = "thread_safety",
                .type         = vk::LayerSettingTypeEXT::eBool32,
                .valueCount   = 1,
                .pValues      = &EnableSetting,
            },
            {
                .pLayerName   = "VK_LAYER_KHRONOS_validation",
                .pSettingName = "validate_sync",
                .type         = vk::LayerSettingTypeEXT::eBool32,
                .valueCount   = 1,
                .pValues      = &EnableSetting,
            },
            {
                .pLayerName   = "VK_LAYER_KHRONOS_validation",
                .pSettingName = "validate_best_practices",
                .type         = vk::LayerSettingTypeEXT::eBool32,
                .valueCount   = 1,
                .pValues      = &EnableSetting,
            } };

    const vk::LayerSettingsCreateInfoEXT LayerSettingsInfo{ .settingCount = 3,
                                                            .pSettings
                                                            = Settings };

#define EXTENT_STRUCT &LayerSettingsInfo // Pointer to struct
#else
#define EXTENT_STRUCT nullptr
#endif

    // Create Instance
    const vk::InstanceCreateInfo InstanceInfo{
      .pNext            = EXTENT_STRUCT,
      .pApplicationInfo = &PatacheEngineInfo,
#if PATACHE_DEBUG == 1 && defined(PATACHE_USE_VVL)
      // Using Layers
      .enabledLayerCount   = PATACHE_VK_LAYER_COUNT,
      .ppEnabledLayerNames = Engine->engineInfo.ppVkLayers,
#else
      // Not Using Layers
      .enabledLayerCount   = 0,
      .ppEnabledLayerNames = nullptr,
#endif
      .enabledExtensionCount   = EXTENSION_COUNT,
      .ppEnabledExtensionNames = ppAllExtensionInstance,
    };

    Result = vk::createInstance (&InstanceInfo, nullptr,
                                 &Engine->Vulkan.Instance);

    delete[] ppAllExtensionInstance;
    ppAllExtensionInstance = nullptr;

    if (Result == vk::Result::eErrorIncompatibleDriver)
      {
        char ErrorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

        PATACHE_STRNCPY (
            ErrorText,
            "You do not have Vulkan API compatible drivers or your "
            "GPU does not support the Vulkan API. ",
            PATACHE_ERROR_TEXT_SIZE - 1);

        PATACHE_STRNCAT (ErrorText, vk::to_string (Result).c_str (),
                         PATACHE_ERROR_TEXT_SIZE - 1);

        std::future<void> ReturnVulkanErr
            = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                          "Patache Engine - Raccoon Renderer", ErrorText,
                          std::cref (Engine->configuration));

        return false;
      }

    if (Result != vk::Result::eSuccess)
      {
        std::future<void> ReturnVulkanCheck = std::async (
            std::launch::async, Patache::Log::VulkanCheck, "Instance", Result);

        return false;
      }
  }

#if PATACHE_DEBUG == 1
  // Debug Utils Messenger Info
  {
    // Get function address
    pfnVkCreateDebugUtilsMessengerEXT
        = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT> (
            Engine->Vulkan.Instance.getProcAddr (
                "vkCreateDebugUtilsMessengerEXT"));

    if (pfnVkCreateDebugUtilsMessengerEXT == nullptr)
      {
        std::future<void> Err
            = std::async (std::launch::async, Patache::Log::ErrorMessage,
                          "Cannot get address of function "
                          "vkCreateDebugUtilsMessengerEXT");

        goto EXIT_CREATE_DEBUG_UTILS_MESSENGER;
      }

    // Debug Messenger Info
    const vk::DebugUtilsMessengerCreateInfoEXT DebugMessengerInfo{
      .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                         | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
      .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                     | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
                     | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
      .pfnUserCallback = &debugMessageFunc
    };

    Result = Engine->Vulkan.Instance.createDebugUtilsMessengerEXT (
        &DebugMessengerInfo, nullptr, &Engine->Vulkan.DebugMessenger);

    if (Result != vk::Result::eSuccess)
      {
        std::future<void> ReturnVulkanCheck
            = std::async (std::launch::async, Patache::Log::VulkanCheck,
                          "Debug Utils Messenger", Result);

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
    std::uint32_t GpuCount = 0;

    Result = Engine->Vulkan.Instance.enumeratePhysicalDevices (&GpuCount,
                                                               nullptr);

    if (Result != vk::Result::eSuccess)
      {
        std::future<void> ReturnVulkanCheck = std::async (
            std::launch::async, Patache::Log::VulkanCheck,
            "Enumerate Physical Devices - Obtaining the count", Result);

        return false;
      }

    if (GpuCount <= 0)
      {
        std::future<void> Err
            = std::async (std::launch::async, Patache::Log::FatalErrorMessage,
                          "Patache engine - Raccoon Renderer",
                          "No vulkan compatible device found",
                          std::cref (Engine->configuration));

        return false;
      }

    vk::PhysicalDevice * TmpPhysicalDevices = VK_NULL_HANDLE;

    if (GpuCount == 1)
      {
        // If you only have one compatible GPU, I will take it directly.
        fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_BOLD,
                              PATACHE_TERM_COLOR_PATACHE, "Raccoon Renderer",
                              PATACHE_TERM_RESET, PATACHE_TERM_BOLD,
                              " : Only one vulkan compatible device found",
                              PATACHE_TERM_RESET);

        TmpPhysicalDevices = new vk::PhysicalDevice[GpuCount];

        Result = Engine->Vulkan.Instance.enumeratePhysicalDevices (
            &GpuCount, TmpPhysicalDevices);
        if (Result != vk::Result::eSuccess)
          {
            std::future<void> ReturnVulkanCheck
                = std::async (std::launch::async, Patache::Log::VulkanCheck,
                              "Enumerate Physical Devices", Result);

            std::future<void> Err = std::async (
                std::launch::async, Patache::Log::FatalErrorMessage,
                "Patache Engine - RaccoonRenderer",
                "Failed to obtain a physical device",
                std::cref (Engine->configuration));

            delete[] TmpPhysicalDevices;
            TmpPhysicalDevices = nullptr;

            return false;
          }

        Engine->Vulkan.PhysicalDevice = TmpPhysicalDevices[0];
        delete[] TmpPhysicalDevices;
        TmpPhysicalDevices = nullptr;

        vk::PhysicalDeviceFeatures DeviceFeatures
            = Engine->Vulkan.PhysicalDevice.getFeatures ();

        // I need the GPU to have geometryShader
        if (!DeviceFeatures.geometryShader)
          {
            std::future<void> Err
                = std::async (std::launch::async, Patache::Log::ErrorMessage,
                              "Your GPU Dont Support Geometry Shader");

            Engine->Vulkan.PhysicalDevice = VK_NULL_HANDLE;

            return false;
          }

        fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT);

        goto EXIT_CREATE_DEVICE;
      }
    else
      {
        // Get multiple devices
        TmpPhysicalDevices = new vk::PhysicalDevice[GpuCount];
        Result             = Engine->Vulkan.Instance.enumeratePhysicalDevices (
            &GpuCount, TmpPhysicalDevices);

        if (Result != vk::Result::eSuccess)
          {
            std::future<void> ReturnVulkanCheck
                = std::async (std::launch::async, Patache::Log::VulkanCheck,
                              "Enumerate Physical Devices", Result);

            return false;
          }
      }

    // Search one device from multiple devices
    fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_BOLD,
                          PATACHE_TERM_COLOR_PATACHE, "Raccoon Renderer",
                          PATACHE_TERM_RESET, PATACHE_TERM_BOLD, " : Found ",
                          GpuCount, " vulkan compatible devices",
                          PATACHE_TERM_RESET);

    std::uint32_t * GpuScore = new std::uint32_t[GpuCount - 1];

    vk::PhysicalDeviceProperties2 DeviceProperties;
    vk::PhysicalDeviceFeatures2   DeviceFeatures;

    for (std::uint32_t i = 0; i < GpuCount; ++i)
      {
        DeviceProperties = TmpPhysicalDevices[i].getProperties2 ();
        DeviceFeatures   = TmpPhysicalDevices[i].getFeatures2 ();

        switch (DeviceProperties.properties.deviceType)
          {
          case vk::PhysicalDeviceType::eDiscreteGpu:
            GpuScore[i] += 1000;
            break;
          case vk::PhysicalDeviceType::eIntegratedGpu:
            GpuScore[i] += 50;
            break;

          case vk::PhysicalDeviceType::eCpu:
          case vk::PhysicalDeviceType::eOther:
          case vk::PhysicalDeviceType::eVirtualGpu:
            break;
          }

        // Maximum possible texture size.
        GpuScore[i] += DeviceProperties.properties.limits.maxImageDimension2D;

        // I need the GPU to have geometryShader
        if (!DeviceFeatures.features.geometryShader)
          {
            GpuScore[i] = 0;

            std::future<void> Err
                = std::async (std::launch::async, Patache::Log::ErrorMessage,
                              "This GPU Dont Support Geometry "
                              "Shader, your points will be 0");
          }

        fast_io::io::println (
            PATACHE_FAST_IO_BUFF_OUT,
#if !defined(_WIN64)
            PATACHE_TERM_DIM,
#endif
            PATACHE_TERM_COLOR_GRAY0, "  [",
            vk::to_string (DeviceProperties.properties.deviceType), "] ",
            PATACHE_TERM_RESET,
            fast_io::mnp::os_c_str (DeviceProperties.properties.deviceName),
            " ", GpuScore[i]);
      }

    fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT);

    std::uint32_t HighestScore = 0, GpuIndexWinner = 0;

    for (std::uint32_t i = 0; i < GpuCount; ++i)
      if (GpuScore[i] > HighestScore)
        {
          HighestScore   = GpuScore[i];
          GpuIndexWinner = i;
        }

    // Select device
    Engine->Vulkan.PhysicalDevice = TmpPhysicalDevices[GpuIndexWinner];

    delete[] TmpPhysicalDevices;
    TmpPhysicalDevices = nullptr;
    delete[] GpuScore;
  }
EXIT_CREATE_DEVICE:

  /*
  Search a queue of graphics from the selected physical device.
  Create a logical device from the selected physical device.
  */
  {
    float QueuePriority = 1.0f;

    // Graphics Queue
    {
      std::uint32_t QueueCount = 0;

      Engine->Vulkan.PhysicalDevice.getQueueFamilyProperties (&QueueCount,
                                                              nullptr);

      fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_BOLD,
                            PATACHE_TERM_COLOR_PATACHE, "Raccoon Renderer",
                            PATACHE_TERM_RESET, PATACHE_TERM_BOLD, " : Found ",
                            QueueCount, " queues", PATACHE_TERM_RESET);

      vk::QueueFamilyProperties * QueueFamilyProperties
          = new vk::QueueFamilyProperties[QueueCount];

      Engine->Vulkan.PhysicalDevice.getQueueFamilyProperties (
          &QueueCount, QueueFamilyProperties);

      for (std::uint32_t i = 0; i < QueueCount; ++i)
        fast_io::io::println (
            PATACHE_FAST_IO_BUFF_OUT,
#if PATACHE_DEBUG == 1
#if !defined(_WIN64)
            PATACHE_TERM_DIM,
#endif
            PATACHE_TERM_COLOR_GRAY0,
#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
            "  [",
            fast_io::mnp::os_c_str (abi::__cxa_demangle (
                typeid (QueueFamilyProperties[i].queueFlags).name (), nullptr,
                nullptr, nullptr)),
            "] ",
#else
            "  [",
            fast_io::mnp::os_c_str (
                typeid (QueueFamilyProperties[i].queueFlags).name ()),
            "] ",
#endif
            PATACHE_TERM_RESET,
#else  // PATACHE_DEBUG
            "  ",
#endif // PATACHE_DEBUG
            PATACHE_TERM_BOLD, "Index [", PATACHE_TERM_RESET, i,
            PATACHE_TERM_BOLD, "] : ", PATACHE_TERM_RESET,
            vk::to_string (QueueFamilyProperties[i].queueFlags));

      for (std::uint32_t index = 0; index < QueueCount; ++index)
        if (QueueFamilyProperties[index].queueFlags
            & vk::QueueFlagBits::eGraphics)
          {
            Engine->Vulkan.GraphicsQueueFamilyIndex = index;

            fast_io::io::println (
                PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_BOLD,
                PATACHE_TERM_COLOR_PATACHE, "Raccoon Renderer",
                PATACHE_TERM_RESET, PATACHE_TERM_BOLD, " : Found index ",
                Engine->Vulkan.GraphicsQueueFamilyIndex,
                " that contains a graphics queue", PATACHE_TERM_RESET);

            break;
          }

      if (Engine->Vulkan.GraphicsQueueFamilyIndex == (QueueCount - 1))
        {
          std::future<void> ReturnVulkanErr = std::async (
              std::launch::async, Patache::Log::FatalErrorMessage,
              "Patache Engine - Raccoon Renderer",
              "No Queue found for graphics",
              std::cref (Engine->configuration));

          return false;
        }

#if PATACHE_DEBUG == 1
      Engine->engineInfo.VkQueuePriority = QueuePriority;
      Engine->engineInfo.VkQueueFlags
          = QueueFamilyProperties[Engine->Vulkan.GraphicsQueueFamilyIndex]
                .queueFlags;
#endif

      delete[] QueueFamilyProperties;
      QueueFamilyProperties = nullptr;
    }

    // Logical Device
    {
      const vk::DeviceQueueCreateInfo DeviceQueueCreateInfo{
        .queueFamilyIndex = Engine->Vulkan.GraphicsQueueFamilyIndex,
        .queueCount       = 1,
        .pQueuePriorities = &QueuePriority,
      };

      const char * const DeviceExtensions[1]{ "VK_KHR_swapchain" };

#if PATACHE_DEBUG == 1
      Engine->engineInfo.ppVkDeviceExtensions    = new const char *[1];
      Engine->engineInfo.ppVkDeviceExtensions[0] = DeviceExtensions[0];
      Engine->engineInfo.VkDeviceExtensionsCount = 1;
#endif

      // Logical Device Info
      const vk::DeviceCreateInfo DeviceCreateInfo{
        .queueCreateInfoCount    = 1,
        .pQueueCreateInfos       = &DeviceQueueCreateInfo,
        .enabledExtensionCount   = 1,
        .ppEnabledExtensionNames = DeviceExtensions,
      };

      // List Device Extensions
      {
        fast_io::io::println (PATACHE_FAST_IO_BUFF_OUT);

        std::future<void> VulkanList
            = std::async (std::launch::async, Patache::Log::VulkanList,
                          DeviceExtensions, 1, "Device Extensions");
      }

      Result = Engine->Vulkan.PhysicalDevice.createDevice (
          &DeviceCreateInfo, nullptr, &Engine->Vulkan.Device);

      if (Result != vk::Result::eSuccess)
        {
          std::future<void> ReturnVulkanCheck
              = std::async (std::launch::async, Patache::Log::VulkanCheck,
                            "Logical Device", Result);

          std::future<void> ReturnVulkanErr = std::async (
              std::launch::async, Patache::Log::FatalErrorMessage,
              "Patache Engine - Raccoon Renderer",
              "Logical device creation failed",
              std::cref (Engine->configuration));

          return false;
        }

      // Graphic queue associated with the logic device
      Engine->Vulkan.Queue = Engine->Vulkan.Device.getQueue (
          Engine->Vulkan.GraphicsQueueFamilyIndex, 0);
    }
  }

#if PATACHE_DEBUG == 1
  InitImgui_Async.wait ();

  std::future<bool> CreateImguiDescriptorPool_Async
      = std::async (std::launch::async, std::ref (CreateImguiDescriptorPool),
                    std::ref (Engine->Vulkan));
  std::future<bool> CreateImguiPipelineCache_Async
      = std::async (std::launch::async, std::ref (CreateImguiPipelineCache),
                    std::ref (Engine->Vulkan));
#endif

  // Create a surface for the window to draw on
  if (!SDL_Vulkan_CreateSurface (
          Engine->GameWindow, Engine->Vulkan.Instance, nullptr,
          reinterpret_cast<VkSurfaceKHR *> (&Engine->Vulkan.Surface)))
    {
      fast_io::io::println (
          PATACHE_FAST_IO_BUFF_OUT, PATACHE_TERM_RESET, PATACHE_TERM_BOLD,
          "SDL Create Window Surface : ", PATACHE_TERM_RESET,
          PATACHE_TERM_COLOR_YELLOW, "Fail", PATACHE_TERM_RESET);

      std::future<void> FatalErr = std::async (
          std::launch::async, Patache::Log::FatalErrorMessage, "SDL",
          SDL_GetError (), std::cref (Engine->configuration));

      return false;
    }

  Patache::SwapChainInfo SwapChainInfo;
  if (!CreateSwapChain (Engine, SwapChainInfo))
    return false;

#if PATACHE_DEBUG == 1
  // saving the data for display in imgui
  Engine->engineInfo.VkSwapchainPresentMode = SwapChainInfo.PresentMode;
  Engine->engineInfo.VkSwapchainImageColorFormat
      = SwapChainInfo.ImageColorFormat;
  Engine->engineInfo.VkSwapchainImageColorSpace
      = SwapChainInfo.ImageColorSpace;
  {
    vk::SurfaceCapabilitiesKHR SurfaceCapabilities;

    Result = Engine->Vulkan.PhysicalDevice.getSurfaceCapabilitiesKHR (
        Engine->Vulkan.Surface, &SurfaceCapabilities);

    Engine->engineInfo.VkMinImageCount = SurfaceCapabilities.minImageCount;
  }
#endif

  if (!CreateImageView (Engine->Vulkan, SwapChainInfo))
    return false;

  /*
  Depthbuffer will not be useful for now.
  It will be useful for advanced 2D effects and even more so for 3D
  performance.
  */

  // Render Pass
  {
    const vk::AttachmentDescription ColorAttachmentDescriptionInfo{
      .format         = SwapChainInfo.ImageColorFormat,
      .samples        = vk::SampleCountFlagBits::e1,
      .loadOp         = vk::AttachmentLoadOp::eClear,
      .storeOp        = vk::AttachmentStoreOp::eStore,
      .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
      .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
      .initialLayout  = vk::ImageLayout::eUndefined,
      .finalLayout    = vk::ImageLayout::ePresentSrcKHR
    };

    const vk::AttachmentReference ColorAttachmentRef{
      .layout = vk::ImageLayout::eColorAttachmentOptimal
    };

    const vk::SubpassDescription SubpassDescriptionInfo{
      .pipelineBindPoint    = vk::PipelineBindPoint::eGraphics,
      .colorAttachmentCount = 1,
      .pColorAttachments    = &ColorAttachmentRef
    };

    const vk::RenderPassCreateInfo RenderPassCreateInfo{
      .attachmentCount = 1,
      .pAttachments    = &ColorAttachmentDescriptionInfo,
      .subpassCount    = 1,
      .pSubpasses      = &SubpassDescriptionInfo,
      .dependencyCount = 0
    };

    Result = Engine->Vulkan.Device.createRenderPass (
        &RenderPassCreateInfo, nullptr, &Engine->Vulkan.RenderPass);

    if (Result != vk::Result::eSuccess)
      {
        std::future<void> ReturnVulkanCheck
            = std::async (std::launch::async, Patache::Log::VulkanCheck,
                          "Render Pass", Result);

        return false;
      }
  }

  // Command Pool
  std::future<bool> CreateCommandPool_Async
      = std::async (std::launch::async, [&Engine] (void) -> bool {
          const vk::CommandPoolCreateInfo CommandPoolInfo{
            .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            .queueFamilyIndex = Engine->Vulkan.GraphicsQueueFamilyIndex,
          };

          vk::Result Result = Engine->Vulkan.Device.createCommandPool (
              &CommandPoolInfo, nullptr, &Engine->Vulkan.CommandPool);

          if (Result != vk::Result::eSuccess)
            {
              std::future<void> ReturnVulkanCheck
                  = std::async (std::launch::async, Patache::Log::VulkanCheck,
                                "Command Pool", Result);

              return false;
            }

          return true;
        });

  std::future<bool> CreateFrameBuffer_Async = std::async (
      std::launch::async, CreateFrameBuffer, std::ref (Engine->Vulkan));

  CreateCommandPool_Async.wait ();
  if (!CreateCommandPool_Async.get ())
    return false;

  std::future<bool> CreateCommandBuffer_Async = std::async (
      std::launch::async, CreateCommandBuffer, std::ref (Engine->Vulkan));

  std::future<bool> CreateFence_Async = std::async (
      std::launch::async, CreateFence, std::ref (Engine->Vulkan));

  std::future<bool> CreateSemaphores_Async = std::async (
      std::launch::async, CreateSemaphores, std::ref (Engine->Vulkan));

  std::future<bool> CreatePipeline_Async
      = std::async (std::launch::async, [&Engine] (void) -> bool {
          // Load Shaders
          char ShaderDirectory_str[2056]{ 0 };

          PATACHE_STRNCPY (ShaderDirectory_str, SDL_GetBasePath (), 2055);
          PATACHE_STRNCAT (ShaderDirectory_str, "Shaders/", 2055);

          fast_io::dir_file ShaderDirectory (ShaderDirectory_str);

          fast_io::native_file_loader VertexShader (at (ShaderDirectory),
                                                    "Triangle.vert.spv");

          fast_io::native_file_loader FragmentShader (at (ShaderDirectory),
                                                      "Triangle.frag.spv");

          if (VertexShader.data () == nullptr)
            return false;

          if (FragmentShader.data () == nullptr)
            return false;

          vk::ShaderModule VertexShaderModule{ VK_NULL_HANDLE },
              FragmentShaderModule{ VK_NULL_HANDLE };

          // Create Shader Modules
          const vk::ShaderModuleCreateInfo ShaderModuleInfo[2]{
            { .codeSize = VertexShader.size (),
              .pCode    = reinterpret_cast<const std::uint32_t *> (
                  VertexShader.data ()) },
            { .codeSize = FragmentShader.size (),
              .pCode    = reinterpret_cast<const std::uint32_t *> (
                  FragmentShader.data ()) }
          };

          // Vertex Shader Module
          vk::Result Result = Engine->Vulkan.Device.createShaderModule (
              &ShaderModuleInfo[0], nullptr, &VertexShaderModule);

          if (Result != vk::Result::eSuccess)
            {
              std::future<void> ReturnVulkanCheck
                  = std::async (std::launch::async, Patache::Log::VulkanCheck,
                                "Vertex Shader Module", Result);

              return false;
            }

          // Fragment Shader Module
          Result = Engine->Vulkan.Device.createShaderModule (
              &ShaderModuleInfo[1], nullptr, &FragmentShaderModule);

          if (Result != vk::Result::eSuccess)
            {
              std::future<void> ReturnVulkanCheck
                  = std::async (std::launch::async, Patache::Log::VulkanCheck,
                                "Fragment Shader Module", Result);

              return false;
            }

          // Create Pipeline
          const vk::PipelineShaderStageCreateInfo ShaderStageInfo[2]{
            { .stage  = vk::ShaderStageFlagBits::eVertex,
              .module = VertexShaderModule,
              .pName  = "main" },
            { .stage  = vk::ShaderStageFlagBits::eFragment,
              .module = FragmentShaderModule,
              .pName  = "main" }
          };

          // Vertex Input & Input Assembly
          /*Vertex Triangle{};

          vk::VertexInputBindingDescription BindingDescription{
            .binding   = 0,
            .stride    = static_cast<std::uint32_t> (sizeof (Vertex)),
            .inputRate = vk::VertexInputRate::eVertex
          };

          vk::VertexInputAttributeDescription VertexInputAttrib[2]{
            // Position
            { .location = 0,
              .binding  = 0,
              .format   = vk::Format::eR32G32Sfloat,
              .offset   = offsetof (Vertex, pos) },
            // Color
            { .location = 1,
              .binding  = 0,
              .format   = vk::Format::eR32G32B32Sfloat,
              .offset   = offsetof (Vertex, color) }
          };*/

          const vk::PipelineVertexInputStateCreateInfo VertexInputStateInfo{
            .vertexBindingDescriptionCount   = 0,
            .pVertexBindingDescriptions      = nullptr,
            .vertexAttributeDescriptionCount = 0,
            .pVertexAttributeDescriptions    = nullptr
          };

          constexpr const vk::PipelineInputAssemblyStateCreateInfo
                          InputAssemblyStateInfo{ .topology
                                      = vk::PrimitiveTopology::eTriangleList,
                                                  .primitiveRestartEnable = VK_FALSE };

          // Dynamic State
          constexpr const vk::DynamicState DynamicStates[2]{
            vk::DynamicState::eViewport, vk::DynamicState::eScissor
          };

          const vk::PipelineDynamicStateCreateInfo DynamicStateInfo{
            .dynamicStateCount = 2, .pDynamicStates = DynamicStates
          };

          // Viewport and Scissor
          Engine->Vulkan.Viewport.x = 0;
          Engine->Vulkan.Viewport.y = 0;
          Engine->Vulkan.Viewport.width
              = static_cast<float> (Engine->Vulkan.SwapChainExtent.width);
          Engine->Vulkan.Viewport.height
              = static_cast<float> (Engine->Vulkan.SwapChainExtent.height);
          Engine->Vulkan.Viewport.minDepth = 0.0f;
          Engine->Vulkan.Viewport.maxDepth = 1.0f;

          Engine->Vulkan.Scissor.offset = vk::Offset2D (0, 0);
          Engine->Vulkan.Scissor.extent = Engine->Vulkan.SwapChainExtent;

          const vk::PipelineViewportStateCreateInfo ViewportStateInfo{
            .viewportCount = 1,
            .pViewports    = &Engine->Vulkan.Viewport,
            .scissorCount  = 1,
            .pScissors     = &Engine->Vulkan.Scissor
          };

          // Rasterizer
          constexpr const vk::PipelineRasterizationStateCreateInfo
                          RasterizerStateInfo{ .depthClampEnable = VK_FALSE,
                                               .rasterizerDiscardEnable = VK_FALSE,
                                               .polygonMode = vk::PolygonMode::eFill,
                                               .cullMode = vk::CullModeFlagBits::eBack,
                                               .frontFace = vk::FrontFace::eClockwise,
                                               .depthBiasEnable = VK_FALSE,
                                               .depthBiasConstantFactor = 0.0f,
                                               .depthBiasClamp          = 0.0f,
                                               .depthBiasSlopeFactor    = 0.0f,
                                               .lineWidth = 1.0f };

          // MSAA Multisampling
          const vk::PipelineMultisampleStateCreateInfo MultisamplingStateInfo{
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
          constexpr const vk::PipelineColorBlendAttachmentState
                          ColorBlendAttachmentState{
                            .blendEnable         = VK_FALSE,
                            .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
                            .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
                            .colorBlendOp        = vk::BlendOp::eAdd,
                            .srcAlphaBlendFactor = vk::BlendFactor::eOne,
                            .dstAlphaBlendFactor = vk::BlendFactor::eZero,
                            .alphaBlendOp        = vk::BlendOp::eAdd,
                            .colorWriteMask = vk::ColorComponentFlagBits::eB
                                  | vk::ColorComponentFlagBits::eG
                                  | vk::ColorComponentFlagBits::eR
                                  | vk::ColorComponentFlagBits::eA
              };

          const vk::PipelineColorBlendStateCreateInfo ColorBlendStateInfo{
            .logicOpEnable   = VK_FALSE,
            .logicOp         = vk::LogicOp::eCopy,
            .attachmentCount = 1,
            .pAttachments    = &ColorBlendAttachmentState
          };

          const vk::PipelineLayoutCreateInfo PipelineLayoutInfo{
            .setLayoutCount         = 0,
            .pSetLayouts            = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges    = nullptr
          };

          Result = Engine->Vulkan.Device.createPipelineLayout (
              &PipelineLayoutInfo, nullptr, &Engine->Vulkan.PipelineLayout);
          if (Result != vk::Result::eSuccess)
            {
              std::future<void> ReturnVulkanCheck
                  = std::async (std::launch::async, Patache::Log::VulkanCheck,
                                "Pipeline Layout", Result);

              return false;
            }

          const vk::GraphicsPipelineCreateInfo GraphicsPipelineInfo{
            .stageCount          = 2,
            .pStages             = ShaderStageInfo,
            .pVertexInputState   = &VertexInputStateInfo,
            .pInputAssemblyState = &InputAssemblyStateInfo,
            .pViewportState      = &ViewportStateInfo,
            .pRasterizationState = &RasterizerStateInfo,
            .pMultisampleState   = &MultisamplingStateInfo,
            .pDepthStencilState  = nullptr,
            .pColorBlendState    = &ColorBlendStateInfo,
            .pDynamicState       = &DynamicStateInfo,
            .layout              = Engine->Vulkan.PipelineLayout,
            .renderPass          = Engine->Vulkan.RenderPass
          };

          Result = Engine->Vulkan.Device.createGraphicsPipelines (
              VK_NULL_HANDLE, 1, &GraphicsPipelineInfo, nullptr,
              &Engine->Vulkan.GraphicsPipeline);
          if (Result != vk::Result::eSuccess)
            {
              std::future<void> ReturnVulkanCheck
                  = std::async (std::launch::async, Patache::Log::VulkanCheck,
                                "Graphics Pipeline", Result);

              return false;
            }

          Engine->Vulkan.Device.destroyShaderModule (VertexShaderModule);
          Engine->Vulkan.Device.destroyShaderModule (FragmentShaderModule);

          return true;
        });

  std::future<void> VulkanInfo_Async
      = std::async (std::launch::async, VulkanInfo, std::ref (Engine),
                    std::ref (SwapChainInfo));

#if PATACHE_DEBUG == 1
  CreateImguiPipelineCache_Async.wait ();
  if (!CreateImguiPipelineCache_Async.get ())
    return false;

  CreateImguiDescriptorPool_Async.wait ();
  if (!CreateImguiDescriptorPool_Async.get ())
    return false;

  InitImguiVulkan (Engine);
#endif

  /*
  CreateDepthBuffer_Async.wait();
  if (!CreateDepthBuffer_Async.get()) return false;
  */

  CreateFrameBuffer_Async.wait ();
  if (!CreateFrameBuffer_Async.get ())
    return false;

  CreateCommandBuffer_Async.wait ();
  if (!CreateCommandBuffer_Async.get ())
    return false;

  CreateFence_Async.wait ();
  if (!CreateFence_Async.get ())
    return false;

  CreateSemaphores_Async.wait ();
  if (!CreateSemaphores_Async.get ())
    return false;

  CreatePipeline_Async.wait ();
  if (!CreatePipeline_Async.get ())
    return false;

  return true;
}

void
RaccoonRendererClose (Patache::VulkanBackend & Vulkan)
{
  if (Vulkan.Device == VK_NULL_HANDLE || Vulkan.Instance == VK_NULL_HANDLE)
    return;

  vk::Result Result = Vulkan.Device.waitIdle ();

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patache::Log::VulkanCheck,
                        "Device Wait Idle", Result);
    }

  if (Vulkan.Queue == VK_NULL_HANDLE)
    return;

  Result = Vulkan.Queue.waitIdle ();

  if (Result != vk::Result::eSuccess)
    std::future<void> ReturnVulkanCheck
        = std::async (std::launch::async, Patache::Log::VulkanCheck,
                      "Queue Wait Idle", Result);

  // Color
  for (std::uint8_t i = 0; i < Vulkan.SwapChainImageCount; ++i)
    {
      Vulkan.Device.destroyFramebuffer (Vulkan.SwapChainFrameBuffer[i]);
      Vulkan.Device.destroyImageView (Vulkan.SwapChainColorImageView[i]);
    }

  delete[] Vulkan.SwapChainFrameBuffer;
  delete[] Vulkan.SwapChainColorImageView;
  Vulkan.SwapChainFrameBuffer    = VK_NULL_HANDLE;
  Vulkan.SwapChainColorImageView = VK_NULL_HANDLE;

  Vulkan.Device.destroySwapchainKHR (Vulkan.SwapChain);
  delete[] Vulkan.SwapChainImages;
  Vulkan.SwapChainImages = VK_NULL_HANDLE;

  // Vulkan.Device.destroyBuffer (Vulkan.VertexBuffer);
  // Vulkan.Device.freeMemory (Vulkan.VertexBufferMemory);

// Imgui
#if PATACHE_DEBUG == 1
  ImGuiIO * io = nullptr;

  if (ImGui::GetCurrentContext () != nullptr)
    {
      io = &ImGui::GetIO ();

      if (io->BackendPlatformName != nullptr)
        ImGui_ImplVulkan_Shutdown ();
    }

  Vulkan.Device.destroyDescriptorPool (Vulkan.ImguiDescriptorPool);
  Vulkan.Device.destroyPipelineCache (Vulkan.ImguiPipelineCache);
#endif

  Vulkan.Device.destroyPipelineCache (Vulkan.GraphicsPipelineCache);
  Vulkan.Device.destroyPipelineLayout (Vulkan.PipelineLayout);
  Vulkan.Device.destroyPipeline (Vulkan.GraphicsPipeline);

  Vulkan.Device.destroyRenderPass (Vulkan.RenderPass);

  // Primitivas de sincronizacion
  for (std::uint32_t i = 0; i < Vulkan.SwapChainImageCount; ++i)
    {
      Vulkan.Device.destroySemaphore (Vulkan.ImageAvailableSemaphore[i]);
      Vulkan.Device.destroySemaphore (Vulkan.ImageFinishedSemaphore[i]);
      Vulkan.Device.destroyFence (Vulkan.InFlightFences[i]);
    }
  delete[] Vulkan.ImageAvailableSemaphore;
  delete[] Vulkan.ImageFinishedSemaphore;
  delete[] Vulkan.InFlightFences;

  Vulkan.ImageAvailableSemaphore = VK_NULL_HANDLE;
  Vulkan.ImageFinishedSemaphore  = VK_NULL_HANDLE;
  Vulkan.InFlightFences          = VK_NULL_HANDLE;

  Vulkan.Device.destroyCommandPool (Vulkan.CommandPool);
  delete[] Vulkan.Cmd;
  Vulkan.Cmd = VK_NULL_HANDLE;

  Vulkan.Device.destroy ();

  Vulkan.Instance.destroySurfaceKHR (Vulkan.Surface);

#if PATACHE_DEBUG == 1
  if (pfnVkCreateDebugUtilsMessengerEXT != nullptr)
    {
      pfnVkDestroyDebugUtilsMessengerEXT
          = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT> (
              Vulkan.Instance.getProcAddr ("vkDestroyDebugUtilsMessengerEXT"));

      if (pfnVkDestroyDebugUtilsMessengerEXT == nullptr)
        std::future<void> Err
            = std::async (std::launch::async, Patache::Log::ErrorMessage,
                          "Cannot get address of function "
                          "vkDestroyDebugUtilsMessengerEXT");
      else
        Vulkan.Instance.destroyDebugUtilsMessengerEXT (Vulkan.DebugMessenger);
    }
#endif

  Vulkan.Instance.destroy ();
}
