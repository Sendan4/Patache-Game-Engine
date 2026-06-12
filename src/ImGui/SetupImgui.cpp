#include <vulkan/vulkan.h>
#include "PatacheEngine/VmaUsage.hpp"
#include <SDL3/SDL.h>
#if PATACHE_DEBUG == 1
  #include <imgui_impl_sdl3.h>
  #include <imgui_impl_vulkan.h>
  #include <imgui_freetype.h>
#endif

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "Message.hpp"
#include "Vulkan_SetupLog.hpp"

#include "InterDisplay-Medium.hpp"

#include "SetupImgui.hpp"

void
Patache::InitImGuiCore (const Patache::Config & rConfiguration, Patache::EngineInfo & rDebugInfo)
{
  IMGUI_CHECKVERSION ();
  ImGui::CreateContext ();

  if (ImGui::GetCurrentContext () == nullptr)
    {
      Patache::FatalErrorMessage ("Patache Engine - ImGui",
                                  "Fail to initialize global ImGui Context", rConfiguration);

      return;
    }

  [[maybe_unused]] ImGuiIO & rIO{ ImGui::GetIO () };
  rIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad
                     | ImGuiConfigFlags_DockingEnable;
  rIO.IniSavingRate   = 0;
  rIO.IniFilename     = nullptr;
  rIO.LogFilename     = nullptr;

  ImFontConfig fontConfig{};
  fontConfig.RasterizerDensity = 4.0F;
  fontConfig.OversampleH       = 1;
  fontConfig.OversampleV       = 1;

  rIO.Fonts->AddFontFromMemoryCompressedTTF (sInterDisplay_Medium_compressed_data,
                                             sInterDisplay_Medium_compressed_size, 16.0F,
                                             &fontConfig, nullptr);

  // constexpr ImWchar ranges[] = { 0x1, 0x1FFFF, 0 };
  // FontConfig.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;
  // FontConfig.MergeMode = true;

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
  ImGui::GetStyle ().WindowPadding                   = ImVec2 (20.0F, 6.0F);
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
Patache::CreateImguiDescriptorPool (Patache::VulkanBackend & rVulkan)
{
  if (ImGui::GetCurrentContext () == nullptr)
    {
      Patache::ErrorMessage ("You cannot initialize the descriptor pool for imgui "
                             "without having initialized the ImGui context first");

      return false;
    }

  constexpr VkDescriptorPoolSize poolSize{ .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                           .descriptorCount
                                           = IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE };

  const VkDescriptorPoolCreateInfo info{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                                         .pNext = nullptr,
                                         .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
                                         .maxSets       = 12U,
                                         .poolSizeCount = 1U,
                                         .pPoolSizes    = &poolSize };

  const VkResult result{ vkCreateDescriptorPool (rVulkan.device, &info, nullptr,
                                                 &rVulkan.imguiDescriptorPool) };

  if (result != VK_SUCCESS)
    {
      Patache::VulkanCheck ("vkCreateDescriptorPool()", result);

      return false;
    }

  return true;
}

bool
Patache::CreateImguiPipelineCache (Patache::VulkanBackend & rVulkan)
{
  if (ImGui::GetCurrentContext () == nullptr)
    {
      Patache::ErrorMessage ("You cannot initialize the pipeline cache for imgui "
                             "without having initialized the ImGui context first");

      return false;
    }

  constexpr VkPipelineCacheCreateInfo info{ .sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
                                            .pNext = nullptr,
                                            .flags = 0U,
                                            .initialDataSize = 0U,
                                            .pInitialData    = nullptr };

  const VkResult result{ vkCreatePipelineCache (rVulkan.device, &info, nullptr,
                                                &rVulkan.imguiPipelineCache) };

  if (result != VK_SUCCESS)
    {
      Patache::VulkanCheck ("Imgui PipeLine Cache", result);

      return false;
    }

  return true;
}

bool
Patache::InitImGuiVulkan (Patache::Engine * const pEngine)
{
  if (ImGui::GetCurrentContext () == nullptr)
    {
      Patache::ErrorMessage ("You cannot initialize the implementation without "
                             "having initialized the ImGui context first");

      return false;
    }

  ImGui_ImplSDL3_InitForVulkan (pEngine->pGameWindow);

  VkSurfaceCapabilitiesKHR surfaceCapabilities{};

  const VkResult result{ vkGetPhysicalDeviceSurfaceCapabilitiesKHR (
      pEngine->vulkan.physicalDevice, pEngine->vulkan.surface, &surfaceCapabilities) };

  if (result != VK_SUCCESS)
    {
      Patache::VulkanCheck ("vkGetPhysicalDeviceSurfaceCapabilitiesKHR()", result);

      return false;
    }

  ImGui_ImplVulkan_InitInfo info{ .ApiVersion         = VK_API_VERSION_1_3,
                                  .Instance           = pEngine->vulkan.instance,
                                  .PhysicalDevice     = pEngine->vulkan.physicalDevice,
                                  .Device             = pEngine->vulkan.device,
                                  .QueueFamily        = pEngine->vulkan.graphicsQueueFamilyIndex,
                                  .Queue              = pEngine->vulkan.queue,
                                  .DescriptorPool     = pEngine->vulkan.imguiDescriptorPool,
                                  .DescriptorPoolSize = 0U,
                                  .MinImageCount      = surfaceCapabilities.minImageCount,
                                  .ImageCount         = pEngine->vulkan.swapchainImageCount,
                                  .PipelineCache      = pEngine->vulkan.imguiPipelineCache,
                                  .PipelineInfoMain{
                                      .RenderPass  = pEngine->vulkan.renderPass,
                                      .Subpass     = 0U,
                                      .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
                                      .ExtraDynamicStates{},
#ifdef IMGUI_IMPL_VULKAN_HAS_DYNAMIC_RENDERING
                                      .PipelineRenderingCreateInfo{},
#endif
                                      .SwapChainImageUsage = 0U,
                                  },
                                  .PipelineInfoForViewports{},
                                  .UseDynamicRendering = false,
                                  .Allocator           = nullptr,
                                  .CheckVkResultFn     = nullptr,
                                  .MinAllocationSize   = 1048576U,
                                  .CustomShaderVertCreateInfo{},
                                  .CustomShaderFragCreateInfo{} };

  if (!ImGui_ImplVulkan_Init (&info))
    {
      Patache::FatalErrorMessage ("Patache Engine - ImGui",
                                  "Fail to initialize ImGui Vulkan Implementation Context",
                                  pEngine->configuration);

      return false;
    }

  return true;
}
