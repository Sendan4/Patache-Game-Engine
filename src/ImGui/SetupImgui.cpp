#include "SetupImgui.hpp"

void
InitImgui (const Patache::Config & configuration, Patache::EngineInfo & engineInfo)
{
  IMGUI_CHECKVERSION ();
  ImGui::CreateContext ();

  if (ImGui::GetCurrentContext () == nullptr)
    {
      std::future<void> FatalErr = std::async (
          std::launch::async, Patache::Log::FatalErrorMessage, "Patache Engine - ImGui",
          "Fail to initialize global ImGui Context", configuration);

      return;
    }

  ImGuiIO & io = ImGui::GetIO ();
  static_cast<void> (io);
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad
                    | ImGuiConfigFlags_DockingEnable;
  io.IniSavingRate   = 0;
  io.IniFilename     = nullptr;
  io.LogFilename     = nullptr;
  io.FontGlobalScale = 1.0f;

  ImFontConfig FontConfig;
  FontConfig.RasterizerDensity = 4.0f;
  FontConfig.OversampleH       = 1;
  FontConfig.OversampleV       = 1;

  io.Fonts->AddFontFromMemoryCompressedTTF (InterDisplay_Medium_compressed_data,
                                            InterDisplay_Medium_compressed_size, 16.0f, &FontConfig,
                                            nullptr);

  // static constexpr ImWchar ranges[] = { 0x1, 0x1FFFF, 0 };
  //  FontConfig.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;
  //  FontConfig.MergeMode = true;

  io.Fonts->Build ();

  // General styles
  ImGui::StyleColorsDark ();
  ImGui::GetStyle ().FrameRounding                   = 0.0f;
  ImGui::GetStyle ().GrabRounding                    = 0.0f;
  ImGui::GetStyle ().PopupBorderSize                 = 0.0f;
  ImGui::GetStyle ().ChildBorderSize                 = 1.0f;
  ImGui::GetStyle ().TabRounding                     = 0.0f;
  ImGui::GetStyle ().TabBorderSize                   = 0.0f;
  ImGui::GetStyle ().TabBarBorderSize                = 0.0f;
  ImGui::GetStyle ().ScrollbarRounding               = 0.0f;
  ImGui::GetStyle ().ItemSpacing                     = ImVec2 (8.0f, 12.0f);
  ImGui::GetStyle ().WindowPadding                   = ImVec2 (20, 6);
  ImGui::GetStyle ().Colors[ImGuiCol_WindowBg].w     = 0.90f;
  ImGui::GetStyle ().Colors[ImGuiCol_TitleBg]        = ImGui::GetStyle ().Colors[ImGuiCol_WindowBg];
  ImGui::GetStyle ().Colors[ImGuiCol_DockingEmptyBg] = ImVec4 (0.0f, 0.0f, 0.0f, 0.0f);

  // specific window engine styles
  engineInfo.engineStyles[0]  = ImGui::GetStyle ().Colors[ImGuiCol_TitleBgActive];
  engineInfo.engineStyles[1]  = ImGui::GetStyle ().Colors[ImGuiCol_ResizeGrip];
  engineInfo.engineStyles[2]  = ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripActive];
  engineInfo.engineStyles[3]  = ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripHovered];
  engineInfo.engineStyles[4]  = ImGui::GetStyle ().Colors[ImGuiCol_SeparatorHovered];
  engineInfo.engineStyles[5]  = ImGui::GetStyle ().Colors[ImGuiCol_SeparatorActive];
  engineInfo.engineStyles[6]  = ImGui::GetStyle ().Colors[ImGuiCol_Tab];
  engineInfo.engineStyles[7]  = ImGui::GetStyle ().Colors[ImGuiCol_TabSelected];
  engineInfo.engineStyles[8]  = ImGui::GetStyle ().Colors[ImGuiCol_TabHovered];
  engineInfo.engineStyles[9]  = ImGui::GetStyle ().Colors[ImGuiCol_Header];
  engineInfo.engineStyles[10] = ImGui::GetStyle ().Colors[ImGuiCol_HeaderActive];
  engineInfo.engineStyles[11] = ImGui::GetStyle ().Colors[ImGuiCol_HeaderHovered];
  engineInfo.engineStyles[12] = ImGui::GetStyle ().Colors[ImGuiCol_Button];
  engineInfo.engineStyles[13] = ImGui::GetStyle ().Colors[ImGuiCol_ButtonHovered];
  engineInfo.engineStyles[14] = ImGui::GetStyle ().Colors[ImGuiCol_ButtonActive];
  engineInfo.engineStyles[15] = ImGui::GetStyle ().Colors[ImGuiCol_FrameBg];
  engineInfo.engineStyles[16] = ImGui::GetStyle ().Colors[ImGuiCol_FrameBgHovered];
  engineInfo.engineStyles[17] = ImGui::GetStyle ().Colors[ImGuiCol_FrameBgActive];
  engineInfo.engineStyles[18] = ImGui::GetStyle ().Colors[ImGuiCol_CheckMark];
  engineInfo.engineStyles[19] = ImGui::GetStyle ().Colors[ImGuiCol_SliderGrab];
  engineInfo.engineStyles[20] = ImGui::GetStyle ().Colors[ImGuiCol_SliderGrabActive];
}

