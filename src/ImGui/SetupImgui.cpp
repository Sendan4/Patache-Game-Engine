#include "SetupImgui.hpp"

void
InitImGuiCore (const Patache::Config & rConfiguration, Patache::EngineInfo & rDebugInfo)
{
  IMGUI_CHECKVERSION ();
  ImGui::CreateContext ();

  if (ImGui::GetCurrentContext () == nullptr)
    {
      std::future<void> fatalErr
          = std::async (std::launch::async, Patache::FatalErrorMessage, "Patache Engine - ImGui",
                        "Fail to initialize global ImGui Context", std::cref (rConfiguration));

      return;
    }

  [[maybe_unused]] ImGuiIO & rIO = ImGui::GetIO ();
  rIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad
                     | ImGuiConfigFlags_DockingEnable;
  rIO.IniSavingRate   = 0;
  rIO.IniFilename     = nullptr;
  rIO.LogFilename     = nullptr;
  rIO.FontGlobalScale = 1.0F;

  ImFontConfig fontConfig;
  fontConfig.RasterizerDensity = 4.0f;
  fontConfig.OversampleH       = 1;
  fontConfig.OversampleV       = 1;

  rIO.Fonts->AddFontFromMemoryCompressedTTF (sInterDisplay_Medium_compressed_data,
                                             sInterDisplay_Medium_compressed_size, 16.0F,
                                             &fontConfig, nullptr);

  // constexpr ImWchar ranges[] = { 0x1, 0x1FFFF, 0 };
  // FontConfig.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;
  // FontConfig.MergeMode = true;

  rIO.Fonts->Build ();

  // General styles
  ImGui::StyleColorsDark ();
  ImGui::GetStyle ().FrameRounding                   = 0.0F;
  ImGui::GetStyle ().GrabRounding                    = 0.0F;
  ImGui::GetStyle ().PopupBorderSize                 = 0.0F;
  ImGui::GetStyle ().ChildBorderSize                 = 1.0F;
  ImGui::GetStyle ().TabRounding                     = 0.0F;
  ImGui::GetStyle ().TabBorderSize                   = 0.0F;
  ImGui::GetStyle ().TabBarBorderSize                = 0.0F;
  ImGui::GetStyle ().ScrollbarRounding               = 0.0F;
  ImGui::GetStyle ().ItemSpacing                     = ImVec2 (8.0F, 12.0F);
  ImGui::GetStyle ().WindowPadding                   = ImVec2 (20, 6);
  ImGui::GetStyle ().Colors[ImGuiCol_WindowBg].w     = 0.90F;
  ImGui::GetStyle ().Colors[ImGuiCol_TitleBg]        = ImGui::GetStyle ().Colors[ImGuiCol_WindowBg];
  ImGui::GetStyle ().Colors[ImGuiCol_DockingEmptyBg] = ImVec4 (0.0F, 0.0F, 0.0F, 0.0F);

  // specific window engine styles
  rDebugInfo.engineStyles[0U]  = ImGui::GetStyle ().Colors[ImGuiCol_TitleBgActive];
  rDebugInfo.engineStyles[1U]  = ImGui::GetStyle ().Colors[ImGuiCol_ResizeGrip];
  rDebugInfo.engineStyles[2U]  = ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripActive];
  rDebugInfo.engineStyles[3U]  = ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripHovered];
  rDebugInfo.engineStyles[4U]  = ImGui::GetStyle ().Colors[ImGuiCol_SeparatorHovered];
  rDebugInfo.engineStyles[5U]  = ImGui::GetStyle ().Colors[ImGuiCol_SeparatorActive];
  rDebugInfo.engineStyles[6U]  = ImGui::GetStyle ().Colors[ImGuiCol_Tab];
  rDebugInfo.engineStyles[7U]  = ImGui::GetStyle ().Colors[ImGuiCol_TabSelected];
  rDebugInfo.engineStyles[8U]  = ImGui::GetStyle ().Colors[ImGuiCol_TabHovered];
  rDebugInfo.engineStyles[9U]  = ImGui::GetStyle ().Colors[ImGuiCol_Header];
  rDebugInfo.engineStyles[10U] = ImGui::GetStyle ().Colors[ImGuiCol_HeaderActive];
  rDebugInfo.engineStyles[11U] = ImGui::GetStyle ().Colors[ImGuiCol_HeaderHovered];
  rDebugInfo.engineStyles[12U] = ImGui::GetStyle ().Colors[ImGuiCol_Button];
  rDebugInfo.engineStyles[13U] = ImGui::GetStyle ().Colors[ImGuiCol_ButtonHovered];
  rDebugInfo.engineStyles[14U] = ImGui::GetStyle ().Colors[ImGuiCol_ButtonActive];
  rDebugInfo.engineStyles[15U] = ImGui::GetStyle ().Colors[ImGuiCol_FrameBg];
  rDebugInfo.engineStyles[16U] = ImGui::GetStyle ().Colors[ImGuiCol_FrameBgHovered];
  rDebugInfo.engineStyles[17U] = ImGui::GetStyle ().Colors[ImGuiCol_FrameBgActive];
  rDebugInfo.engineStyles[18U] = ImGui::GetStyle ().Colors[ImGuiCol_CheckMark];
  rDebugInfo.engineStyles[19U] = ImGui::GetStyle ().Colors[ImGuiCol_SliderGrab];
  rDebugInfo.engineStyles[20U] = ImGui::GetStyle ().Colors[ImGuiCol_SliderGrabActive];
}

