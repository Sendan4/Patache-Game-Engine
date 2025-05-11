// Vulkan Vendor
#define PATACHE_IMGUI_INTEL_COLOR          0.314f, 0.759f, 0.950f, 1.0f
#define PATACHE_IMGUI_AMD_COLOR            0.909f, 0.180f, 0.180f, 1.0f
#define PATACHE_IMGUI_NVIDIA_COLOR         0.463f, 0.725f, 0.0f, 1.0f
#define PATACHE_IMGUI_MESA_COLOR           0.844f, 0.867f, 0.870f, 1.0f
#define PATACHE_IMGUI_UNKNOWN_VENDOR_COLOR 0.640f, 0.646f, 0.646f, 1.0f

// ImGui styles
// table
#define PATACHE_IMGUI_TABLE_PADDING     10, 4
#define PATACHE_IMGUI_TITLE_TABLE_COLOR 0.930f, 0.718f, 0.223f, 1.0f
#define PATACHE_IMGUI_TABLE_FLAGS                                             \
  ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuterH                       \
      | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_NoHostExtendX

// Error, Warning and positive values
#define PATACHE_IMGUI_POSITIVE_VALUE 0.634f, 0.990f, 0.657f, 1.0f
#define PATACHE_IMGUI_WARNING_VALUE  0.984f, 0.990f, 0.634f, 1.0f
#define PATACHE_IMGUI_ERROR_VALUE    0.990f, 0.669f, 0.634f, 1.0f

// Custom Styles
#define PATACHE_IMGUI_PATACHE_COLOR               0.596f, 0.463f, 0.278f, 1.0f
#define PATACHE_IMGUI_PATACHE_SELECTED_COLOR      0.868f, 0.663f, 0.380f, 1.0f
#define PATACHE_IMGUI_PATACHE_DARKER_COLOR        0.353f, 0.267f, 0.153f, 1.0f
#define PATACHE_IMGUI_PATACHE_INFO_COLOR          0.341f, 0.341f, 0.341f, 1.0f
#define PATACHE_IMGUI_PATACHE_INFO_DARKER_COLOR   0.200f, 0.200f, 0.200f, 1.0f
#define PATACHE_IMGUI_PATACHE_INFO_SELECTED_COLOR 0.404f, 0.404f, 0.404f, 1.0f
#define PATACHE_IMGUI_PATACHE_BORDER_WINDOW_COLOR 1.0f, 1.0f, 1.0f, 1.0f
#define PATACHE_IMGUI_PATACHE_NONE                0.0f, 0.0f, 0.0f, 1.0f

