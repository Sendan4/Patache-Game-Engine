#include "DebugUI.hpp"

void
Patache::DrawDebugUI (Patache::Engine * const engine)
{
  // Patache Style
  ImGui::GetStyle ().Colors[ImGuiCol_TitleBgActive]
      = ImVec4 (PATACHE_IMGUI_PATACHE_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGrip]
      = ImVec4 (PATACHE_IMGUI_PATACHE_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripActive]
      = ImVec4 (PATACHE_IMGUI_PATACHE_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripHovered]
      = ImVec4 (PATACHE_IMGUI_SELECTED_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_SeparatorHovered]
      = ImVec4 (PATACHE_IMGUI_CORNERBORDER_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_SeparatorActive]
      = ImVec4 (PATACHE_IMGUI_CORNERBORDER_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_Tab]
      = ImVec4 (PATACHE_IMGUI_DARKER_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_TabSelected]
      = ImVec4 (PATACHE_IMGUI_PATACHE_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_TabHovered]
      = ImVec4 (PATACHE_IMGUI_SELECTED_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_Button]
      = ImVec4 (PATACHE_IMGUI_PATACHE_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_ButtonHovered]
      = ImVec4 (PATACHE_IMGUI_SELECTED_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_ButtonActive]
      = ImVec4 (PATACHE_IMGUI_DARKER_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_Header]
      = ImVec4 (PATACHE_IMGUI_INFO_DARKER_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_HeaderActive]
      = ImVec4 (PATACHE_IMGUI_INFO_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_HeaderHovered]
      = ImVec4 (PATACHE_IMGUI_INFO_SELECTED_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_FrameBg]
      = ImVec4 (PATACHE_IMGUI_FRAMEBG_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_FrameBgHovered]
      = ImVec4 (PATACHE_IMGUI_FRAMEBG_HOVERED_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_FrameBgActive]
      = ImVec4 (PATACHE_IMGUI_FRAMEBG_ACTIVE_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_CheckMark]
      = ImVec4 (PATACHE_IMGUI_CHECKMARK_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_SliderGrab]
      = ImVec4 (PATACHE_IMGUI_SLIDERGRAB_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_SliderGrabActive]
      = ImVec4 (PATACHE_IMGUI_SLIDERGRABACTIVE_COLOR);
  // End Patache Style

  // Menu Bar
  if (engine->engineInfo.ShowMainMenuBar)
    {
      ImGui::GetStyle ().Colors[ImGuiCol_Button]
          = ImVec4 (0.0f, 0.0f, 0.0f, 0.0f);

      if (ImGui::BeginMainMenuBar ())
        {
          if (ImGui::Button ("(Patache Icon)"))
            engine->engineInfo.ShowMainMenuBar ^= 0b00000001; // Cambiar bits

          if (ImGui::BeginMenu ("Raccoon"))
            {
              if (ImGui::MenuItem ("Renderer", ""))
                engine->engineInfo.PatacheRaccoonRendererInfoWindow
                    ^= 0b00000001; // Cambiar bits

              // No implementado aun
              // ImGui::MenuItem ("Profiler", "");
              // ImGui::MenuItem ("Audio", "");

              ImGui::EndMenu ();
            }

          if (ImGui::Button ("Configuration"))
            engine->engineInfo.PatacheConfigWindow
                ^= 0b00000001; // Cambiar bits

          if (ImGui::Button ("About"))
            engine->engineInfo.PatacheInfoWindow ^= 0b00000001; // Cambiar bits

          ImGui::EndMainMenuBar ();
        }

      ImGui::GetStyle ().Colors[ImGuiCol_Button]
          = ImVec4 (PATACHE_IMGUI_PATACHE_COLOR);
    }
  // End Menu Bar

  // Patache Window Info
  if (engine->engineInfo.PatacheInfoWindow)
    {
      ImGui::SetNextWindowSize (ImVec2 (400, 440), ImGuiCond_FirstUseEver);

      ImGui::Begin (PATACHE_ENGINE_NAME,
                    &engine->engineInfo.PatacheInfoWindow);

      ImGui::PushStyleVar (ImGuiStyleVar_CellPadding,
                           ImVec2 (PATACHE_IMGUI_TABLE_PADDING));

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

      // Text about the engine
      ImGui::Text ("Simple and free Game engine");
      ImGui::Text ("%s is under the MIT License.", PATACHE_ENGINE_NAME);
      ImGui::Spacing ();

      // below is the advanced Info
      // Git
#if defined(PATACHE_GIT_BRANCH) && defined(PATACHE_GIT_HASH_SHORT)
      if (ImGui::CollapsingHeader ("GIT"))
        {
          ImGui::Text ("BRANCH : %s", PATACHE_GIT_BRANCH);
          ImGui::Text ("COMMIT HASH : %s", PATACHE_GIT_HASH_SHORT);

#if defined(PATACHE_GIT_HASH_LONG)
          ImGui::SetItemTooltip ("LONG HASH : %s", PATACHE_GIT_HASH_LONG);
#endif

#if defined(PATACHE_GIT_WORK_DIR_IS_CLEAN)                                    \
    && defined(PATACHE_GIT_WORK_DIR_IS_CLEAN_BOOL)                            \
    && defined(PATACHE_GIT_WORK_DIR_IS_STAGED_BOOL)
          ImGui::Text ("SOURCE STATE :");

          if constexpr (PATACHE_GIT_WORK_DIR_IS_CLEAN_BOOL)
            {
              ImGui::SameLine ();
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_POSITIVE_VALUE),
                                  "Clean");
            }
          else
            {
              ImGui::SameLine ();
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_WARNING_VALUE),
                                  "Dirty");
            }

          if constexpr (PATACHE_GIT_WORK_DIR_IS_STAGED_BOOL)
            {
              ImGui::SameLine ();
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_POSITIVE_VALUE),
                                  "In Stage Area");
            }
          else
            {
              ImGui::SameLine ();
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_WARNING_VALUE),
                                  "Not Staged");
            }