bool
CreateImguiDescriptorPool (Patache::VulkanBackend & Vulkan)
{
  if (ImGui::GetCurrentContext () == nullptr)
    {
      std::future<void> Err = std::async (std::launch::async, Patache::Log::ErrorMessage,
                                          "You cannot initialize the descriptor pool for imgui "
                                          "without having initialized the ImGui context first");

      return false;
    }

  static constexpr vk::DescriptorPoolSize PoolSize
      = { vk::DescriptorType::eCombinedImageSampler, 1 };

  const vk::DescriptorPoolCreateInfo Info{ .flags
                                           = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
                                           .maxSets       = 1,
                                           .poolSizeCount = 1,
                                           .pPoolSizes    = &PoolSize };

  vk::Result Result
      = Vulkan.Device.createDescriptorPool (&Info, nullptr, &Vulkan.ImguiDescriptorPool);

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patache::Log::VulkanCheck, "Imgui Descriptor Pool", Result);

      return false;
    }
  else
    return true;
}

bool
CreateImguiPipelineCache (Patache::VulkanBackend & Vulkan)
{
  if (ImGui::GetCurrentContext () == nullptr)
    {
      std::future<void> Err = std::async (std::launch::async, Patache::Log::ErrorMessage,
                                          "You cannot initialize the pipeline cache for imgui "
                                          "without having initialized the ImGui context first");

      return false;
    }

  constexpr vk::PipelineCacheCreateInfo Info{};

  vk::Result Result
      = Vulkan.Device.createPipelineCache (&Info, nullptr, &Vulkan.ImguiPipelineCache);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patache::Log::VulkanCheck, "Imgui PipeLine Cache", Result);
      return false;
    }
  else
    return true;
}

bool
InitImguiVulkan (Patache::Engine * const Engine)
{
  if (ImGui::GetCurrentContext () == nullptr)
    {
      std::future<void> Err = std::async (std::launch::async, Patache::Log::ErrorMessage,
                                          "You cannot initialize the implementation without "
                                          "having initialized the ImGui context first");

      return false;
    }

  ImGui_ImplSDL3_InitForVulkan (Engine->GameWindow);

  vk::SurfaceCapabilitiesKHR SurfaceCapabilities;

  vk::Result Result = Engine->Vulkan.PhysicalDevice.getSurfaceCapabilitiesKHR (
      Engine->Vulkan.Surface, &SurfaceCapabilities);

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck = std::async (
          std::launch::async, Patache::Log::VulkanCheck, "Get Surface Capabilities KHR", Result);

      return false;
    }

  ImGui_ImplVulkan_InitInfo ImGuiImplInitInfo{
    .ApiVersion          = VK_API_VERSION_1_3,
    .Instance            = static_cast<VkInstance> (Engine->Vulkan.Instance),
    .PhysicalDevice      = static_cast<VkPhysicalDevice> (Engine->Vulkan.PhysicalDevice),
    .Device              = static_cast<VkDevice> (Engine->Vulkan.Device),
    .QueueFamily         = Engine->Vulkan.GraphicsQueueFamilyIndex,
    .Queue               = static_cast<VkQueue> (Engine->Vulkan.Queue),
    .DescriptorPool      = static_cast<VkDescriptorPool> (Engine->Vulkan.ImguiDescriptorPool),
    .RenderPass          = static_cast<VkRenderPass> (Engine->Vulkan.RenderPass),
    .MinImageCount       = SurfaceCapabilities.minImageCount,
    .ImageCount          = Engine->Vulkan.SwapChainImageCount,
    .MSAASamples         = VK_SAMPLE_COUNT_1_BIT,
    .PipelineCache       = static_cast<VkPipelineCache> (Engine->Vulkan.ImguiPipelineCache),
    .Subpass             = 0,
    .DescriptorPoolSize  = 0,
    .UseDynamicRendering = false,
    .PipelineRenderingCreateInfo = {},
    .Allocator                   = nullptr,
    .CheckVkResultFn             = nullptr,
    .MinAllocationSize           = 1048576
  };

  if (!ImGui_ImplVulkan_Init (&ImGuiImplInitInfo))
    {
      std::future<void> FatalErr = std::async (
          std::launch::async, Patache::Log::FatalErrorMessage, "Patache Engine - ImGui",
          "Fail to initialize ImGui Vulkan Implementation Context", Engine->configuration);

      return false;
    }

  return true;
}