namespace Patache
{
static inline void
DrawDebugUI (Patache::EngineInfo * engineInfo, Patache::Config & configuration,
             const std::uint32_t & SwapChainImageCount,
             const vk::Extent2D &  SwapChainExtent)
{
  // Guardando los estilos predeterminados para revertir los cambios
  ImVec4 tmp[16]{ ImGui::GetStyle ().Colors[ImGuiCol_TitleBgActive],
                  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGrip],
                  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripActive],
                  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripHovered],
                  ImGui::GetStyle ().Colors[ImGuiCol_SeparatorHovered],
                  ImGui::GetStyle ().Colors[ImGuiCol_SeparatorActive],
                  ImGui::GetStyle ().Colors[ImGuiCol_Tab],
                  ImGui::GetStyle ().Colors[ImGuiCol_TabSelected],
                  ImGui::GetStyle ().Colors[ImGuiCol_TabHovered],
                  ImGui::GetStyle ().Colors[ImGuiCol_Header],
                  ImGui::GetStyle ().Colors[ImGuiCol_HeaderActive],
                  ImGui::GetStyle ().Colors[ImGuiCol_HeaderHovered],
                  ImGui::GetStyle ().Colors[ImGuiCol_Border],
                  ImGui::GetStyle ().Colors[ImGuiCol_Button],
                  ImGui::GetStyle ().Colors[ImGuiCol_ButtonHovered],
                  ImGui::GetStyle ().Colors[ImGuiCol_ButtonActive] };

  // Patache Style
  ImGui::GetStyle ().Colors[ImGuiCol_TitleBgActive]
      = ImVec4 (PATACHE_IMGUI_PATACHE_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGrip]
      = ImVec4 (PATACHE_IMGUI_PATACHE_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripActive]
      = ImVec4 (PATACHE_IMGUI_PATACHE_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripHovered]
      = ImVec4 (PATACHE_IMGUI_PATACHE_SELECTED_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_SeparatorHovered]
      = ImVec4 (PATACHE_IMGUI_PATACHE_BORDER_WINDOW_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_SeparatorActive]
      = ImVec4 (PATACHE_IMGUI_PATACHE_BORDER_WINDOW_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_Tab]
      = ImVec4 (PATACHE_IMGUI_PATACHE_DARKER_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_TabSelected]
      = ImVec4 (PATACHE_IMGUI_PATACHE_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_TabHovered]
      = ImVec4 (PATACHE_IMGUI_PATACHE_SELECTED_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_Button]
      = ImVec4 (PATACHE_IMGUI_PATACHE_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_ButtonHovered]
      = ImVec4 (PATACHE_IMGUI_PATACHE_SELECTED_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_ButtonActive]
      = ImVec4 (PATACHE_IMGUI_PATACHE_DARKER_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_Header]
      = ImVec4 (PATACHE_IMGUI_PATACHE_INFO_DARKER_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_HeaderActive]
      = ImVec4 (PATACHE_IMGUI_PATACHE_INFO_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_HeaderHovered]
      = ImVec4 (PATACHE_IMGUI_PATACHE_INFO_SELECTED_COLOR);

  ImGui::GetStyle ().FrameBorderSize  = 0.0f;
  ImGui::GetStyle ().WindowBorderSize = 0.0f;
  // End Patache Style

  // Menu Bar
  if (engineInfo->ShowMainMenuBar)
    {
      ImGui::GetStyle ().Colors[ImGuiCol_Button]
          = ImVec4 (0.0f, 0.0f, 0.0f, 0.0f);
      ImGui::GetStyle ().Colors[ImGuiCol_ButtonHovered]
          = ImVec4 (PATACHE_IMGUI_PATACHE_SELECTED_COLOR);
      ImGui::GetStyle ().Colors[ImGuiCol_ButtonActive]
          = ImVec4 (PATACHE_IMGUI_PATACHE_DARKER_COLOR);

      if (ImGui::BeginMainMenuBar ())
        {
          if (ImGui::Button ("(Patache Icon)"))
            // Cambiar bits
            engineInfo->ShowMainMenuBar ^= 0b00000001;

          if (ImGui::BeginMenu ("Raccoon"))
            {
              if (ImGui::MenuItem ("Renderer", ""))
                // Cambiar bits
                engineInfo->PatacheRaccoonRendererInfoWindow ^= 0b00000001;

              // No implementado aun
              // ImGui::MenuItem ("Audio", "");

              ImGui::EndMenu ();
            }

          // No implementado aun
          /*
          if (ImGui::BeginMenu ("Tools"))
            {
              ImGui::EndMenu ();
            }
          */

          if (ImGui::Button ("Configuration"))
            // Cambiar bits
            engineInfo->PatacheConfigWindow ^= 0b00000001;

          if (ImGui::Button ("About"))
            // Cambiar bits
            engineInfo->PatacheInfoWindow ^= 0b00000001;

          ImGui::EndMainMenuBar ();
        }

      ImGui::GetStyle ().Colors[ImGuiCol_Button]
          = ImVec4 (PATACHE_IMGUI_PATACHE_COLOR);
    }
  // End Menu Bar

  // Patache Window Info
  if (engineInfo->PatacheInfoWindow)
    {
      ImGui::SetNextWindowSize (ImVec2 (570, 550), ImGuiCond_FirstUseEver);

      ImGui::Begin ("About the Patache Engine",
                    &engineInfo->PatacheInfoWindow);

      ImGui::PushStyleVar (ImGuiStyleVar_CellPadding,
                           ImVec2 (PATACHE_IMGUI_TABLE_PADDING));

      ImGui::Spacing ();
      ImGui::SeparatorText (PATACHE_ENGINE_NAME);

      if (ImGui::BeginTable ("##AboutBasicInfo", 3, PATACHE_IMGUI_TABLE_FLAGS))
        {
          ImGui::TableSetupColumn ("", ImGuiTableColumnFlags_WidthFixed);
          ImGui::TableSetupColumn ("", ImGuiTableColumnFlags_WidthFixed);

#if defined(PATACHE_ENGINE_VERSION)
          ImGui::TableNextRow ();
          ImGui::TableSetColumnIndex (0);
          ImGui::Text ("Version");
          ImGui::TableSetColumnIndex (1);
          ImGui::Text (PATACHE_ENGINE_VERSION);
#endif
#if defined(__DATE__) && defined(__TIME__)
          ImGui::TableNextRow ();
          ImGui::TableSetColumnIndex (0);
          ImGui::Text ("Build Date");
          ImGui::TableSetColumnIndex (1);
          ImGui::Text ("%s %s", __DATE__, __TIME__);
#endif

#if defined(PATACHE_ARCH)
          ImGui::TableNextRow ();
          ImGui::TableSetColumnIndex (0);
          ImGui::Text ("CPU Architecture Target");

          ImGui::TableSetColumnIndex (1);
          ImGui::Text (PATACHE_ARCH);
#endif
          ImGui::EndTable ();
        }

      ImGui::Text ("Simple and free Game Engine");
      ImGui::Text ("%s is under the MIT License.", PATACHE_ENGINE_NAME);
      ImGui::Spacing ();

      // General Engine Info
      if (ImGui::CollapsingHeader ("Advanced##AdvancedInfo"))
        {
          // Tabla de Git
          if (ImGui::TreeNode ("Git##AdvancedInfoGit"))
            {

              if (ImGui::BeginTable ("Git", 3, PATACHE_IMGUI_TABLE_FLAGS))
                {
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);

                  // Git Branch
#if defined(PATACHE_GIT_BRANCH)
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Branch");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (PATACHE_GIT_BRANCH);
#endif

                  // Git Commit - Short And Long
#if defined(PATACHE_GIT_HASH_SHORT) || defined(PATACHE_GIT_HASH_LONG)
                  ImGui::TableNextRow ();
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Commit Hash");
                  ImGui::TableSetColumnIndex (1);
#endif
#if defined(PATACHE_GIT_HASH_LONG)
                  ImGui::Text (PATACHE_GIT_HASH_LONG);
#endif
#if defined(PATACHE_GIT_HASH_SHORT)
                  ImGui::Text (PATACHE_GIT_HASH_SHORT);
#endif

                  // Git Stage
#if defined(PATACHE_GIT_WORK_DIR_IS_CLEAN)                                    \
    && defined(PATACHE_GIT_WORK_DIR_IS_CLEAN_BOOL)                            \
    && defined(PATACHE_GIT_WORK_DIR_IS_STAGED_BOOL)
                  ImGui::TableNextRow ();
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Source State");

                  ImGui::TableSetColumnIndex (1);
#if PATACHE_GIT_WORK_DIR_IS_CLEAN_BOOL == 1
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_POSITIVE_VALUE),
                                      "Clean");
#else
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_WARNING_VALUE),
                                      "Dirty");
