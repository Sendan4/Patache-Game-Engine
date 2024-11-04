#include "SetupImgui.hpp"

void
Patata::Engine::EngineImpl::InitialImguiSetup (void)
{
  IMGUI_CHECKVERSION ();
  ImGui::CreateContext ();

  fast_io::io::print(
      #if defined(_WIN64)
      fast_io::out(),
      #endif
      PATATA_TERM_BOLD,
      "Initialize Global ImGui Context : ",
      PATATA_TERM_RESET
  );

  if (ImGui::GetCurrentContext() != nullptr)
  {
      fast_io::io::println(
          #if defined(_WIN64)
          fast_io::out(),
          #endif
          PATATA_TERM_COLOR_GREEN,
          "Success\n",
          PATATA_TERM_RESET
      );
  }
  else {
      fast_io::io::println(
          #if defined(_WIN64)
          fast_io::out(),
          #endif
          PATATA_TERM_COLOR_YELLOW,
          "Fail\n",
          PATATA_TERM_RESET
      );
      return;
  }

  ImGuiIO & io = ImGui::GetIO ();
  static_cast<void>(io);
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_DockingEnable;
  io.IniSavingRate = 0;
  io.IniFilename   = nullptr;
  io.LogFilename   = nullptr;
  //io.FontGlobalScale = 1.2f;

  ImGui::StyleColorsDark ();
  ImGui::GetStyle().FrameBorderSize = 0.0f;
  ImGui::GetStyle().FrameRounding = 2.8f;
  ImGui::GetStyle().GrabRounding = 4.0f;
  ImGui::GetStyle().WindowBorderSize = 1.0f;
  ImGui::GetStyle().PopupBorderSize = 0.0f;
  ImGui::GetStyle().ChildBorderSize = 0.0f;
  ImGui::GetStyle().TabRounding = 0.0f;
  ImGui::GetStyle().ScrollbarRounding = 0;
  ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
}

// Vulkan Backend
#include "RaccoonRenderer.hpp"

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateImguiDescriptorPool(void)
{
    if (ImGui::GetCurrentContext() == nullptr)
    {
        std::future<void> Err = std::async(std::launch::async,
            Patata::Log::ErrorMessage,
            "You cannot initialize the descriptor pool for imgui without having initialized the ImGui context first"
        );

        return false;
    }

    vk::DescriptorPoolSize PoolSize[] = {{ vk::DescriptorType::eCombinedImageSampler, 1 }};
    vk::DescriptorPoolCreateInfo Info (
        vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, // flags
        1,                                                    // maxSets
        static_cast<uint32_t>(IM_ARRAYSIZE(PoolSize)),        // poolSizeCount
        PoolSize,                                             // pPoolSizes
        nullptr                                               // pNext
    );

    vk::Result Result = Device.createDescriptorPool(&Info, nullptr, &ImguiDescriptorPool);

    {
        std::future<void> ReturnVulkanCheck = std::async(std::launch::async,
            Patata::Log::VulkanCheck, "Imgui Descriptor Pool", Result);
    }

    if (Result != vk::Result::eSuccess) return false;
    else return true;
}

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::CreateImguiPipelineCache(void)
{
    if (ImGui::GetCurrentContext() == nullptr)
    {
        std::future<void> Err = std::async(std::launch::async,
            Patata::Log::ErrorMessage,
            "You cannot initialize the pipeline cache for imgui without having initialized the ImGui context first"
        );

        return false;
    }

    vk::PipelineCacheCreateInfo Info {};
    vk::Result Result = Device.createPipelineCache(&Info, nullptr, &ImguiPipelineCache);

    {
        std::future<void> ReturnVulkanCheck = std::async(std::launch::async,
            Patata::Log::VulkanCheck, "Imgui PipeLine Cache", Result);
    }

    if (Result != vk::Result::eSuccess) return false;
    else return true;
}

bool
Patata::Graphics::RaccoonRenderer::VulkanBackend::VulkanImguiSetup (SDL_Window * Window)
{
    if (ImGui::GetCurrentContext() == nullptr)
    {
        std::future<void> Err = std::async(std::launch::async,
            Patata::Log::ErrorMessage,
            "You cannot initialize the implementation without having initialized the ImGui context first"
        );

        return false;
    }

    ImGui_ImplSDL2_InitForVulkan(Window);

    vk::SurfaceCapabilitiesKHR SurfaceCapabilities;

    vk::Result Result = PhysicalDevice.getSurfaceCapabilitiesKHR (Surface, &SurfaceCapabilities);
    if (Result != vk::Result::eSuccess) {
        std::future<void> ReturnVulkanCheck = std::async(std::launch::async,
			Patata::Log::VulkanCheck, "Get Surface Capabilities KHR", Result);

		return false;
    }

    ImGui_ImplVulkan_InitInfo init_info {
        .Instance = static_cast<VkInstance>(Instance),
        .PhysicalDevice = static_cast<VkPhysicalDevice>(PhysicalDevice),
        .Device = static_cast<VkDevice>(Device),
        .QueueFamily = GraphicsQueueFamilyIndex,
        .Queue = static_cast<VkQueue>(Queue),
        .DescriptorPool = static_cast<VkDescriptorPool>(ImguiDescriptorPool),
        .RenderPass = static_cast<VkRenderPass>(RenderPass),
        .MinImageCount = SurfaceCapabilities.minImageCount,
        .ImageCount = SwapChainImageCount,
        .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
        .PipelineCache = static_cast<VkPipelineCache>(ImguiPipelineCache),
        .Subpass = 0,
        .UseDynamicRendering = false,
        .Allocator = nullptr,
        .CheckVkResultFn = nullptr
    };

    fast_io::io::print(
        #if defined(_WIN64)
        fast_io::out(),
        #endif
        PATATA_TERM_BOLD,
        "ImGui Vulkan Implementation Initialization : ",
        PATATA_TERM_RESET
    );

    if (ImGui_ImplVulkan_Init(&init_info))
    {
        fast_io::io::println(
            #if defined(_WIN64)
            fast_io::out(),
            #endif
            PATATA_TERM_COLOR_GREEN,
            "Success",
            PATATA_TERM_RESET
        );
        return true;
    }
    else {
        fast_io::io::println(
            #if defined(_WIN64)
            fast_io::out(),
            #endif
            PATATA_TERM_COLOR_YELLOW,
            "Fail",
            PATATA_TERM_RESET
        );
        return false;
    }
}
