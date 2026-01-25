#include "DebugUI.hpp"

void
Patache::DrawDebugUI (Patache::Engine * const pEngine)
{
  // Patache Style
  ImGui::GetStyle ().Colors[ImGuiCol_TitleBgActive] = ImVec4 (PATACHE_IMGUI_PATACHE_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGrip]        = ImVec4 (PATACHE_IMGUI_PATACHE_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripActive]  = ImVec4 (PATACHE_IMGUI_PATACHE_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripHovered] = ImVec4 (PATACHE_IMGUI_SELECTED_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_SeparatorHovered] = ImVec4 (PATACHE_IMGUI_CORNERBORDER_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_SeparatorActive]  = ImVec4 (PATACHE_IMGUI_CORNERBORDER_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_Tab]         = ImVec4 (PATACHE_IMGUI_DARKER_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_TabSelected] = ImVec4 (PATACHE_IMGUI_PATACHE_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_TabHovered]  = ImVec4 (PATACHE_IMGUI_SELECTED_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_Button]        = ImVec4 (PATACHE_IMGUI_PATACHE_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_ButtonHovered] = ImVec4 (PATACHE_IMGUI_SELECTED_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_ButtonActive]  = ImVec4 (PATACHE_IMGUI_DARKER_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_Header]        = ImVec4 (PATACHE_IMGUI_INFO_DARKER_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_HeaderActive]  = ImVec4 (PATACHE_IMGUI_INFO_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_HeaderHovered] = ImVec4 (PATACHE_IMGUI_INFO_SELECTED_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_FrameBg]        = ImVec4 (PATACHE_IMGUI_FRAMEBG_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_FrameBgHovered] = ImVec4 (PATACHE_IMGUI_FRAMEBG_HOVERED_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_FrameBgActive]  = ImVec4 (PATACHE_IMGUI_FRAMEBG_ACTIVE_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_CheckMark] = ImVec4 (PATACHE_IMGUI_CHECKMARK_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_SliderGrab] = ImVec4 (PATACHE_IMGUI_SLIDERGRAB_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_SliderGrabActive]
      = ImVec4 (PATACHE_IMGUI_SLIDERGRABACTIVE_COLOR);
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

      ImGui::GetStyle ().Colors[ImGuiCol_Button] = ImVec4 (PATACHE_IMGUI_PATACHE_COLOR);
    }
  // End Menu Bar

  // Patache Window Info
  if (pEngine->debugInfo.infoWindow)
    {
      ImGui::SetNextWindowSize (ImVec2 (400.0F, 440.0F), ImGuiCond_FirstUseEver);

      ImGui::Begin (PATACHE_ENGINE_NAME, &pEngine->debugInfo.infoWindow);

      ImGui::PushStyleVar (ImGuiStyleVar_CellPadding, ImVec2 (PATACHE_IMGUI_TABLE_PADDING));

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
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_WARNING_VALUE), "Dirty");
            }
          else
            {
              ImGui::SameLine ();
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_POSITIVE_VALUE), "Clean");
            }

          if constexpr (PATACHE_GIT_STAGE)
            {
              ImGui::SameLine ();
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_POSITIVE_VALUE), "In stage area");
            }
          else
            {
              ImGui::SameLine ();
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_WARNING_VALUE), "Un-staged changes");
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

      ImGui::PopStyleVar ();
      ImGui::End ();
    }
  // End Patache Window Info

  // Raccoon Renderer Info
  if (pEngine->debugInfo.raccoonRendererInfoWindow)
    {
      ImGui::SetNextWindowSize (ImVec2 (480.0F, 400.0F), ImGuiCond_FirstUseEver);

      ImGui::Begin ("Raccoon Renderer", &pEngine->debugInfo.raccoonRendererInfoWindow);

      ImGui::PushStyleVar (ImGuiStyleVar_CellPadding, ImVec2 (PATACHE_IMGUI_TABLE_PADDING));

      // Only vulkan for now. OpenGL as compability option maybe in the future
      ImGui::Text ("API : vulkan %s", pEngine->debugInfo.versionVK);

      if (ImGui::IsItemHovered ())
        {
          ImGui::SetTooltip ("In Use %d.%d", VK_VERSION_MAJOR (VK_API_VERSION_1_2),
                             VK_VERSION_MINOR (VK_API_VERSION_1_2));
        }

      if (ImGui::CollapsingHeader ("LAYERS AND EXTENSIONS"))
        {
          // vulkan Instance Extensions list
          if (ImGui::BeginTable ("INSTANCE##VkInstanceList", 1, PATACHE_IMGUI_TABLE_FLAGS))
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
          if (ImGui::BeginTable ("DEVICE##VkDeviceList", 1, PATACHE_IMGUI_TABLE_FLAGS))
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
          if (ImGui::BeginTable ("LAYERS##VkLayerList", 1, PATACHE_IMGUI_TABLE_FLAGS))
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
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_INTEL_COLOR), "%s",
                                  pEngine->debugInfo.deviceNameVK);
              break;

            case 4098:
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_AMD_COLOR), "%s",
                                  pEngine->debugInfo.deviceNameVK);
              break;

            case 4318:
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_NVIDIA_COLOR), "%s",
                                  pEngine->debugInfo.deviceNameVK);
              break;

            case 65541:
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_MESA_COLOR), "%s",
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
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_INTEL_COLOR), "Intel");
                  break;

                case 4098:
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_AMD_COLOR), "AMD");
                  break;

                case 4318:
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_NVIDIA_COLOR), "Nvidia");
                  break;

                case 65541:
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_MESA_COLOR), "Mesa");
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
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_INTEL_COLOR), "%u 0x%X",
                                      pEngine->debugInfo.deviceVendorIdVK,
                                      pEngine->debugInfo.deviceVendorIdVK);
                  break;

                case 4098:
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_AMD_COLOR), "%u 0x%X",
                                      pEngine->debugInfo.deviceVendorIdVK,
                                      pEngine->debugInfo.deviceVendorIdVK);
                  break;

                case 4318:
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_NVIDIA_COLOR), "%u 0x%X",
                                      pEngine->debugInfo.deviceVendorIdVK,
                                      pEngine->debugInfo.deviceVendorIdVK);
                  break;

                case 65541:
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_MESA_COLOR), "%u 0x%X",
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

      // Device Memory Size (VRAM size)
      ImGui::Text ("DEVICE VRAM : %.2f %s", pEngine->debugInfo.vramSize,
                   pEngine->debugInfo.vramSizeUnit);

      ImGui::Spacing ();

      if (ImGui::CollapsingHeader ("DEVICE MEMORY"))
        {
          if (ImGui::BeginTable ("DeviceMemory##AboutDeviceMemory", 3, PATACHE_IMGUI_TABLE_FLAGS))
            {
              ImGui::TableSetupColumn ("TYPE PROPERTY", ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableSetupColumn ("HEAP", ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableSetupColumn ("SIZE", ImGuiTableColumnFlags_WidthFixed);

              ImGui::TableHeadersRow ();

              for (std::uint8_t i{ 0U }; i < pEngine->vulkan.swapchainImageCount; ++i)
                {
                  ImGui::TableNextRow ();

                  if (pEngine->debugInfo.physicalDeviceProperties.properties.deviceType
                      == vk::PhysicalDeviceType::eDiscreteGpu)
                    {
                      ImGui::TableSetColumnIndex (0);
                      ImGui::Text ("%s", pEngine->debugInfo.ppVramMemoryDeviceType[i]);

                      ImGui::TableSetColumnIndex (1);
                      ImGui::Text ("%s", pEngine->debugInfo.ppVramMemoryDeviceHeap[i]);

                      ImGui::TableSetColumnIndex (2);
                      ImGui::Text ("%.2f %s", *pEngine->debugInfo.ppVramMemoryDeviceSize[i],
                                   pEngine->debugInfo.ppVramMemoryDeviceSizeUnit[i]);
                    }
                }

              // Host memory
              ImGui::TableNextRow ();

              ImGui::TableSetColumnIndex (0);
              ImGui::Text ("%s", pEngine->debugInfo.vramMemoryHostType);

              ImGui::TableSetColumnIndex (1);
              ImGui::Text ("%s", pEngine->debugInfo.vramMemoryHostHeap);

              ImGui::TableSetColumnIndex (2);
              ImGui::Text ("%.2f %s", pEngine->debugInfo.vramMemoryHostSize,
                           pEngine->debugInfo.pVramMemoryHostSizeUnit);

              ImGui::EndTable ();
            }

          ImGui::Text (
              "vma Pool Size Per Block : %.4f %s * %d", pEngine->debugInfo.vramPoolSizePerBlock,
              pEngine->debugInfo.pVramPoolSizePerBlockUnit, pEngine->vulkan.swapchainImageCount);

          ImGui::Text ("vma Pool Size of all blocks : %.4f %s",
                       pEngine->debugInfo.vramPoolSizeAllBlocks,
                       pEngine->debugInfo.pVramPoolSizeAllBlocksUnit);

          ImGui::Spacing ();
        }

      // Queues in use
      if (ImGui::CollapsingHeader ("DEVICE QUEUES"))
        {
          if (ImGui::BeginTable ("Queues##AboutQueues", 3, PATACHE_IMGUI_TABLE_FLAGS))
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

              if (pEngine->debugInfo.queueFlagsVK & vk::QueueFlagBits::eGraphics)
                ImGui::BulletText ("Graphics");

              if (pEngine->debugInfo.queueFlagsVK & vk::QueueFlagBits::eCompute)
                ImGui::BulletText ("Compute");

              if (pEngine->debugInfo.queueFlagsVK & vk::QueueFlagBits::eTransfer)
                ImGui::BulletText ("Transfer");

              if (pEngine->debugInfo.queueFlagsVK & vk::QueueFlagBits::eSparseBinding)
                ImGui::BulletText ("Sparse Binding");

              if (pEngine->debugInfo.queueFlagsVK & vk::QueueFlagBits::eProtected)
                ImGui::BulletText ("Protected");

              if (pEngine->debugInfo.queueFlagsVK & vk::QueueFlagBits::eVideoDecodeKHR)
                ImGui::BulletText ("Video Decode KHR");

              if (pEngine->debugInfo.queueFlagsVK & vk::QueueFlagBits::eVideoEncodeKHR)
                ImGui::BulletText ("Video Encode KHR");

              if (pEngine->debugInfo.queueFlagsVK & vk::QueueFlagBits::eOpticalFlowNV)
                ImGui::BulletText ("Nvidia Optical Flow");

              // Queue Priority
              ImGui::TableSetColumnIndex (2);

              if (pEngine->debugInfo.queuePriorityVK == 1.0F)
                ImGui::Text ("%.2f (High)", pEngine->debugInfo.queuePriorityVK);
              else if (pEngine->debugInfo.queuePriorityVK < 0.4F)
                ImGui::Text ("%.2f (Low)", pEngine->debugInfo.queuePriorityVK);
              else
                ImGui::Text ("%.2f", pEngine->debugInfo.queuePriorityVK);

              ImGui::EndTable ();
            }
        }

      ImGui::Spacing ();

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
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_INTEL_COLOR), "%s",
                                  pEngine->debugInfo.driverNameVK);
              break;
            case 4098:
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_AMD_COLOR), "%s",
                                  pEngine->debugInfo.driverNameVK);
              break;
            case 4318:
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_NVIDIA_COLOR), "%s",
                                  pEngine->debugInfo.driverNameVK);
              break;
            case 65541:
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_MESA_COLOR), "%s",
                                  pEngine->debugInfo.driverNameVK);
              break;

            default:
              ImGui::Text ("%s", pEngine->debugInfo.driverNameVK);
              break;
            }

          ImGui::SameLine ();

          // Driver Version
          if (pEngine->debugInfo.driverVersionVK[0U] != '\0')
            ImGui::Text ("%s", pEngine->debugInfo.driverVersionVK);

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

          ImGui::Spacing ();
        }

      // Swapchain present mode
      ImGui::Text ("SWAPCHAIN PRESENT MODE : %s",
                   vk::to_string (pEngine->debugInfo.swapchainPresentModeVK).c_str ());

      // Is vsync?
      if ((pEngine->debugInfo.swapchainPresentModeVK == vk::PresentModeKHR::eFifo
           || pEngine->debugInfo.swapchainPresentModeVK == vk::PresentModeKHR::eFifoRelaxed
           || pEngine->debugInfo.swapchainPresentModeVK
                  == vk::PresentModeKHR::eSharedContinuousRefresh)
          && (pEngine->configuration.vsync))
        {
          ImGui::SameLine ();
          ImGui::TextColored (ImVec4 (PATACHE_IMGUI_POSITIVE_VALUE), "Vertical sync");
        }

      // Image count
      ImGui::Text ("SWAPCHAIN IMAGE COUNT : %u", pEngine->vulkan.swapchainImageCount);

      // Image Color Format
      ImGui::Text ("COLOR FORMAT : %s",
                   vk::to_string (pEngine->debugInfo.swapchainImageColorFormatVK).c_str ());

      // Image Color Space
      ImGui::Text ("COLOR SPACE : %s",
                   vk::to_string (pEngine->debugInfo.swapchainImageColorSpaceVK).c_str ());

      // Drawable Size
      ImGui::Text ("SWAPCHAIN DRAWABLE SIZE : %u x %u", pEngine->vulkan.swapchainExtent.width,
                   pEngine->vulkan.swapchainExtent.height);

      ImGui::Spacing ();

      ImGui::PopStyleVar ();
      ImGui::End ();
    }
  // End Raccoon Renderer Info

  // Patache Window configuration
  if (pEngine->debugInfo.configWindow)
    {
      ImGui::Begin ("Configuration##ConfigView", &pEngine->debugInfo.configWindow);
      ImGui::TextColored (ImVec4 (PATACHE_IMGUI_WARNING_VALUE), "Danger Zone");

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