#endif
          ImGui::Spacing ();
        }
#endif

        // Build Info
#if defined(PATACHE_COMPILER) && defined(PATACHE_COMPILER_VERSION)            \
    && defined(PATACHE_BUILD_SYSTEM) && defined(PATACHE_BUILD_SYSTEM_VERSION) \
    && defined(PATACHE_BUILD_SYSTEM_GENERATOR)                                \
    && defined(PATACHE_BUILD_SYSTEM_GENERATOR_VERSION)
      if (ImGui::CollapsingHeader ("BUILD INFO"))
        {
          ImGui::Text ("COMPILER : %s %s", PATACHE_COMPILER,
                       PATACHE_COMPILER_VERSION);

#if defined(PATACHE_COMPILER_PROGRAM)
          ImGui::SetItemTooltip ("PROGRAM : %s", PATACHE_COMPILER_PROGRAM);
#endif

          if (ImGui::TreeNode ("###BUILD_SYSTEM", "BUILD SYSTEM : %s %s",
                               PATACHE_BUILD_SYSTEM,
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
          ImGui::Text ("LIBC LINKED VERSION : %u.%u", __GLIBC__,
                       __GLIBC_MINOR__);
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
              if (engine->engineInfo.WindowCreationFlags
                  & SDL_WINDOW_FULLSCREEN)
                ImGui::BulletText ("fullscreen window");

              if (engine->engineInfo.WindowCreationFlags & SDL_WINDOW_OPENGL)
                ImGui::BulletText ("window usable with OpenGL context");

              if (engine->engineInfo.WindowCreationFlags & SDL_WINDOW_OCCLUDED)
                ImGui::BulletText ("window is occluded");

              if (engine->engineInfo.WindowCreationFlags & SDL_WINDOW_HIDDEN)
                ImGui::BulletText ("window is not visible");

              if (engine->engineInfo.WindowCreationFlags
                  & SDL_WINDOW_BORDERLESS)
                ImGui::BulletText ("no window decoration");

              if (engine->engineInfo.WindowCreationFlags
                  & SDL_WINDOW_RESIZABLE)
                ImGui::BulletText ("window can be resized");

              if (engine->engineInfo.WindowCreationFlags
                  & SDL_WINDOW_MINIMIZED)
                ImGui::BulletText ("window is minimized");

              if (engine->engineInfo.WindowCreationFlags
                  & SDL_WINDOW_MAXIMIZED)
                ImGui::BulletText ("window is maximized");

              if (engine->engineInfo.WindowCreationFlags
                  & SDL_WINDOW_MOUSE_GRABBED)
                ImGui::BulletText ("window has grabbed mouse input");

              if (engine->engineInfo.WindowCreationFlags
                  & SDL_WINDOW_INPUT_FOCUS)
                ImGui::BulletText ("window has input focus");

              if (engine->engineInfo.WindowCreationFlags
                  & SDL_WINDOW_MOUSE_FOCUS)
                ImGui::BulletText ("window has mouse focus");

              if (engine->engineInfo.WindowCreationFlags & SDL_WINDOW_EXTERNAL)
                ImGui::BulletText ("window not created by SDL");

              if (engine->engineInfo.WindowCreationFlags & SDL_WINDOW_MODAL)
                ImGui::BulletText ("window is modal");

              if (engine->engineInfo.WindowCreationFlags
                  & SDL_WINDOW_HIGH_PIXEL_DENSITY)
                ImGui::BulletText ("window uses high pixel density back "
                                   "buffer if possible");

              if (engine->engineInfo.WindowCreationFlags
                  & SDL_WINDOW_MOUSE_CAPTURE)
                ImGui::BulletText ("window has mouse captured "
                                   "(unrelated to MOUSE_GRABBED)");

              if (engine->engineInfo.WindowCreationFlags
                  & SDL_WINDOW_MOUSE_RELATIVE_MODE)
                ImGui::BulletText ("window has relative mode enabled");

              if (engine->engineInfo.WindowCreationFlags
                  & SDL_WINDOW_ALWAYS_ON_TOP)
                ImGui::BulletText ("window should always be above others");

              if (engine->engineInfo.WindowCreationFlags & SDL_WINDOW_UTILITY)
                ImGui::BulletText (
                    "window should be treated as a utility window");

              if (engine->engineInfo.WindowCreationFlags & SDL_WINDOW_TOOLTIP)
                ImGui::BulletText ("window should be treated as a tooltip");

              if (engine->engineInfo.WindowCreationFlags
                  & SDL_WINDOW_POPUP_MENU)
                ImGui::BulletText ("window should be treated as a popup menu");

              if (engine->engineInfo.WindowCreationFlags
                  & SDL_WINDOW_KEYBOARD_GRABBED)
                ImGui::BulletText ("window has grabbed keyboard input");

              if (engine->engineInfo.WindowCreationFlags & SDL_WINDOW_VULKAN)
                ImGui::BulletText ("window usable for Vulkan surface");

              if (engine->engineInfo.WindowCreationFlags & SDL_WINDOW_METAL)
                ImGui::BulletText ("window usable for Metal view");

              if (engine->engineInfo.WindowCreationFlags
                  & SDL_WINDOW_TRANSPARENT)
                ImGui::BulletText ("window with transparent buffer");

              if (engine->engineInfo.WindowCreationFlags
                  & SDL_WINDOW_NOT_FOCUSABLE)
                ImGui::BulletText ("window should not be focusable");

              ImGui::Spacing ();
              ImGui::TreePop ();
            }

#if defined(__linux__)
          ImGui::Text ("WINDOW DECORATION :");

          switch (engine->engineInfo.windowDecorationType)
            {
            case Patache::WindowDecorationType::None:
              ImGui::SameLine ();
              ImGui::Text ("NO");
              break;

            case Patache::WindowDecorationType::ClientSideDecoration:
              ImGui::SameLine ();
              ImGui::Text ("Client Side");
              break;

            case Patache::WindowDecorationType::ServerSideDecoration:
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
  if (engine->engineInfo.PatacheRaccoonRendererInfoWindow)
    {
      ImGui::SetNextWindowSize (ImVec2 (480, 400), ImGuiCond_FirstUseEver);

      ImGui::Begin ("Raccoon Renderer",
                    &engine->engineInfo.PatacheRaccoonRendererInfoWindow);

      ImGui::PushStyleVar (ImGuiStyleVar_CellPadding,
                           ImVec2 (PATACHE_IMGUI_TABLE_PADDING));

      // Only Vulkan for now. OpenGL as compability option maybe in the future
      ImGui::Text ("API : Vulkan");

      if (engine->engineInfo.VkVersion[0] != '\0')
        {
          ImGui::SameLine ();
          ImGui::Text ("%s", engine->engineInfo.VkVersion);
        }

      ImGui::Spacing ();

      // Lists of Vulkan layers and extensions
      if (ImGui::CollapsingHeader ("LAYERS AND EXTENSIONS"))
        {
          // Vulkan Instance Extensions list
          if (ImGui::BeginTable ("INSTANCE##VkInstanceList", 1,
                                 PATACHE_IMGUI_TABLE_FLAGS))
            {
              ImGui::TableSetupColumn ("INSTANCE",
                                       ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableHeadersRow ();

              for (std::uint16_t i = 0;
                   i < engine->engineInfo.VkInstanceExtensionsCount; ++i)
                {
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("%s",
                               engine->engineInfo.ppVkInstanceExtensions[i]);
                }
              ImGui::EndTable ();
            }

          ImGui::SameLine ();

          // Vulkan Device Extensions List
          if (ImGui::BeginTable ("DEVICE##VkDeviceList", 1,
                                 PATACHE_IMGUI_TABLE_FLAGS))
            {
              ImGui::TableSetupColumn ("DEVICE",
                                       ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableHeadersRow ();

              for (std::uint32_t i = 0;
                   i < engine->engineInfo.VkDeviceExtensionsCount; ++i)
                {
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("%s",
                               engine->engineInfo.ppVkDeviceExtensions[i]);
                }
              ImGui::EndTable ();
            }

            // Vulkan Layers List
#if defined(PATACHE_USE_VVL)
          if (ImGui::BeginTable ("LAYERS##VkLayerList", 1,
                                 PATACHE_IMGUI_TABLE_FLAGS))
            {
              ImGui::TableSetupColumn ("LAYER",
                                       ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableHeadersRow ();

              for (std::uint16_t i = 0; i < PATACHE_VK_LAYER_COUNT; ++i)
                {
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("%s", engine->engineInfo.ppVkLayers[i]);
                }

              ImGui::EndTable ();
            }
#endif
        }

      ImGui::Spacing ();

      // Device name
      if (engine->engineInfo.VkDeviceName[0] != '\0')
        {
          ImGui::BeginGroup ();
          ImGui::Text ("DEVICE :");
          ImGui::SameLine ();

          switch (engine->engineInfo.VkDeviceVendorId)
            {
            case 32902:
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_INTEL_COLOR), "%s",
                                  engine->engineInfo.VkDeviceName);
              break;

            case 4098:
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_AMD_COLOR), "%s",
                                  engine->engineInfo.VkDeviceName);
              break;

            case 4318:
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_NVIDIA_COLOR), "%s",
                                  engine->engineInfo.VkDeviceName);
              break;

            case 65541:
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_MESA_COLOR), "%s",
                                  engine->engineInfo.VkDeviceName);
              break;

            default:
              ImGui::Text ("%s", engine->engineInfo.VkDeviceName);
              break;
            }
          ImGui::EndGroup ();
        }

      // Vendor Info tooltip
      if (ImGui::BeginItemTooltip ())
        {
          // GPU Vendor Name if is not unknown
          if (engine->engineInfo.VkDeviceVendorId != 0)
            {
              ImGui::Text ("VENDOR :");
              ImGui::SameLine ();

              switch (engine->engineInfo.VkDeviceVendorId)
                {
                case 32902:
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_INTEL_COLOR),
                                      "Intel");
                  break;

                case 4098:
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_AMD_COLOR), "AMD");
                  break;

                case 4318:
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_NVIDIA_COLOR),
                                      "Nvidia");
                  break;

                case 65541:
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_MESA_COLOR),
                                      "Mesa");
                  break;

                default:
                  ImGui::Text ("%d", engine->engineInfo.VkDeviceVendorId);
                  break;
                }

              // GPU Vendor ID
              ImGui::Text ("VENDOR ID :");
              ImGui::SameLine ();

              switch (engine->engineInfo.VkDeviceVendorId)
                {
                case 32902:
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_INTEL_COLOR),
                                      "%u 0x%X",
                                      engine->engineInfo.VkDeviceVendorId,
                                      engine->engineInfo.VkDeviceVendorId);
                  break;

                case 4098:
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_AMD_COLOR),
                                      "%u 0x%X",
                                      engine->engineInfo.VkDeviceVendorId,
                                      engine->engineInfo.VkDeviceVendorId);
                  break;

                case 4318:
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_NVIDIA_COLOR),
                                      "%u 0x%X",
                                      engine->engineInfo.VkDeviceVendorId,
                                      engine->engineInfo.VkDeviceVendorId);
                  break;

                case 65541:
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_MESA_COLOR),
                                      "%u 0x%X",
                                      engine->engineInfo.VkDeviceVendorId,
                                      engine->engineInfo.VkDeviceVendorId);
                  break;

                default:
                  ImGui::Text ("%u 0x%X", engine->engineInfo.VkDeviceVendorId,
                               engine->engineInfo.VkDeviceVendorId);
                  break;
                }
            }
          ImGui::EndTooltip ();
        }

      // Device type {Discrete?, Integrated?}
      if (engine->engineInfo.VkDeviceType[0] != '\0')
        ImGui::Text ("DEVICE TYPE : %s", engine->engineInfo.VkDeviceType);

      // Device Memory Size (VRAM size)
      ImGui::Text ("DEVICE VRAM : %.2f %s", engine->engineInfo.VkVramSize,
                   engine->engineInfo.VkVramSizeUnit);

      ImGui::Spacing ();

      // Queues in use
      if (ImGui::CollapsingHeader ("DEVICE QUEUES"))
        {
          if (ImGui::BeginTable ("Queues##AboutQueues", 3,
                                 PATACHE_IMGUI_TABLE_FLAGS))
            {
              ImGui::TableSetupColumn ("INDEX",
                                       ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableSetupColumn ("FAMILY PROPERTIES",
                                       ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableSetupColumn ("PRIORITY",
                                       ImGuiTableColumnFlags_WidthFixed);

              ImGui::TableHeadersRow ();

              // Queue Index
              ImGui::TableNextRow ();
              ImGui::TableSetColumnIndex (0);
              ImGui::Text ("%u", engine->Vulkan.GraphicsQueueFamilyIndex);

              // Queue Family Properties
              ImGui::TableSetColumnIndex (1);

              if (engine->engineInfo.VkQueueFlags
                  & vk::QueueFlagBits::eGraphics)
                ImGui::BulletText ("Graphics");

              if (engine->engineInfo.VkQueueFlags
                  & vk::QueueFlagBits::eCompute)
                ImGui::BulletText ("Compute");

              if (engine->engineInfo.VkQueueFlags
                  & vk::QueueFlagBits::eTransfer)
                ImGui::BulletText ("Transfer");

              if (engine->engineInfo.VkQueueFlags
                  & vk::QueueFlagBits::eSparseBinding)
                ImGui::BulletText ("Sparse Binding");

              if (engine->engineInfo.VkQueueFlags
                  & vk::QueueFlagBits::eProtected)
                ImGui::BulletText ("Protected");

              if (engine->engineInfo.VkQueueFlags
                  & vk::QueueFlagBits::eVideoDecodeKHR)
                ImGui::BulletText ("Video Decode KHR");

              if (engine->engineInfo.VkQueueFlags
                  & vk::QueueFlagBits::eVideoEncodeKHR)
                ImGui::BulletText ("Video Encode KHR");

              if (engine->engineInfo.VkQueueFlags
                  & vk::QueueFlagBits::eOpticalFlowNV)
                ImGui::BulletText ("Nvidia Optical Flow");

              // Queue Priority
              ImGui::TableSetColumnIndex (2);

              if (engine->engineInfo.VkQueuePriority == 1.0f)
                ImGui::Text ("%.2f (High)",
                             engine->engineInfo.VkQueuePriority);
              else if (engine->engineInfo.VkQueuePriority < 0.4f)
                ImGui::Text ("%.2f (Low)", engine->engineInfo.VkQueuePriority);
              else
                ImGui::Text ("%.2f", engine->engineInfo.VkQueuePriority);

              ImGui::EndTable ();
            }
        }

      ImGui::Spacing ();

      // Driver
      if (engine->engineInfo.VkDriverName[0] != '\0')
        {
          ImGui::BeginGroup ();
          ImGui::Text ("DRIVER :");
          ImGui::SameLine ();

          // Driver Name
          switch (engine->engineInfo.VkDeviceVendorId)
            {
            case 32902:
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_INTEL_COLOR), "%s",
                                  engine->engineInfo.VkDriverName);
              break;
            case 4098:
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_AMD_COLOR), "%s",
                                  engine->engineInfo.VkDriverName);
              break;
            case 4318:
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_NVIDIA_COLOR), "%s",
                                  engine->engineInfo.VkDriverName);
              break;
            case 65541:
              ImGui::TextColored (ImVec4 (PATACHE_IMGUI_MESA_COLOR), "%s",
                                  engine->engineInfo.VkDriverName);
              break;

            default:
              ImGui::Text ("%s", engine->engineInfo.VkDriverName);
              break;
            }

          ImGui::SameLine ();

          // Driver Version
          if (engine->engineInfo.VkDriverVersion[0] != '\0')
            ImGui::Text ("%s", engine->engineInfo.VkDriverVersion);

          ImGui::EndGroup ();

          // Driver Vendor Tooltip
          if (ImGui::BeginItemTooltip ())
            {
              if (engine->engineInfo.VkDriverId[0] != '\0')
                ImGui::Text ("ID : %s", engine->engineInfo.VkDriverId);

              if (engine->engineInfo.VkDriverInfo[0] != '\0')
                ImGui::Text ("INFO : %s", engine->engineInfo.VkDriverInfo);

              ImGui::EndTooltip ();
            }

          ImGui::Spacing ();
        }

      // Swapchain present mode
      ImGui::Text (
          "SWAPCHAIN PRESENT MODE : %s",
          vk::to_string (engine->engineInfo.VkSwapchainPresentMode).c_str ());

      // Is Vsync?
      if ((engine->engineInfo.VkSwapchainPresentMode
               == vk::PresentModeKHR::eFifo
           || engine->engineInfo.VkSwapchainPresentMode
                  == vk::PresentModeKHR::eFifoRelaxed
           || engine->engineInfo.VkSwapchainPresentMode
                  == vk::PresentModeKHR::eSharedContinuousRefresh)
          && (engine->configuration.Vsync))
        {
          ImGui::SameLine ();
          ImGui::TextColored (ImVec4 (PATACHE_IMGUI_POSITIVE_VALUE),
                              "Vertical sync");
        }

      // Image count
      ImGui::Text ("SWAPCHAIN IMAGE COUNT : %u",
                   engine->Vulkan.SwapChainImageCount);

      // Image Color Format
      ImGui::Text (
          "COLOR FORMAT : %s",
          vk::to_string (engine->engineInfo.VkSwapchainImageColorFormat)
              .c_str ());

      // Image Color Space
      ImGui::Text (
          "COLOR SPACE : %s",
          vk::to_string (engine->engineInfo.VkSwapchainImageColorSpace)
              .c_str ());

      // Drawable Size
      ImGui::Text ("SWAPCHAIN DRAWABLE SIZE : %u x %u",
                   engine->Vulkan.SwapChainExtent.width,
                   engine->Vulkan.SwapChainExtent.height);

      ImGui::Spacing ();

      ImGui::PopStyleVar ();
      ImGui::End ();
    }
  // End Raccoon Renderer Info

  // Patache Window configuration
  if (engine->engineInfo.PatacheConfigWindow)
    {
      ImGui::Begin ("Configuration##ConfigView",
                    &engine->engineInfo.PatacheConfigWindow);
      ImGui::TextColored (ImVec4 (PATACHE_IMGUI_WARNING_VALUE), "Danger Zone");

      // Show Fatal Error MessageBox
      if (ImGui::Checkbox ("Show Fatal Error "
                           "Messagebox##ConfigView_ShowFatalErrorMessagebox",
                           &engine->configuration.ShowFatalErrorMessagebox))
        engine->configuration.TriggeredChange
            |= Patache::ChangeConfigFlagBits::ShowFatalErrorMessagebox;

      // Vsync
      if (ImGui::Checkbox ("Vsync##ConfigView_Vsync",
                           &engine->configuration.Vsync))
        engine->configuration.TriggeredChange
            |= Patache::ChangeConfigFlagBits::Vsync;

      ImGui::SameLine ();

      // 10 Bit Depth
      if (ImGui::Checkbox ("10 Bit Depth##ConfigView_BitDepth",
                           &engine->configuration.BitDepth10))
        engine->configuration.TriggeredChange
            |= Patache::ChangeConfigFlagBits::BitDepth10;

      ImGui::Spacing ();

      // Add Image Count
      int ActualCount = engine->Vulkan.SwapChainImageCount;

      ImGui::Text ("Add Image Count");
      if (ImGui::SliderInt (
              "###ConfigView_AddImageCount", &ActualCount,
              engine->engineInfo.VkMinImageCount,
              static_cast<int> (engine->engineInfo.VkMinImageCount + 4),
              "%d Images"))
        engine->configuration.TriggeredChange
            |= Patache::ChangeConfigFlagBits::AddImageCount;

      ImGui::End ();
    }
  // End Patache Window configuration

  // Reset style outside of engine core
  ImGui::GetStyle ().Colors[ImGuiCol_TitleBgActive]
      = engine->engineInfo.engineStyles[0];
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGrip]
      = engine->engineInfo.engineStyles[1];
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripActive]
      = engine->engineInfo.engineStyles[2];
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripHovered]
      = engine->engineInfo.engineStyles[3];
  ImGui::GetStyle ().Colors[ImGuiCol_SeparatorHovered]
      = engine->engineInfo.engineStyles[4];
  ImGui::GetStyle ().Colors[ImGuiCol_SeparatorActive]
      = engine->engineInfo.engineStyles[5];
  ImGui::GetStyle ().Colors[ImGuiCol_Tab] = engine->engineInfo.engineStyles[6];
  ImGui::GetStyle ().Colors[ImGuiCol_TabSelected]
      = engine->engineInfo.engineStyles[7];
  ImGui::GetStyle ().Colors[ImGuiCol_TabHovered]
      = engine->engineInfo.engineStyles[8];
  ImGui::GetStyle ().Colors[ImGuiCol_Header]
      = engine->engineInfo.engineStyles[9];
  ImGui::GetStyle ().Colors[ImGuiCol_HeaderActive]
      = engine->engineInfo.engineStyles[10];
  ImGui::GetStyle ().Colors[ImGuiCol_HeaderHovered]
      = engine->engineInfo.engineStyles[11];
  ImGui::GetStyle ().Colors[ImGuiCol_Button]
      = engine->engineInfo.engineStyles[12];
  ImGui::GetStyle ().Colors[ImGuiCol_ButtonHovered]
      = engine->engineInfo.engineStyles[13];
  ImGui::GetStyle ().Colors[ImGuiCol_ButtonActive]
      = engine->engineInfo.engineStyles[14];
  ImGui::GetStyle ().Colors[ImGuiCol_FrameBg]
      = engine->engineInfo.engineStyles[15];
  ImGui::GetStyle ().Colors[ImGuiCol_FrameBgHovered]
      = engine->engineInfo.engineStyles[16];
  ImGui::GetStyle ().Colors[ImGuiCol_FrameBgActive]
      = engine->engineInfo.engineStyles[17];
  ImGui::GetStyle ().Colors[ImGuiCol_CheckMark]
      = engine->engineInfo.engineStyles[18];
  ImGui::GetStyle ().Colors[ImGuiCol_SliderGrab]
      = engine->engineInfo.engineStyles[19];
  ImGui::GetStyle ().Colors[ImGuiCol_SliderGrabActive]
      = engine->engineInfo.engineStyles[20];
}