bool
CreateImguiDescriptorPool (Patache::VulkanBackend & rVulkan)
{
  if (ImGui::GetCurrentContext () == nullptr)
    {
      std::future<void> err = std::async (std::launch::async, Patache::ErrorMessage,
                                          "You cannot initialize the descriptor pool for imgui "
                                          "without having initialized the ImGui context first");

      return false;
    }

  constexpr vk::DescriptorPoolSize poolSize = { vk::DescriptorType::eCombinedImageSampler, 1U };

  const vk::DescriptorPoolCreateInfo info{ .flags
                                           = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
                                           .maxSets       = 1U,
                                           .poolSizeCount = 1U,
                                           .pPoolSizes    = &poolSize };

  vk::Result result
      = rVulkan.device.createDescriptorPool (&info, nullptr, &rVulkan.imguiDescriptorPool);

  if (result != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck
          = std::async (std::launch::async, Patache::VulkanCheck, "Imgui Descriptor Pool", result);

      return false;
    }
  else
    {
      return true;
    }
}

bool
CreateImguiPipelineCache (Patache::VulkanBackend & rVulkan)
{
  if (ImGui::GetCurrentContext () == nullptr)
    {
      std::future<void> err = std::async (std::launch::async, Patache::ErrorMessage,
                                          "You cannot initialize the pipeline cache for imgui "
                                          "without having initialized the ImGui context first");

      return false;
    }

  constexpr vk::PipelineCacheCreateInfo info{};

  vk::Result result
      = rVulkan.device.createPipelineCache (&info, nullptr, &rVulkan.imguiPipelineCache);

  if (result != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck
          = std::async (std::launch::async, Patache::VulkanCheck, "Imgui PipeLine Cache", result);

      return false;
    }
  else
    return true;
}

bool
InitImGuiVulkan (Patache::Engine * const pEngine)
{
  if (ImGui::GetCurrentContext () == nullptr)
    {
      std::future<void> err = std::async (std::launch::async, Patache::ErrorMessage,
                                          "You cannot initialize the implementation without "
                                          "having initialized the ImGui context first");

      return false;
    }

  ImGui_ImplSDL3_InitForVulkan (pEngine->pGameWindow);

  vk::SurfaceCapabilitiesKHR surfaceCapabilities;

  vk::Result result = pEngine->vulkan.physicalDevice.getSurfaceCapabilitiesKHR (
      pEngine->vulkan.surface, &surfaceCapabilities);

  if (result != vk::Result::eSuccess)
    {
      std::future<void> returnVulkanCheck = std::async (std::launch::async, Patache::VulkanCheck,
                                                        "Get Surface Capabilities KHR", result);

      return false;
    }

  ImGui_ImplVulkan_InitInfo info{
    .ApiVersion          = VK_API_VERSION_1_3,
    .Instance            = static_cast<VkInstance> (pEngine->vulkan.instance),
    .PhysicalDevice      = static_cast<VkPhysicalDevice> (pEngine->vulkan.physicalDevice),
    .Device              = static_cast<VkDevice> (pEngine->vulkan.device),
    .QueueFamily         = pEngine->vulkan.graphicsQueueFamilyIndex,
    .Queue               = static_cast<VkQueue> (pEngine->vulkan.queue),
    .DescriptorPool      = static_cast<VkDescriptorPool> (pEngine->vulkan.imguiDescriptorPool),
    .RenderPass          = static_cast<VkRenderPass> (pEngine->vulkan.renderPass),
    .MinImageCount       = surfaceCapabilities.minImageCount,
    .ImageCount          = pEngine->vulkan.swapchainImageCount,
    .MSAASamples         = VK_SAMPLE_COUNT_1_BIT,
    .PipelineCache       = static_cast<VkPipelineCache> (pEngine->vulkan.imguiPipelineCache),
    .Subpass             = 0U,
    .DescriptorPoolSize  = 0U,
    .UseDynamicRendering = false,
    .PipelineRenderingCreateInfo = {},
    .Allocator                   = nullptr,
    .CheckVkResultFn             = nullptr,
    .MinAllocationSize           = 1048576U
  };

  if (!ImGui_ImplVulkan_Init (&info))
    {
      std::future<void> fatalErr
          = std::async (std::launch::async, Patache::FatalErrorMessage, "Patache Engine - ImGui",
                        "Fail to initialize ImGui Vulkan Implementation Context",
                        std::cref (pEngine->configuration));

      return false;
    }

  return true;
}
