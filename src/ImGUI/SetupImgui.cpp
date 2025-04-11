#include "SetupImgui.hpp"

void
Patata::Engine::InitImgui (void)
{
  IMGUI_CHECKVERSION ();
  ImGui::CreateContext ();

  if (ImGui::GetCurrentContext () == nullptr)
    {
      fast_io::io::println (
#if defined(_WIN64)
          fast_io::out (),
#endif
          PATATA_TERM_BOLD,
          "Initialize Global ImGui Context : ", PATATA_TERM_RESET,
          PATATA_TERM_COLOR_YELLOW, "Fail\n", PATATA_TERM_RESET);

      return;
    }

  ImGuiIO & io = ImGui::GetIO ();
  static_cast<void> (io);
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard
                    | ImGuiConfigFlags_NavEnableGamepad
                    | ImGuiConfigFlags_DockingEnable;
  io.IniSavingRate = 0;
  io.IniFilename   = nullptr;
  io.LogFilename   = nullptr;

  ImFontConfig FontConfig;
  FontConfig.RasterizerDensity = 4.0f;
  FontConfig.OversampleH       = 4;
  FontConfig.OversampleV       = 4;
  // FontConfig.GlyphExtraSpacing.x = 1.008f;
  io.Fonts->AddFontDefault ();
  io.FontGlobalScale = 1.0f;
  io.Fonts->Build ();

  ImGui::StyleColorsDark ();
  ImGui::GetStyle ().FrameBorderSize             = 0.0f;
  ImGui::GetStyle ().FrameRounding               = 0.0f;
  ImGui::GetStyle ().GrabRounding                = 0.0f;
  ImGui::GetStyle ().WindowBorderSize            = 0.0f;
  ImGui::GetStyle ().PopupBorderSize             = 0.0f;
  ImGui::GetStyle ().ChildBorderSize             = 1.0f;
  ImGui::GetStyle ().TabRounding                 = 0.0f;
  ImGui::GetStyle ().TabBorderSize               = 0.0f;
  ImGui::GetStyle ().TabBarBorderSize            = 0.0f;
  ImGui::GetStyle ().ScrollbarRounding           = 0.0f;
  ImGui::GetStyle ().ItemSpacing                 = ImVec2 (8.0f, 12.0f);
  ImGui::GetStyle ().WindowPadding               = ImVec2 (20, 6);
  ImGui::GetStyle ().Colors[ImGuiCol_WindowBg].w = 0.82f;
  ImGui::GetStyle ().Colors[ImGuiCol_TitleBg]
      = ImGui::GetStyle ().Colors[ImGuiCol_WindowBg];
  ImGui::GetStyle ().Colors[ImGuiCol_DockingEmptyBg]
      = ImVec4 (0.0f, 0.0f, 0.0f, 0.0f);
}

bool
Patata::Engine::CreateImguiDescriptorPool (void)
{
  if (ImGui::GetCurrentContext () == nullptr)
    {
      std::future<void> Err
          = std::async (std::launch::async, Patata::Log::ErrorMessage,
                        "You cannot initialize the descriptor pool for imgui "
                        "without having initialized the ImGui context first");

      return false;
    }

  vk::DescriptorPoolSize PoolSize
      = { vk::DescriptorType::eCombinedImageSampler, 1 };

  vk::DescriptorPoolCreateInfo Info{
    .flags         = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
    .maxSets       = 1,
    .poolSizeCount = 1,
    .pPoolSizes    = &PoolSize
  };

  vk::Result Result = Vulkan.Device.createDescriptorPool (
      &Info, nullptr, &Vulkan.ImguiDescriptorPool);

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Imgui Descriptor Pool", Result);

      return false;
    }
  else
    return true;
}

bool
Patata::Engine::CreateImguiPipelineCache (void)
{
  if (ImGui::GetCurrentContext () == nullptr)
    {
      std::future<void> Err
          = std::async (std::launch::async, Patata::Log::ErrorMessage,
                        "You cannot initialize the pipeline cache for imgui "
                        "without having initialized the ImGui context first");

      return false;
    }

  vk::PipelineCacheCreateInfo Info{};

  vk::Result Result = Vulkan.Device.createPipelineCache (
      &Info, nullptr, &Vulkan.ImguiPipelineCache);
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Imgui PipeLine Cache", Result);
      return false;
    }
  else
    return true;
}

bool
Patata::Engine::InitImguiVulkan (void)
{
  if (ImGui::GetCurrentContext () == nullptr)
    {
      std::future<void> Err
          = std::async (std::launch::async, Patata::Log::ErrorMessage,
                        "You cannot initialize the implementation without "
                        "having initialized the ImGui context first");

      return false;
    }

  ImGui_ImplSDL2_InitForVulkan (GameWindow);

  vk::SurfaceCapabilitiesKHR SurfaceCapabilities;

  vk::Result Result = Vulkan.PhysicalDevice.getSurfaceCapabilitiesKHR (
      Vulkan.Surface, &SurfaceCapabilities);

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Get Surface Capabilities KHR", Result);

      return false;
    }

  ImGui_ImplVulkan_InitInfo init_info{
    .Instance       = static_cast<VkInstance> (Vulkan.Instance),
    .PhysicalDevice = static_cast<VkPhysicalDevice> (Vulkan.PhysicalDevice),
    .Device         = static_cast<VkDevice> (Vulkan.Device),
    .QueueFamily    = Vulkan.GraphicsQueueFamilyIndex,
    .Queue          = static_cast<VkQueue> (Vulkan.Queue),
    .DescriptorPool
    = static_cast<VkDescriptorPool> (Vulkan.ImguiDescriptorPool),
    .RenderPass    = static_cast<VkRenderPass> (Vulkan.RenderPass),
    .MinImageCount = SurfaceCapabilities.minImageCount,
    .ImageCount    = Vulkan.SwapChainImageCount,
    .MSAASamples   = VK_SAMPLE_COUNT_1_BIT,
    .PipelineCache = static_cast<VkPipelineCache> (Vulkan.ImguiPipelineCache),
    .Subpass       = 0,
    .DescriptorPoolSize          = 0,
    .UseDynamicRendering         = false,
    .PipelineRenderingCreateInfo = {},
    .Allocator                   = nullptr,
    .CheckVkResultFn             = nullptr,
    .MinAllocationSize           = 1048576
  };

  if (!ImGui_ImplVulkan_Init (&init_info))
    {
      fast_io::io::print (
#if defined(_WIN64)
          fast_io::out (),
#endif
          PATATA_TERM_BOLD,
          "ImGui Vulkan Implementation Initialization : ", PATATA_TERM_RESET,
          PATATA_TERM_COLOR_YELLOW, "Fail", PATATA_TERM_RESET);

      return false;
    }
  else
    return true;
}
