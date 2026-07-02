#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include "PatacheEngine/VmaUsage.hpp"
#include <SDL3/SDL.h>
#include <imgui.h>

#include "PatacheEngine/PatacheEngine.hpp"
#include "CstringWrapped.hpp"
#include "StartLogInfo.hpp"
#include "UnitSize.hpp"

namespace Patache
{
// Vendor
static constexpr ImVec4                  sImGuiIntelColor{ ImVec4 (0.314F, 0.759F, 0.950F, 1.0F) };
static constexpr ImVec4                  sImGuiAMDColor{ ImVec4 (1.0F, 0.396F, 0.396F, 1.0F) };
static constexpr ImVec4                  sImGuiNvidiaColor{ ImVec4 (0.463F, 0.725F, 0.0F, 1.0F) };
static constexpr ImVec4                  sImGuiMesaColor{ ImVec4 (0.844F, 0.867F, 0.870F, 1.0F) };
[[maybe_unused]] static constexpr ImVec4 sImGuiUnknownColor{ ImVec4 (0.640F, 0.646F, 0.646F,
                                                                     1.0F) };

// Table
static constexpr ImVec2                  sImGuiTablePadding{ ImVec2 (10, 4) };
[[maybe_unused]] static constexpr ImVec4 sImGuiTableTitleColor{ ImVec4 (0.930F, 0.718F, 0.223F,
                                                                        1.0F) };
static constexpr ImGuiTableFlags         sImGuiTableFlags{ ImGuiTableFlags_NoHostExtendX
                                                           | ImGuiTableFlags_RowBg
                                                           | ImGuiTableFlags_BordersH
                                                           | ImGuiTableFlags_BordersV };

// Colors -> Positive, Warning, Error
static constexpr ImVec4 sImGuiPositiveValue{ ImVec4 (0.634F, 0.990F, 0.657F, 1.0F) };
static constexpr ImVec4 sImGuiWarningValue{ ImVec4 (0.984F, 0.990F, 0.634F, 1.0F) };
[[maybe_unused]] static constexpr ImVec4 sImGuiErrorValue{ ImVec4 (0.990F, 0.669F, 0.634F, 1.0F) };

// Accent Color
static constexpr ImVec4 sImGuiPatacheColor{ ImVec4 (0.596F, 0.463F, 0.278F, 1.0F) };
static constexpr ImVec4 sImGuiSelectedColor{ ImVec4 (0.868F, 0.663F, 0.380F, 1.0F) };
static constexpr ImVec4 sImGuiDarkerColor{ ImVec4 (0.353F, 0.267F, 0.153F, 1.0F) };

// Collapse Header Color
static constexpr ImVec4 sImGuiCollapseHeaderColor{ ImVec4 (0.341F, 0.341F, 0.341F, 1.0F) };
static constexpr ImVec4 sImGuiCollapseHeaderDarkerColor{ ImVec4 (0.200F, 0.200F, 0.200F, 1.0F) };
static constexpr ImVec4 sImGuiCollapseHeaderSelectedColor{ ImVec4 (0.404F, 0.404F, 0.404F, 1.0F) };

// Corner Border
static constexpr ImVec4 sImGuiCornerBorderColor{ ImVec4 (1.0F, 1.0F, 1.0F, 1.0F) };

// FrameBG
static constexpr ImVec4 sImGuiFrameBGColor{ ImVec4 (0.21176470588235294F, 0.1803921568627451F,
                                                    0.12549019607843137F, 1.0F) };
static constexpr ImVec4 sImGuiFrameBGHoveredColor{ ImVec4 (
    0.36470588235294116F, 0.28627450980392155F, 0.1450980392156863F, 1.0F) };
static constexpr ImVec4 sImGuiFrameBGActiveColor{ ImVec4 (0.5372549019607843F, 0.4235294117647059F,
                                                          0.21568627450980393F, 1.0F) };

// Chec kMark
static constexpr ImVec4 sImGuiCheckMarkColor{ ImVec4 (0.9647058823529412F, 0.7568627450980392F,
                                                      0.3803921568627451F, 1.0F) };

// Slider Grab
static constexpr ImVec4 sImGuiSliderGrabColor{ ImVec4 (0.8156862745098039F, 0.6392156862745098F,
                                                       0.3176470588235294F, 1.0F) };
static constexpr ImVec4 sImGuiSliderGrabActiveColor{ ImVec4 (
    0.8941176470588236F, 0.6980392156862745F, 0.3411764705882353F, 1.0F) };

[[maybe_unused]] static constexpr ImVec4 sImGuiNoneColor{ ImVec4 (0.0F, 0.0F, 0.0F, 1.0F) };
}

#include "DebugUI.hpp"