#endif

#if PATACHE_GIT_WORK_DIR_IS_STAGED_BOOL == 1
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_POSITIVE_VALUE),
                                      "In Stage Area");
#else
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_WARNING_VALUE),
                                      "Not Staged");
#endif
#endif
                  ImGui::EndTable ();
                }
              ImGui::TreePop ();
            }

          // Tabla General
          if (ImGui::TreeNode ("Engine"))
            {
              if (ImGui::BeginTable ("Engine", 3, PATACHE_IMGUI_TABLE_FLAGS))
                {
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);

                  // Compiler
#if defined(PATACHE_COMPILER_PROGRAM) && defined(PATACHE_COMPILER)
                  ImGui::TableNextRow ();
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Compiler");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (PATACHE_COMPILER_PROGRAM);

                  ImGui::TableSetColumnIndex (2);
                  ImGui::Text (PATACHE_COMPILER);
#if defined(PATACHE_COMPILER_VERSION)
                  ImGui::Text (PATACHE_COMPILER_VERSION);
#endif
#endif

                  // Build System
#if defined(PATACHE_BUILD_SYSTEM) && defined(PATACHE_BUILD_SYSTEM_GENERATOR)
                  ImGui::TableNextRow ();
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Build System");

                  // Build System And Version
                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (PATACHE_BUILD_SYSTEM);
#if defined(PATACHE_BUILD_SYSTEM_VERSION)
                  ImGui::Text (PATACHE_BUILD_SYSTEM_VERSION);
#endif

                  // Generator And Version
                  ImGui::TableSetColumnIndex (2);
                  ImGui::Text (PATACHE_BUILD_SYSTEM_GENERATOR);
#if defined(PATACHE_BUILD_SYSTEM_GENERATOR_VERSION)
                  ImGui::Text (PATACHE_BUILD_SYSTEM_GENERATOR_VERSION);
#endif
#endif

                  // Build Type
#if defined(PATACHE_BUILD_TYPE)
                  ImGui::TableNextRow ();
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Build Type");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (PATACHE_BUILD_TYPE);
#endif

                  // Operating System
#if defined(PATACHE_OS)
                  ImGui::TableNextRow ();
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Operating System");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (PATACHE_OS);
#endif
                  ImGui::EndTable ();
                }
              ImGui::Spacing ();
              ImGui::TreePop ();
            }

          // Tabla para ver las versiones de las librerias
          if (ImGui::TreeNode ("Libraries used"))
            {
              if (ImGui::BeginTable ("Libraries", 3,
                                     PATACHE_IMGUI_TABLE_FLAGS))
                {
                  ImGui::TableSetupColumn ("Library",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableSetupColumn ("Version",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableSetupColumn ("Licence",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableHeadersRow ();

                  ImGui::TableNextRow ();
                  // Fast IO Commit Hash Short
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Fast IO");

                  ImGui::TableSetColumnIndex (2);
                  ImGui::Text ("MIT");
                  ImGui::TableNextRow ();

                  // RapidYaml Version
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("RapidYaml");

                  ImGui::TableSetColumnIndex (2);
                  ImGui::Text ("MIT");
                  ImGui::TableNextRow ();

                  // SDL Version
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("SDL");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text ("%u.%u.%u", SDL_MAJOR_VERSION,
                               SDL_MINOR_VERSION, SDL_MICRO_VERSION);

                  ImGui::TableSetColumnIndex (2);
                  ImGui::Text ("zlib");

                  // Vulkan Version
                  if (engineInfo->VkVersion[0] != '\0')
                    {
                      ImGui::TableNextRow ();

                      ImGui::TableSetColumnIndex (0);
                      ImGui::Text ("Vulkan");

                      ImGui::TableSetColumnIndex (1);
                      ImGui::Text ("%s", engineInfo->VkVersion);

                      ImGui::TableSetColumnIndex (2);
                      ImGui::Text ("Apache 2.0");
                    }

                    // ImGui Version
#if defined(IMGUI_VERSION) && defined(IMGUI_VERSION_NUM)
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("ImGui");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text ("%s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);

                  ImGui::TableSetColumnIndex (2);
                  ImGui::Text ("MIT");
#endif

                  ImGui::EndTable ();
                }
              ImGui::TreePop ();
            }
          ImGui::Spacing ();
        }

      if (ImGui::CollapsingHeader ("Window"))
        {
          ImGui::Text ("Creation Flags");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_FULLSCREEN)
            ImGui::BulletText ("fullscreen window");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_OPENGL)
            ImGui::BulletText ("window usable with OpenGL context");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_OCCLUDED)
            ImGui::BulletText ("window is occluded");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_HIDDEN)
            ImGui::BulletText ("window is not visible");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_BORDERLESS)
            ImGui::BulletText ("no window decoration");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_RESIZABLE)
            ImGui::BulletText ("window can be resized");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_MINIMIZED)
            ImGui::BulletText ("window is minimized");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_MAXIMIZED)
            ImGui::BulletText ("window is maximized");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_MOUSE_GRABBED)
            ImGui::BulletText ("window has grabbed mouse input");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_INPUT_FOCUS)
            ImGui::BulletText ("window has input focus");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_MOUSE_FOCUS)
            ImGui::BulletText ("window has mouse focus");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_EXTERNAL)
            ImGui::BulletText ("window not created by SDL");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_MODAL)
            ImGui::BulletText ("window is modal");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_HIGH_PIXEL_DENSITY)
            ImGui::BulletText (
                "window uses high pixel density back buffer if possible");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_MOUSE_CAPTURE)
            ImGui::BulletText ("window has mouse captured "
                               "(unrelated to MOUSE_GRABBED)");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_MOUSE_RELATIVE_MODE)
            ImGui::BulletText ("window has relative mode enabled");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_ALWAYS_ON_TOP)
            ImGui::BulletText ("window should always be above others");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_UTILITY)
            ImGui::BulletText ("window should be treated as a utility window");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_TOOLTIP)
            ImGui::BulletText ("window should be treated as a tooltip");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_POPUP_MENU)
            ImGui::BulletText ("window should be treated as a popup menu");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_KEYBOARD_GRABBED)
            ImGui::BulletText ("window has grabbed keyboard input");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_VULKAN)
            ImGui::BulletText ("window usable for Vulkan surface");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_METAL)
            ImGui::BulletText ("window usable for Metal view");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_TRANSPARENT)
            ImGui::BulletText ("window with transparent buffer");

          if (engineInfo->WindowCreationFlags & SDL_WINDOW_NOT_FOCUSABLE)
            ImGui::BulletText ("window should not be focusable");

          ImGui::Spacing ();
        }

      ImGui::PopStyleVar ();
      ImGui::End ();
    }
  // End Patache Window Info

  // Raccoon Renderer Info
  if (engineInfo->PatacheRaccoonRendererInfoWindow)
    {
      ImGui::SetNextWindowSize (ImVec2 (570, 550), ImGuiCond_FirstUseEver);

      ImGui::Begin ("Raccoon Renderer Info",
                    &engineInfo->PatacheRaccoonRendererInfoWindow);

      ImGui::PushStyleVar (ImGuiStyleVar_CellPadding,
                           ImVec2 (PATACHE_IMGUI_TABLE_PADDING));

      ImGui::Spacing ();

      if (engineInfo->VkVersion[0] != '\0')
        ImGui::Text ("Using the backend Vulkan %s", engineInfo->VkVersion);

      if (ImGui::CollapsingHeader ("Extensions and layers##RRI2"))
        {
#if defined(PATACHE_USE_VVL)
          if (ImGui::BeginTable ("Layers##VkLayerList", 1,
                                 PATACHE_IMGUI_TABLE_FLAGS))
            {
              ImGui::TableSetupColumn ("Layer",
                                       ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableHeadersRow ();

              for (std::uint16_t i = 0; i < PATACHE_VK_LAYER_COUNT; ++i)
                {
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("%s", engineInfo->ppVkLayers[i]);
                }

              ImGui::EndTable ();
            }
#endif

          if (ImGui::BeginTable ("Instance##VkInstanceList", 1,
                                 PATACHE_IMGUI_TABLE_FLAGS))
            {
              ImGui::TableSetupColumn ("Instance",
                                       ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableHeadersRow ();

              for (std::uint16_t i = 0;
                   i < engineInfo->VkInstanceExtensionsCount; ++i)
                {
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("%s", engineInfo->ppVkInstanceExtensions[i]);
                }
              ImGui::EndTable ();
            }

          if (ImGui::BeginTable ("Device##VkDeviceList", 1,
                                 PATACHE_IMGUI_TABLE_FLAGS))
            {
              ImGui::TableSetupColumn ("Device",
                                       ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableHeadersRow ();

              for (std::uint32_t i = 0;
                   i < engineInfo->VkDeviceExtensionsCount; ++i)
                {
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("%s", engineInfo->ppVkDeviceExtensions[i]);
                }
              ImGui::EndTable ();
            }
          ImGui::Spacing ();
        }

      // Device
      if (ImGui::CollapsingHeader ("Device##RRI3"))
        {
          if (engineInfo->VkDeviceName[0] != '\0')
            {
              switch (engineInfo->VkDeviceVendorId)
                {
                case 32902:
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_INTEL_COLOR), "%s",
                                      engineInfo->VkDeviceName);
                  break;
                case 4098:
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_AMD_COLOR), "%s",
                                      engineInfo->VkDeviceName);
                  break;
                case 4318:
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_NVIDIA_COLOR),
                                      "%s", engineInfo->VkDeviceName);
                  break;
                case 65541:
                  ImGui::TextColored (ImVec4 (PATACHE_IMGUI_MESA_COLOR),
                                      "Mesa %s", engineInfo->VkDeviceName);
                  break;
                default:
                  ImGui::Text ("Unknown");
                  break;
                }
            }

          if (ImGui::BeginTable ("DeviceInfo##AboutDevice", 2,
                                 PATACHE_IMGUI_TABLE_FLAGS))
            {
              ImGui::TableSetupColumn ("", ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableSetupColumn ("", ImGuiTableColumnFlags_WidthFixed);

              if (engineInfo->VkDeviceType[0] != '\0')
                {
                  ImGui::TableNextRow ();
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Type");
                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text ("%s", engineInfo->VkDeviceType);
                }

              if (engineInfo->VkDeviceVendorId != 0)
                {
                  ImGui::TableNextRow ();
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Vendor");

                  ImGui::TableSetColumnIndex (1);
                  switch (engineInfo->VkDeviceVendorId)
                    {
                    case 32902:
                      ImGui::TextColored (ImVec4 (PATACHE_IMGUI_INTEL_COLOR),
                                          "Intel");
                      break;
                    case 4098:
                      ImGui::TextColored (ImVec4 (PATACHE_IMGUI_AMD_COLOR),
                                          "AMD");
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
                      ImGui::TextColored (
                          ImVec4 (PATACHE_IMGUI_UNKNOWN_VENDOR_COLOR),
                          "Unknown");
                      break;
                    }
                }

              ImGui::Text ("%u  0x%X", engineInfo->VkDeviceVendorId,
                           engineInfo->VkDeviceVendorId);

              ImGui::EndTable ();
            }

          // Queues
          ImGui::Text ("Queues");
          if (ImGui::BeginTable ("Queues##AboutQueues", 3,
                                 PATACHE_IMGUI_TABLE_FLAGS))
            {
              ImGui::TableSetupColumn ("Index",
                                       ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableSetupColumn ("Family Properties",
                                       ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableSetupColumn ("Priority",
                                       ImGuiTableColumnFlags_WidthFixed);

              ImGui::TableHeadersRow ();

              // Queue Index
              ImGui::TableNextRow ();
              ImGui::TableSetColumnIndex (0);
              ImGui::Text ("%u", engineInfo->VkQueueIndex);

              // Queue Family Properties
              ImGui::TableSetColumnIndex (1);

              if (engineInfo->VkQueueFlags & vk::QueueFlagBits::eGraphics)
                ImGui::BulletText ("Graphics");

              if (engineInfo->VkQueueFlags & vk::QueueFlagBits::eCompute)
                ImGui::BulletText ("Compute");

              if (engineInfo->VkQueueFlags & vk::QueueFlagBits::eTransfer)
                ImGui::BulletText ("Transfer");

              if (engineInfo->VkQueueFlags & vk::QueueFlagBits::eSparseBinding)
                ImGui::BulletText ("Sparse Binding");

              if (engineInfo->VkQueueFlags & vk::QueueFlagBits::eProtected)
                ImGui::BulletText ("Protected");

              if (engineInfo->VkQueueFlags
                  & vk::QueueFlagBits::eVideoDecodeKHR)
                ImGui::BulletText ("Video Decode KHR");

              if (engineInfo->VkQueueFlags
                  & vk::QueueFlagBits::eVideoEncodeKHR)
                ImGui::BulletText ("Video Encode KHR");

              if (engineInfo->VkQueueFlags & vk::QueueFlagBits::eOpticalFlowNV)
                ImGui::BulletText ("Nvidia Optical Flow");

              // Queue Priority
              ImGui::TableSetColumnIndex (2);

              if (engineInfo->VkQueuePriority == 1.0f)
                ImGui::Text ("%f (High)", engineInfo->VkQueuePriority);
              else if (engineInfo->VkQueuePriority < 0.4f)
                ImGui::Text ("%f (Low)", engineInfo->VkQueuePriority);
              else
                ImGui::Text ("%f", engineInfo->VkQueuePriority);

              ImGui::EndTable ();
            }
          ImGui::Spacing ();
        }

      // Driver
      if (ImGui::CollapsingHeader ("Driver ##RRI4"))
        {
          if (engineInfo->VkDriverName[0] != '\0')
            {
              ImGui::Text ("%s", engineInfo->VkDriverName);
            }

          if (ImGui::BeginTable ("Vulkan Driver Info##1", 2,
                                 PATACHE_IMGUI_TABLE_FLAGS))
            {
              ImGui::TableSetupColumn ("", ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableSetupColumn ("", ImGuiTableColumnFlags_WidthFixed);

              if (engineInfo->VkDriverId[0] != '\0')
                {
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Id");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text ("%s", engineInfo->VkDriverId);
                }

              if (engineInfo->VkDriverInfo[0] != '\0')
                {
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Info");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text ("%s", engineInfo->VkDriverInfo);
                }

              if (engineInfo->VkDriverVersion[0] != '\0')
                {
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Version");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text ("%s", engineInfo->VkDriverVersion);
                }

              ImGui::EndTable ();
            }
          ImGui::Spacing ();
        }

      // Swapchain
      if (ImGui::CollapsingHeader ("SwapChain##RRI5"))
        {
          if (ImGui::BeginTable ("Vulkan SwapChain Info##1", 2,
                                 PATACHE_IMGUI_TABLE_FLAGS))
            {
              ImGui::TableSetupColumn ("", ImGuiTableColumnFlags_WidthFixed);
              ImGui::TableSetupColumn ("", ImGuiTableColumnFlags_WidthFixed);

              // Present Mode
              ImGui::TableNextRow ();
              ImGui::TableSetColumnIndex (0);
              ImGui::Text ("Present Mode");

              ImGui::TableSetColumnIndex (1);
              ImGui::Text (
                  "%s",
                  vk::to_string (engineInfo->VkSwapchainPresentMode).c_str ());

              if ((engineInfo->VkSwapchainPresentMode
                       == vk::PresentModeKHR::eFifo
                   || engineInfo->VkSwapchainPresentMode
                          == vk::PresentModeKHR::eFifoRelaxed
                   || engineInfo->VkSwapchainPresentMode
                          == vk::PresentModeKHR::eSharedContinuousRefresh)
                  && (configuration.Vsync))
                ImGui::TextColored (ImVec4 (PATACHE_IMGUI_POSITIVE_VALUE),
                                    "Vertical Sync");
              else if ((engineInfo->VkSwapchainPresentMode
                            == vk::PresentModeKHR::eMailbox
                        || engineInfo->VkSwapchainPresentMode
                               == vk::PresentModeKHR::eImmediate
                        || engineInfo->VkSwapchainPresentMode
                               == vk::PresentModeKHR::eSharedDemandRefresh)
                       && (!configuration.Vsync))
                ImGui::TextColored (ImVec4 (PATACHE_IMGUI_WARNING_VALUE),
                                    "Not synchronized with the refresh rate");

              // Images
              ImGui::TableNextRow ();
              ImGui::TableSetColumnIndex (0);
              ImGui::Text ("Images");

              ImGui::TableSetColumnIndex (1);
              ImGui::Text ("%u", SwapChainImageCount);

              // Drawable Size
              ImGui::TableNextRow ();
              ImGui::TableSetColumnIndex (0);
              ImGui::Text ("Drawable Size");

              ImGui::TableSetColumnIndex (1);
              ImGui::Text ("%u x %u", SwapChainExtent.width,
                           SwapChainExtent.height);

              // Image Color Format
              ImGui::TableNextRow ();
              ImGui::TableSetColumnIndex (0);
              ImGui::Text ("Image Color Format");

              ImGui::TableSetColumnIndex (1);
              ImGui::Text (
                  "%s", vk::to_string (engineInfo->VkSwapchainImageColorFormat)
                            .c_str ());

              // Image Color Space
              ImGui::TableNextRow ();
              ImGui::TableSetColumnIndex (0);
              ImGui::Text ("Image Color Space");

              ImGui::TableSetColumnIndex (1);
              ImGui::Text (
                  "%s", vk::to_string (engineInfo->VkSwapchainImageColorSpace)
                            .c_str ());

              ImGui::EndTable ();
            }
          ImGui::Spacing ();
        }

      ImGui::PopStyleVar ();
      ImGui::End ();
    }
  // End Raccoon Renderer Info

  // Patache Window configuration
  if (engineInfo->PatacheConfigWindow)
    {
      ImGui::Begin ("Configuration##ConfigView",
                    &engineInfo->PatacheConfigWindow);

      // Show Fatal Error MessageBox
      if (ImGui::Checkbox ("Show Fatal Error "
                           "Messagebox##ConfigView_ShowFatalErrorMessagebox",
                           &configuration.ShowFatalErrorMessagebox))
        configuration.TriggeredChange
            |= Patache::ChangeConfigFlagBits::ShowFatalErrorMessagebox;

      // Vsync
      if (ImGui::Checkbox ("Vsync##ConfigView_Vsync", &configuration.Vsync))
        configuration.TriggeredChange |= Patache::ChangeConfigFlagBits::Vsync;

      // 10 Bit Depth
      if (ImGui::Checkbox ("10 Bit Depth##ConfigView_BitDepth",
                           &configuration.BitDepth10))
        configuration.TriggeredChange
            |= Patache::ChangeConfigFlagBits::BitDepth10;

      // Add Image Count
      ImGui::Spacing ();

      int ActualCount = SwapChainImageCount;

      ImGui::Text ("Add Image Count");

      if (ImGui::SliderInt (
              "##ConfigView_AddImageCount", &ActualCount, 0,
              static_cast<int> (engineInfo->VkMinImageCount + 4)))
        configuration.TriggeredChange
            |= Patache::ChangeConfigFlagBits::AddImageCount;

      ImGui::End ();
    }
  // End Patache Window configuration

  // Reset Style
  ImGui::GetStyle ().Colors[ImGuiCol_TitleBgActive]     = tmp[0];
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGrip]        = tmp[1];
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripActive]  = tmp[2];
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripHovered] = tmp[3];
  ImGui::GetStyle ().Colors[ImGuiCol_SeparatorHovered]  = tmp[4];
  ImGui::GetStyle ().Colors[ImGuiCol_SeparatorActive]   = tmp[5];
  ImGui::GetStyle ().Colors[ImGuiCol_Tab]               = tmp[6];
  ImGui::GetStyle ().Colors[ImGuiCol_TabSelected]       = tmp[7];
  ImGui::GetStyle ().Colors[ImGuiCol_TabHovered]        = tmp[8];
  ImGui::GetStyle ().Colors[ImGuiCol_Header]            = tmp[9];
  ImGui::GetStyle ().Colors[ImGuiCol_HeaderActive]      = tmp[10];
  ImGui::GetStyle ().Colors[ImGuiCol_HeaderHovered]     = tmp[11];
  ImGui::GetStyle ().Colors[ImGuiCol_Border]            = tmp[12];
  ImGui::GetStyle ().Colors[ImGuiCol_Button]            = tmp[13];
  ImGui::GetStyle ().Colors[ImGuiCol_ButtonHovered]     = tmp[14];
  ImGui::GetStyle ().Colors[ImGuiCol_ButtonActive]      = tmp[15];
  ImGui::GetStyle ().FrameBorderSize                    = 0.0f;
}
}