void
Patache::DrawDebugUI (Patache::Engine * const pEngine)
{
  // Patache Style
  ImGui::GetStyle ().Colors[ImGuiCol_TitleBgActive] = ImVec4 (Patache::sImGuiPatacheColor);

  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGrip]        = ImVec4 (Patache::sImGuiPatacheColor);
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripActive]  = ImVec4 (Patache::sImGuiPatacheColor);
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripHovered] = ImVec4 (Patache::sImGuiSelectedColor);

  ImGui::GetStyle ().Colors[ImGuiCol_SeparatorHovered] = ImVec4 (Patache::sImGuiCornerBorderColor);
  ImGui::GetStyle ().Colors[ImGuiCol_SeparatorActive]  = ImVec4 (Patache::sImGuiCornerBorderColor);

  ImGui::GetStyle ().Colors[ImGuiCol_Tab]         = ImVec4 (Patache::sImGuiDarkerColor);
  ImGui::GetStyle ().Colors[ImGuiCol_TabSelected] = ImVec4 (Patache::sImGuiPatacheColor);
  ImGui::GetStyle ().Colors[ImGuiCol_TabHovered]  = ImVec4 (Patache::sImGuiSelectedColor);

  ImGui::GetStyle ().Colors[ImGuiCol_Button]        = ImVec4 (Patache::sImGuiPatacheColor);
  ImGui::GetStyle ().Colors[ImGuiCol_ButtonHovered] = ImVec4 (Patache::sImGuiSelectedColor);
  ImGui::GetStyle ().Colors[ImGuiCol_ButtonActive]  = ImVec4 (Patache::sImGuiDarkerColor);

  ImGui::GetStyle ().Colors[ImGuiCol_Header] = ImVec4 (Patache::sImGuiCollapseHeaderDarkerColor);
  ImGui::GetStyle ().Colors[ImGuiCol_HeaderActive] = ImVec4 (Patache::sImGuiCollapseHeaderColor);
  ImGui::GetStyle ().Colors[ImGuiCol_HeaderHovered]
      = ImVec4 (Patache::sImGuiCollapseHeaderSelectedColor);

  ImGui::GetStyle ().Colors[ImGuiCol_FrameBg]        = ImVec4 (Patache::sImGuiFrameBGColor);
  ImGui::GetStyle ().Colors[ImGuiCol_FrameBgHovered] = ImVec4 (Patache::sImGuiFrameBGHoveredColor);
  ImGui::GetStyle ().Colors[ImGuiCol_FrameBgActive]  = ImVec4 (Patache::sImGuiFrameBGActiveColor);

  ImGui::GetStyle ().Colors[ImGuiCol_CheckMark] = ImVec4 (Patache::sImGuiCheckMarkColor);

  ImGui::GetStyle ().Colors[ImGuiCol_SliderGrab] = ImVec4 (Patache::sImGuiSliderGrabColor);
  ImGui::GetStyle ().Colors[ImGuiCol_SliderGrabActive]
      = ImVec4 (Patache::sImGuiSliderGrabActiveColor);
  // End Patache Style

  // Menu Bar
  if (pEngine->debugInfo.showMainMenuBar)
    {
      ImGui::GetStyle ().Colors[ImGuiCol_Button] = ImVec4 (0.0F, 0.0F, 0.0F, 0.0F);

      if (ImGui::BeginMainMenuBar ())
        {
          if (ImGui::Button ("(Patache Icon)"))
            {
              pEngine->debugInfo.showMainMenuBar ^= 0b00000001; // Cambiar bits
            }

          if (ImGui::BeginMenu ("Raccoon"))
            {
              if (ImGui::MenuItem ("Renderer", ""))
                {
                  pEngine->debugInfo.raccoonRendererInfoWindow ^= 0b00000001; // Cambiar bits
                }

              // No implementado aun
              // ImGui::MenuItem ("Profiler", "");
              // ImGui::MenuItem ("Audio", "");

              ImGui::EndMenu ();
            }

          if (ImGui::Button ("Configuration"))
            {
              pEngine->debugInfo.configWindow ^= 0b00000001; // Cambiar bits
            }

          if (ImGui::Button ("About"))
            {
              pEngine->debugInfo.infoWindow ^= 0b00000001; // Cambiar bits
            }

          ImGui::EndMainMenuBar ();
        }

      ImGui::GetStyle ().Colors[ImGuiCol_Button] = ImVec4 (Patache::sImGuiPatacheColor);
    }
  // End Menu Bar

  // Patache Window Info
  if (pEngine->debugInfo.infoWindow)
    {
      ImGui::SetNextWindowSize (ImVec2 (400.0F, 440.0F), ImGuiCond_FirstUseEver);

      ImGui::Begin (PATACHE_ENGINE_NAME, &pEngine->debugInfo.infoWindow);

      ImGui::PushStyleVar (ImGuiStyleVar_CellPadding, ImVec2 (Patache::sImGuiTablePadding));

      // Basic info
#if defined(PATACHE_ENGINE_VERSION)
      ImGui::Text ("VERSION : %s", PATACHE_ENGINE_VERSION);
#endif
#if defined(__DATE__) && defined(__TIME__)
      ImGui::Text ("BUILD DATE : %s %s", __DATE__, __TIME__);
#endif
#if defined(PATACHE_OS)
      ImGui::Text ("OPERATING SYSTEM : %s", PATACHE_OS);
#endif
#if defined(PATACHE_ENGINE_VERSION)
      ImGui::Spacing ();
#endif

      // Text about the pEngine
      ImGui::Text ("Simple and free Game Engine");
      ImGui::Text ("%s is under the GPLv3 License.", PATACHE_ENGINE_NAME);
      ImGui::Spacing ();

      // below is the advanced Info
      // Git
#if defined(PATACHE_USE_GIT)
      if (ImGui::CollapsingHeader ("GIT"))
        {
          ImGui::Text ("BRANCH : %s", PATACHE_GIT_BRANCH);
          ImGui::Text ("COMMIT HASH : %s", PATACHE_GIT_HASH_SHORT);
          ImGui::SetItemTooltip ("LONG HASH : %s", PATACHE_GIT_HASH_LONG);

          ImGui::Text ("SOURCE STATE :");

          if constexpr (PATACHE_GIT_DIRTY)
            {
              ImGui::SameLine ();
              ImGui::TextColored (ImVec4 (Patache::sImGuiWarningValue), "Dirty");
            }
          else
            {
              ImGui::SameLine ();
              ImGui::TextColored (ImVec4 (Patache::sImGuiPositiveValue), "Clean");
            }

          if constexpr (PATACHE_GIT_STAGE)
            {
              ImGui::SameLine ();
              ImGui::TextColored (ImVec4 (Patache::sImGuiWarningValue), "In stage area");
            }
          else
            {
              ImGui::SameLine ();
              ImGui::TextColored (ImVec4 (Patache::sImGuiPositiveValue), "Off Stage");
            }

          ImGui::Spacing ();
        }
#endif

        // Build Info
#if defined(PATACHE_COMPILER) && defined(PATACHE_COMPILER_VERSION)                                 \
    && defined(PATACHE_BUILD_SYSTEM) && defined(PATACHE_BUILD_SYSTEM_VERSION)                      \
    && defined(PATACHE_BUILD_SYSTEM_GENERATOR) && defined(PATACHE_BUILD_SYSTEM_GENERATOR_VERSION)
      if (ImGui::CollapsingHeader ("BUILD INFO"))
        {
          ImGui::Text ("COMPILER : %s %s", PATACHE_COMPILER, PATACHE_COMPILER_VERSION);

  #if defined(PATACHE_COMPILER_PROGRAM)
          ImGui::SetItemTooltip ("PROGRAM : %s", PATACHE_COMPILER_PROGRAM);
  #endif

          if (ImGui::TreeNode ("###BUILD_SYSTEM", "BUILD SYSTEM : %s %s", PATACHE_BUILD_SYSTEM,
                               PATACHE_BUILD_SYSTEM_VERSION))
            {

              ImGui::Text ("BACKEND : %s %s", PATACHE_BUILD_SYSTEM_GENERATOR,
                           PATACHE_BUILD_SYSTEM_GENERATOR_VERSION);

              ImGui::Spacing ();
              ImGui::TreePop ();
            }

  #if defined(PATACHE_BUILD_TYPE)
          ImGui::Text ("BUILD TYPE : %s", PATACHE_BUILD_TYPE);
  #endif
  #if defined(__GLIBC__) && defined(__GLIBC_MINOR__)
          ImGui::Text ("LIBC LINKED VERSION : %u.%u", __GLIBC__, __GLIBC_MINOR__);
  #endif
  #if defined(PATACHE_ARCH)
          ImGui::Text ("CPU ARCH TARGET : %s", PATACHE_ARCH);
  #endif
          ImGui::Spacing ();
        }
#endif

      // Window
      if (ImGui::CollapsingHeader ("WINDOW###WindowInfo"))
        {
          // List of SDL WINDOW CREATION FLAGS
          if (ImGui::TreeNode ("SDL CREATION FLAGS"))
            {
              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_FULLSCREEN)
                ImGui::BulletText ("fullscreen window");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_OPENGL)
                ImGui::BulletText ("window usable with OpenGL context");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_OCCLUDED)
                ImGui::BulletText ("window is occluded");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_HIDDEN)
                ImGui::BulletText ("window is not visible");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_BORDERLESS)
                ImGui::BulletText ("no window decoration");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_RESIZABLE)
                ImGui::BulletText ("window can be resized");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_MINIMIZED)
                ImGui::BulletText ("window is minimized");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_MAXIMIZED)
                ImGui::BulletText ("window is maximized");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_MOUSE_GRABBED)
                ImGui::BulletText ("window has grabbed mouse input");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_INPUT_FOCUS)
                ImGui::BulletText ("window has input focus");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_MOUSE_FOCUS)
                ImGui::BulletText ("window has mouse focus");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_EXTERNAL)
                ImGui::BulletText ("window not created by SDL");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_MODAL)
                ImGui::BulletText ("window is modal");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_HIGH_PIXEL_DENSITY)
                ImGui::BulletText ("window uses high pixel density back "
                                   "buffer if possible");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_MOUSE_CAPTURE)
                ImGui::BulletText ("window has mouse captured "
                                   "(unrelated to MOUSE_GRABBED)");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_MOUSE_RELATIVE_MODE)
                ImGui::BulletText ("window has relative mode enabled");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_ALWAYS_ON_TOP)
                ImGui::BulletText ("window should always be above others");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_UTILITY)
                ImGui::BulletText ("window should be treated as a utility window");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_TOOLTIP)
                ImGui::BulletText ("window should be treated as a tooltip");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_POPUP_MENU)
                ImGui::BulletText ("window should be treated as a popup menu");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_KEYBOARD_GRABBED)
                ImGui::BulletText ("window has grabbed keyboard input");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_VULKAN)
                ImGui::BulletText ("window usable for vulkan surface");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_METAL)
                ImGui::BulletText ("window usable for Metal view");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_TRANSPARENT)
                ImGui::BulletText ("window with transparent buffer");

              if (pEngine->debugInfo.windowCreationFlags & SDL_WINDOW_NOT_FOCUSABLE)
                ImGui::BulletText ("window should not be focusable");

              ImGui::Spacing ();
              ImGui::TreePop ();
            }

#if __unix__ || __linux__ || __FreeBSD__ || __NetBSD__ || __NetBSD__ || __OpenBSD__ || __bsdi__    \
    || __DragonFly__ || __MidnightBSD__
          ImGui::Text ("WINDOW DECORATION :");

          switch (pEngine->debugInfo.windowDecorationType)
            {
            case Patache::WindowDecorationType::eNone:
              ImGui::SameLine ();
              ImGui::Text ("NO");
              break;

            case Patache::WindowDecorationType::eClientSideDecoration:
              ImGui::SameLine ();
              ImGui::Text ("Client Side");
              break;

            case Patache::WindowDecorationType::eServerSideDecoration:
              ImGui::SameLine ();
              ImGui::Text ("Server Side");
              break;
            }

          ImGui::Spacing ();
#endif
        }

      ImGui::Text ("Start Time %.2f milliseconds", pEngine->debugInfo.startTime);

      ImGui::PopStyleVar ();
      ImGui::End ();
    }
  // End Patache Window Info

  // Raccoon Renderer Info
  if (pEngine->debugInfo.raccoonRendererInfoWindow)
    {
      ImGui::SetNextWindowSize (ImVec2 (480.0F, 400.0F), ImGuiCond_FirstUseEver);

      ImGui::Begin ("Raccoon Renderer", &pEngine->debugInfo.raccoonRendererInfoWindow);

      ImGui::PushStyleVar (ImGuiStyleVar_CellPadding, ImVec2 (Patache::sImGuiTablePadding));

      // Only vulkan for now. OpenGL as compability option maybe in the future
      ImGui::Text ("Vulkan %s", pEngine->debugInfo.versionVK);

      if (ImGui::IsItemHovered ())
        {
          ImGui::SetTooltip ("In Use %d.%d", VK_VERSION_MAJOR (VK_API_VERSION_1_2),
                             VK_VERSION_MINOR (VK_API_VERSION_1_2));
        }

      ImGui::Spacing ();

      // Device name
      if (pEngine->debugInfo.deviceNameVK[0U] != '\0')
        {
          ImGui::BeginGroup ();
          ImGui::Text ("DEVICE :");
          ImGui::SameLine ();

          switch (pEngine->debugInfo.deviceVendorIdVK)
            {
            case 32902:
              ImGui::TextColored (ImVec4 (Patache::sImGuiIntelColor), "%s",
                                  pEngine->debugInfo.deviceNameVK);
              break;

            case 4098:
              ImGui::TextColored (ImVec4 (Patache::sImGuiAMDColor), "%s",
                                  pEngine->debugInfo.deviceNameVK);
              break;

            case 4318:
              ImGui::TextColored (ImVec4 (Patache::sImGuiNvidiaColor), "%s",
                                  pEngine->debugInfo.deviceNameVK);
              break;

            case 65541:
              ImGui::TextColored (ImVec4 (Patache::sImGuiMesaColor), "%s",
                                  pEngine->debugInfo.deviceNameVK);
              break;

            default:
              ImGui::Text ("%s", pEngine->debugInfo.deviceNameVK);
              break;
            }
          ImGui::EndGroup ();
        }

      // Vendor Info tooltip
      if (ImGui::BeginItemTooltip ())
        {
          // GPU Vendor Name if is not unknown
          if (pEngine->debugInfo.deviceVendorIdVK != 0)
            {
              ImGui::Text ("VENDOR :");
              ImGui::SameLine ();

              switch (pEngine->debugInfo.deviceVendorIdVK)
                {
                case 32902:
                  ImGui::TextColored (ImVec4 (Patache::sImGuiIntelColor), "Intel");
                  break;

                case 4098:
                  ImGui::TextColored (ImVec4 (Patache::sImGuiAMDColor), "AMD");
                  break;

                case 4318:
                  ImGui::TextColored (ImVec4 (Patache::sImGuiNvidiaColor), "Nvidia");
                  break;

                case 65541:
                  ImGui::TextColored (ImVec4 (Patache::sImGuiMesaColor), "Mesa");
                  break;

                default:
                  ImGui::Text ("%d", pEngine->debugInfo.deviceVendorIdVK);
                  break;
                }

              // GPU Vendor ID
              ImGui::Text ("VENDOR ID :");
              ImGui::SameLine ();

              switch (pEngine->debugInfo.deviceVendorIdVK)
                {
                case 32902:
                  ImGui::TextColored (ImVec4 (Patache::sImGuiIntelColor), "%u 0x%X",
                                      pEngine->debugInfo.deviceVendorIdVK,
                                      pEngine->debugInfo.deviceVendorIdVK);
                  break;

                case 4098:
                  ImGui::TextColored (ImVec4 (Patache::sImGuiAMDColor), "%u 0x%X",
                                      pEngine->debugInfo.deviceVendorIdVK,
                                      pEngine->debugInfo.deviceVendorIdVK);
                  break;

                case 4318:
                  ImGui::TextColored (ImVec4 (Patache::sImGuiNvidiaColor), "%u 0x%X",
                                      pEngine->debugInfo.deviceVendorIdVK,
                                      pEngine->debugInfo.deviceVendorIdVK);
                  break;

                case 65541:
                  ImGui::TextColored (ImVec4 (Patache::sImGuiMesaColor), "%u 0x%X",
                                      pEngine->debugInfo.deviceVendorIdVK,
                                      pEngine->debugInfo.deviceVendorIdVK);
                  break;

                default:
                  ImGui::Text ("%u 0x%X", pEngine->debugInfo.deviceVendorIdVK,
                               pEngine->debugInfo.deviceVendorIdVK);
                  break;
                }
            }
          ImGui::EndTooltip ();
        }

      // Device type {Discrete?, Integrated?}
      if (pEngine->debugInfo.deviceTypeVK[0U] != '\0')
        ImGui::Text ("DEVICE TYPE : %s", pEngine->debugInfo.deviceTypeVK);

#if defined(PATACHE_USE_VVL)
      if (ImGui::CollapsingHeader ("LAYERS AND EXTENSIONS"))
#else
      if (ImGui::CollapsingHeader ("EXTENSIONS"))
#endif
        {
          // vulkan Instance Extensions list
          if (ImGui::BeginTable ("INSTANCE##VkInstanceList", 1, Patache::sImGuiTableFlags))
            {
              ImGui::TableSetupColumn ("INSTANCE", ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableHeadersRow ();

              for (std::uint32_t i{ 0U }; i < pEngine->debugInfo.instanceExtensionsCountVK; ++i)
                {
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("%s", pEngine->debugInfo.ppInstanceExtensionsVK[i]);
                }

              ImGui::EndTable ();
            }

          ImGui::SameLine ();

          // vulkan Device Extensions List
          if (ImGui::BeginTable ("DEVICE##VkDeviceList", 1, Patache::sImGuiTableFlags))
            {
              ImGui::TableSetupColumn ("DEVICE", ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableHeadersRow ();

              for (std::uint32_t i{ 0U }; i < pEngine->debugInfo.deviceExtensionsCountVK; ++i)
                {
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("%s", pEngine->debugInfo.ppDeviceExtensionsVK[i]);
                }

              ImGui::EndTable ();
            }

            // vulkan Layers List
#if defined(PATACHE_USE_VVL)
          if (ImGui::BeginTable ("LAYERS##VkLayerList", 1, Patache::sImGuiTableFlags))
            {
              ImGui::TableSetupColumn ("LAYER", ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableHeadersRow ();

              for (std::uint16_t i{ 0U }; i < PATACHE_LAYER_VK_COUNT; ++i)
                {
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("%s", pEngine->debugInfo.ppLayersVK[i]);
                }

              ImGui::EndTable ();
            }
#endif

          ImGui::Spacing ();
        }

      if (ImGui::CollapsingHeader (
              (pEngine->vulkan.physicalDeviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                  ? "DEVICE MEMORY"
                  : "HOST MEMORY"))
        {
          // Device Memory Size (VRAM size)
          ImGui::Text ((pEngine->vulkan.physicalDeviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                           ? "DEVICE VRAM : %.2f %s"
                           : "HOST VRAM : %.2f %s",
                       pEngine->debugInfo.vramSize, pEngine->debugInfo.vramSizeUnit);

          pEngine->debugInfo.memSizeBytes  = 0U;
          pEngine->debugInfo.memSizeBytes2 = 0U;
          vmaCalculateStatistics (pEngine->vulkan.allocator, &pEngine->debugInfo.memStats);
          vmaGetHeapBudgets (pEngine->vulkan.allocator, pEngine->debugInfo.memBudgetInfo);

          for (std::uint32_t i{ 0U }; i < pEngine->debugInfo.memProperties.memoryHeapCount; ++i)
            {
              if (pEngine->vulkan.physicalDeviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                {
                  if (pEngine->debugInfo.memProperties.memoryHeaps[i].flags
                      & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                    {
                      pEngine->debugInfo.memSizeBytes += pEngine->debugInfo.memBudgetInfo[i].budget;
                    }
                }
              else
                {
                  pEngine->debugInfo.memSizeBytes += pEngine->debugInfo.memBudgetInfo[i].budget;
                }
            }

          pEngine->debugInfo.memSize  = pEngine->debugInfo.memSizeBytes;
          pEngine->debugInfo.memSize2 = pEngine->debugInfo.memStats.total.statistics.blockBytes;

          Patache::CalculateSizeUnit (pEngine->debugInfo.memSize, pEngine->debugInfo.pSizeUnit,
                                      PATACHE_SIZEUNIT_STR_SIZE);
          Patache::CalculateSizeUnit (pEngine->debugInfo.memSize2, pEngine->debugInfo.pSizeUnit2,
                                      PATACHE_SIZEUNIT_STR_SIZE);

          std::snprintf (pEngine->debugInfo.memInfo, PATACHE_MEMINFO_STR_SIZE,
                         "%.02f %s / %.02f %s Budget", pEngine->debugInfo.memSize2,
                         pEngine->debugInfo.pSizeUnit2, pEngine->debugInfo.memSize,
                         pEngine->debugInfo.pSizeUnit);

          pEngine->debugInfo.barProgress
              = (float)pEngine->debugInfo.memStats.total.statistics.blockBytes
                / pEngine->debugInfo.memSizeBytes;

          ImGui::Text ((pEngine->vulkan.physicalDeviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                           ? "DEVICE MEMORY ALLOCATED"
                           : "HOST MEMORY ALLOCATED");
          ImGui::ProgressBar (pEngine->debugInfo.barProgress, ImVec2 (400.0F, 0.0F),
                              pEngine->debugInfo.memInfo);

          if (pEngine->vulkan.physicalDeviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
              pEngine->debugInfo.barProgress
                  = (float)pEngine->vulkan.renderBufferInfo[VkBufferInfo::eCurrentOffset]
                    / pEngine->debugInfo.memStats.total.statistics.allocationBytes;

              pEngine->debugInfo.memSize
                  = pEngine->vulkan.renderBufferInfo[VkBufferInfo::eCurrentOffset];
            }
          else
            {
              pEngine->debugInfo.barProgress
                  = (float)pEngine->vulkan.stagingBufferInfo[VkBufferInfo::eCurrentOffset]
                    / pEngine->debugInfo.memStats.total.statistics.allocationBytes;

              pEngine->debugInfo.memSize
                  = pEngine->vulkan.stagingBufferInfo[VkBufferInfo::eCurrentOffset];
            }

          PATACHE_STRNCPY (pEngine->debugInfo.pSizeUnit, "Bytes", PATACHE_SIZEUNIT_STR_SIZE,
                           PATACHE_SIZEUNIT_STR_SIZE);
          Patache::CalculateSizeUnit (pEngine->debugInfo.memSize, pEngine->debugInfo.pSizeUnit,
                                      PATACHE_SIZEUNIT_STR_SIZE);

          pEngine->debugInfo.memSize2
              = pEngine->debugInfo.memStats.total.statistics.allocationBytes;
          PATACHE_STRNCPY (pEngine->debugInfo.pSizeUnit2, "Bytes", PATACHE_SIZEUNIT_STR_SIZE,
                           PATACHE_SIZEUNIT_STR_SIZE);
          Patache::CalculateSizeUnit (pEngine->debugInfo.memSize2, pEngine->debugInfo.pSizeUnit2,
                                      PATACHE_SIZEUNIT_STR_SIZE);

          std::snprintf (pEngine->debugInfo.memInfo, PATACHE_MEMINFO_STR_SIZE,
                         "%.02f %s / %.02f %s", pEngine->debugInfo.memSize,
                         pEngine->debugInfo.pSizeUnit, pEngine->debugInfo.memSize2,
                         pEngine->debugInfo.pSizeUnit2);

          ImGui::Text ("MEMORY BUFFER USAGE (%d BLOCKS)",
                       pEngine->debugInfo.memStats.total.statistics.blockCount);
          ImGui::ProgressBar (pEngine->debugInfo.barProgress, ImVec2 (400.0F, 0.0F),
                              pEngine->debugInfo.memInfo);

          ImGui::Spacing ();
        }

      // Queues in use
      if (ImGui::CollapsingHeader ("DEVICE QUEUES"))
        {
          if (ImGui::BeginTable ("Queues##AboutQueues", 3, Patache::sImGuiTableFlags))
            {
              ImGui::TableSetupColumn ("INDEX", ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableSetupColumn ("FAMILY PROPERTIES", ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableSetupColumn ("PRIORITY", ImGuiTableColumnFlags_WidthFixed);

              ImGui::TableHeadersRow ();

              // Queue Index
              ImGui::TableNextRow ();
              ImGui::TableSetColumnIndex (0);
              ImGui::Text ("%u", pEngine->vulkan.graphicsQueueFamilyIndex);

              // Queue Family Properties
              ImGui::TableSetColumnIndex (1);

              if (pEngine->debugInfo.queueFlagsVK & VK_QUEUE_GRAPHICS_BIT)
                ImGui::BulletText ("VK_QUEUE_GRAPHICS_BIT");

              if (pEngine->debugInfo.queueFlagsVK & VK_QUEUE_COMPUTE_BIT)
                ImGui::BulletText ("VK_QUEUE_COMPUTE_BIT");

              if (pEngine->debugInfo.queueFlagsVK & VK_QUEUE_TRANSFER_BIT)
                ImGui::BulletText ("VK_QUEUE_TRANSFER_BIT");

              if (pEngine->debugInfo.queueFlagsVK & VK_QUEUE_SPARSE_BINDING_BIT)
                ImGui::BulletText ("VK_QUEUE_SPARSE_BINDING_BIT");

              if (pEngine->debugInfo.queueFlagsVK & VK_QUEUE_PROTECTED_BIT)
                ImGui::BulletText ("VK_QUEUE_PROTECTED_BIT");

              if (pEngine->debugInfo.queueFlagsVK & VK_QUEUE_VIDEO_DECODE_BIT_KHR)
                ImGui::BulletText ("VK_QUEUE_VIDEO_DECODE_BIT_KHR");

              if (pEngine->debugInfo.queueFlagsVK & VK_QUEUE_VIDEO_ENCODE_BIT_KHR)
                ImGui::BulletText ("VK_QUEUE_VIDEO_ENCODE_BIT_KHR");

              if (pEngine->debugInfo.queueFlagsVK & VK_QUEUE_OPTICAL_FLOW_BIT_NV)
                ImGui::BulletText ("VK_QUEUE_OPTICAL_FLOW_BIT_NV");

              // Queue Priority
              ImGui::TableSetColumnIndex (2);

              if (pEngine->debugInfo.queuePriorityVK == 1.0F)
                {
                  ImGui::Text ("%.2f (High)", pEngine->debugInfo.queuePriorityVK);
                }
              else if (pEngine->debugInfo.queuePriorityVK < 0.4F)
                {
                  ImGui::Text ("%.2f (Low)", pEngine->debugInfo.queuePriorityVK);
                }
              else
                {
                  ImGui::Text ("%.2f", pEngine->debugInfo.queuePriorityVK);
                }

              ImGui::EndTable ();
            }

          ImGui::Spacing ();
        }

      // Driver
      if (pEngine->debugInfo.driverNameVK[0U] != '\0')
        {
          ImGui::BeginGroup ();
          ImGui::Text ("DRIVER :");
          ImGui::SameLine ();

          // Driver Name
          switch (pEngine->debugInfo.deviceVendorIdVK)
            {
            case 32902:
              ImGui::TextColored (ImVec4 (Patache::sImGuiIntelColor), "%s",
                                  pEngine->debugInfo.driverNameVK);
              break;
            case 4098:
              ImGui::TextColored (ImVec4 (Patache::sImGuiAMDColor), "%s",
                                  pEngine->debugInfo.driverNameVK);
              break;
            case 4318:
              ImGui::TextColored (ImVec4 (Patache::sImGuiNvidiaColor), "%s",
                                  pEngine->debugInfo.driverNameVK);
              break;
            case 65541:
              ImGui::TextColored (ImVec4 (Patache::sImGuiMesaColor), "%s",
                                  pEngine->debugInfo.driverNameVK);
              break;

            default:
              ImGui::Text ("%s", pEngine->debugInfo.driverNameVK);
              break;
            }

          ImGui::SameLine ();

          // Driver Version
          if (pEngine->debugInfo.driverVersionVK[0U] != '\0')
            {
              ImGui::Text ("%s", pEngine->debugInfo.driverVersionVK);
            }

          ImGui::EndGroup ();

          // Driver Vendor Tooltip
          if (ImGui::BeginItemTooltip ())
            {
              if (pEngine->debugInfo.driverIdVK[0U] != '\0')
                ImGui::Text ("ID : %s", pEngine->debugInfo.driverIdVK);

              if (pEngine->debugInfo.driverInfoVK[0U] != '\0')
                ImGui::Text ("INFO : %s", pEngine->debugInfo.driverInfoVK);

              ImGui::EndTooltip ();
            }
        }

      if (ImGui::CollapsingHeader ("SWAPCHAIN"))
        {
          // Swapchain present mode
          // Is vsync?
          if ((pEngine->debugInfo.swapchainPresentModeVK == VK_PRESENT_MODE_FIFO_KHR
               || pEngine->debugInfo.swapchainPresentModeVK == VK_PRESENT_MODE_FIFO_RELAXED_KHR
               || pEngine->debugInfo.swapchainPresentModeVK
                      == VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR)
              && (pEngine->configuration.vsync))
            {
              ImGui::BeginGroup ();
              ImGui::Text ("SWAPCHAIN PRESENT MODE :");
              ImGui::SameLine ();
              ImGui::TextColored (
                  ImVec4 (Patache::sImGuiPositiveValue), "%s",
                  string_VkPresentModeKHR (pEngine->debugInfo.swapchainPresentModeVK));
              ImGui::EndGroup ();

              if (ImGui::BeginItemTooltip ())
                {
                  ImGui::TextColored (ImVec4 (Patache::sImGuiPositiveValue),
                                      "Vertical sync Active");
                  ImGui::EndTooltip ();
                }
            }
          else
            {
              ImGui::BeginGroup ();
              ImGui::Text ("SWAPCHAIN PRESENT MODE :");
              ImGui::SameLine ();
              ImGui::TextColored (
                  ImVec4 (Patache::sImGuiWarningValue), "%s",
                  string_VkPresentModeKHR (pEngine->debugInfo.swapchainPresentModeVK));
              ImGui::EndGroup ();

              if (ImGui::BeginItemTooltip ())
                {
                  ImGui::TextColored (ImVec4 (Patache::sImGuiWarningValue),
                                      "Vertical sync Inactve");
                  ImGui::EndTooltip ();
                }
            }

          // Image count
          ImGui::Text ("SWAPCHAIN IMAGE COUNT : %u", pEngine->vulkan.swapchainImageCount);

          // Image Color Format
          ImGui::Text ("COLOR FORMAT : %s",
                       string_VkFormat (pEngine->debugInfo.swapchainImageColorFormatVK));

          // Image Color Space
          ImGui::Text ("COLOR SPACE : %s",
                       string_VkColorSpaceKHR (pEngine->debugInfo.swapchainImageColorSpaceVK));

          // Drawable Size
          ImGui::Text ("SWAPCHAIN DRAWABLE SIZE : %u x %u", pEngine->vulkan.swapchainExtent.width,
                       pEngine->vulkan.swapchainExtent.height);

          ImGui::Spacing ();
        }

      if (ImGui::CollapsingHeader ("PERFORMANCE"))
        {
          pEngine->debugInfo.calculatePerformanceStats = true;

          if (std::chrono::duration<float, std::chrono::seconds::period> (
                  std::chrono::high_resolution_clock::now () - pEngine->debugInfo.frameTimer)
              >= std::chrono::duration<float, std::chrono::seconds::period> (
                  std::chrono::milliseconds (500)))
            {
              pEngine->debugInfo.fps
                  = pEngine->debugInfo.numFrames
                    / std::chrono::duration<float, std::chrono::seconds::period> (
                          std::chrono::high_resolution_clock::now ()
                          - pEngine->debugInfo.frameTimer)
                          .count ();

              pEngine->debugInfo.fpsHistory[pEngine->debugInfo.fpsHistoryIterator]
                  = pEngine->debugInfo.fps;

              pEngine->debugInfo.fpsHistoryIterator
                  = (pEngine->debugInfo.fpsHistoryIterator + 1) % PATACHE_FPSHISTORY_COUNT;

              pEngine->debugInfo.frameTime
                  = std::chrono::duration<float, std::chrono::milliseconds::period> (
                        std::chrono::high_resolution_clock::now () - pEngine->debugInfo.frameTimer)
                        .count ()
                    / pEngine->debugInfo.numFrames;

              /*pEngine->debugInfo.frameTimeHistory[pEngine->debugInfo.frameTimeHistoryIterator]
                  = pEngine->debugInfo.frameTime;*/

              /*pEngine->debugInfo.frameTimeHistoryIterator
                  = (pEngine->debugInfo.frameTimeHistoryIterator + 1) % PATACHE_FPSHISTORY_COUNT;*/

              pEngine->debugInfo.numFrames   = 0;
              pEngine->debugInfo.timerFpsRun = true;
            }

          ImGui::Text ("Frames Per Second : %.02f", pEngine->debugInfo.fps);
          /*ImGui::PlotHistogram ("fpsHistory", pEngine->debugInfo.fpsHistory,
                                PATACHE_FPSHISTORY_COUNT, PATACHE_FPSHISTORY_COUNT, nullptr, 0.0F,
                                FLT_MAX, ImVec2 (0.0F, 80.0F));*/

          ImGui::Text ("Frame Time : %.02f Milliseconds", pEngine->debugInfo.frameTime);
          /*ImGui::PlotHistogram ("frametimeHistory", pEngine->debugInfo.frameTimeHistory,
                                PATACHE_FPSHISTORY_COUNT, PATACHE_FPSHISTORY_COUNT, nullptr, 0.0F,
                                FLT_MAX, ImVec2 (0.0F, 80.0F));*/

          ImGui::Text ("Main Command Buffer Time : %.02f Milliseconds",
                       pEngine->debugInfo.mainCmdTime);

          ImGui::Text ("Main Render Pass Time : %.02f Milliseconds",
                       pEngine->debugInfo.mainCmdTime);

          ImGui::Text ("Swapchain Resize Time : %.02f Milliseconds",
                       pEngine->debugInfo.mainSwapchainTime);

          ImGui::Spacing ();
        }
      else
        {
          pEngine->debugInfo.calculatePerformanceStats = false;
        }

      ImGui::PopStyleVar ();
      ImGui::End ();
    }
  // End Raccoon Renderer Info

  // Patache Window configuration
  if (pEngine->debugInfo.configWindow)
    {
      ImGui::Begin ("Configuration##ConfigView", &pEngine->debugInfo.configWindow);
      ImGui::TextColored (ImVec4 (Patache::sImGuiWarningValue), "Danger Zone");

      // Show Fatal Error MessageBox
      if (ImGui::Checkbox ("Show Fatal Error "
                           "Messagebox##ConfigView_ShowFatalErrorMessagebox",
                           &pEngine->configuration.showFatalErrorMessagebox))
        {
          pEngine->configuration.triggeredChange
              |= Patache::ChangeConfigFlagBits::eShowFatalErrorMessagebox;
        }

      // Vsync
      if (ImGui::Checkbox ("Vsync##ConfigView_Vsync", &pEngine->configuration.vsync))
        {
          pEngine->configuration.triggeredChange |= Patache::ChangeConfigFlagBits::eVsync;
        }

      ImGui::SameLine ();

      // 10 Bit Depth
      if (ImGui::Checkbox ("10 Bit Depth##ConfigView_BitDepth", &pEngine->configuration.bitDepth10))
        {
          pEngine->configuration.triggeredChange |= Patache::ChangeConfigFlagBits::eBitDepth10;
        }

      ImGui::Spacing ();

      // Add Image Count
      int actualCount = pEngine->vulkan.swapchainImageCount;

      ImGui::Text ("Add Image Count");
      if (ImGui::SliderInt (
              "###ConfigView_AddImageCount", &actualCount, pEngine->debugInfo.minImageCountVK,
              static_cast<int> (pEngine->debugInfo.minImageCountVK + 4U), "%d Images"))
        {
          pEngine->configuration.triggeredChange |= Patache::ChangeConfigFlagBits::eAddImageCount;
        }

      ImGui::End ();
    }
  // End Patache Window configuration

  // Reset style outside of Engine
  ImGui::GetStyle ().Colors[ImGuiCol_TitleBgActive]     = pEngine->debugInfo.engineStyles[0U];
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGrip]        = pEngine->debugInfo.engineStyles[1U];
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripActive]  = pEngine->debugInfo.engineStyles[2U];
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripHovered] = pEngine->debugInfo.engineStyles[3U];
  ImGui::GetStyle ().Colors[ImGuiCol_SeparatorHovered]  = pEngine->debugInfo.engineStyles[4U];
  ImGui::GetStyle ().Colors[ImGuiCol_SeparatorActive]   = pEngine->debugInfo.engineStyles[5U];
  ImGui::GetStyle ().Colors[ImGuiCol_Tab]               = pEngine->debugInfo.engineStyles[6U];
  ImGui::GetStyle ().Colors[ImGuiCol_TabSelected]       = pEngine->debugInfo.engineStyles[7U];
  ImGui::GetStyle ().Colors[ImGuiCol_TabHovered]        = pEngine->debugInfo.engineStyles[8U];
  ImGui::GetStyle ().Colors[ImGuiCol_Header]            = pEngine->debugInfo.engineStyles[9U];
  ImGui::GetStyle ().Colors[ImGuiCol_HeaderActive]      = pEngine->debugInfo.engineStyles[10U];
  ImGui::GetStyle ().Colors[ImGuiCol_HeaderHovered]     = pEngine->debugInfo.engineStyles[11U];
  ImGui::GetStyle ().Colors[ImGuiCol_Button]            = pEngine->debugInfo.engineStyles[12U];
  ImGui::GetStyle ().Colors[ImGuiCol_ButtonHovered]     = pEngine->debugInfo.engineStyles[13U];
  ImGui::GetStyle ().Colors[ImGuiCol_ButtonActive]      = pEngine->debugInfo.engineStyles[14U];
  ImGui::GetStyle ().Colors[ImGuiCol_FrameBg]           = pEngine->debugInfo.engineStyles[15U];
  ImGui::GetStyle ().Colors[ImGuiCol_FrameBgHovered]    = pEngine->debugInfo.engineStyles[16U];
  ImGui::GetStyle ().Colors[ImGuiCol_FrameBgActive]     = pEngine->debugInfo.engineStyles[17U];
  ImGui::GetStyle ().Colors[ImGuiCol_CheckMark]         = pEngine->debugInfo.engineStyles[18U];
  ImGui::GetStyle ().Colors[ImGuiCol_SliderGrab]        = pEngine->debugInfo.engineStyles[19U];
  ImGui::GetStyle ().Colors[ImGuiCol_SliderGrabActive]  = pEngine->debugInfo.engineStyles[20U];
}
