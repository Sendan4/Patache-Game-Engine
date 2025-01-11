// Vulkan Vendor
#define PATATA_IMGUI_INTEL_COLOR          0.314f, 0.759f, 0.950f, 1.0f
#define PATATA_IMGUI_AMD_COLOR            0.909f, 0.180f, 0.180f, 1.0f
#define PATATA_IMGUI_NVIDIA_COLOR         0.463f, 0.725f, 0.0f, 1.0f
#define PATATA_IMGUI_MESA_COLOR           0.844f, 0.867f, 0.870f, 1.0f
#define PATATA_IMGUI_UNKNOWN_VENDOR_COLOR 0.640f, 0.646f, 0.646f, 1.0f

// ImGui styles
// table
#define PATATA_IMGUI_TABLE_PADDING     10, 4
#define PATATA_IMGUI_TITLE_TABLE_COLOR 0.930f, 0.718f, 0.223f, 1.0f
#define PATATA_IMGUI_TABLE_FLAGS                                              \
  ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuterH                       \
      | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_NoHostExtendX
// Error, Warning and positive values
#define PATATA_IMGUI_POSITIVE_VALUE 0.634f, 0.990f, 0.657f, 1.0f
#define PATATA_IMGUI_WARNING_VALUE  0.984f, 0.990f, 0.634f, 1.0f
#define PATATA_IMGUI_ERROR_VALUE    0.990f, 0.669f, 0.634f, 1.0f
// Custom Styles
#define PATATA_IMGUI_PATATA_COLOR               0.596f, 0.463f, 0.278f, 1.0f
#define PATATA_IMGUI_PATATA_SELECTED_COLOR      0.868f, 0.663f, 0.380f, 1.0f
#define PATATA_IMGUI_PATATA_DARKER_COLOR        0.353f, 0.267f, 0.153f, 1.0f
#define PATATA_IMGUI_PATATA_INFO_COLOR          0.341f, 0.341f, 0.341f, 1.0f
#define PATATA_IMGUI_PATATA_INFO_DARKER_COLOR   0.200f, 0.200f, 0.200f, 1.0f
#define PATATA_IMGUI_PATATA_INFO_SELECTED_COLOR 0.404f, 0.404f, 0.404f, 1.0f
#define PATATA_IMGUI_PATATA_BORDER_WINDOW_COLOR 1.0f, 1.0f, 1.0f, 1.0f
#define PATATA_IMGUI_PATATA_NONE                0.0f, 0.0f, 0.0f, 1.0f

namespace Patata
{
inline void
DrawDebugUI (Patata::EngineInfo * pPatataEngineInfo,
             Patata::Config * pConfiguration, uint32_t & SwapChainImageCount,
             vk::Extent2D & SwapChainExtent)
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

  // Patata Style
  ImGui::GetStyle ().Colors[ImGuiCol_TitleBgActive]
      = ImVec4 (PATATA_IMGUI_PATATA_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGrip]
      = ImVec4 (PATATA_IMGUI_PATATA_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripActive]
      = ImVec4 (PATATA_IMGUI_PATATA_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_ResizeGripHovered]
      = ImVec4 (PATATA_IMGUI_PATATA_SELECTED_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_SeparatorHovered]
      = ImVec4 (PATATA_IMGUI_PATATA_BORDER_WINDOW_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_SeparatorActive]
      = ImVec4 (PATATA_IMGUI_PATATA_BORDER_WINDOW_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_Tab]
      = ImVec4 (PATATA_IMGUI_PATATA_DARKER_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_TabSelected]
      = ImVec4 (PATATA_IMGUI_PATATA_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_TabHovered]
      = ImVec4 (PATATA_IMGUI_PATATA_SELECTED_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_Button]
      = ImVec4 (PATATA_IMGUI_PATATA_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_ButtonHovered]
      = ImVec4 (PATATA_IMGUI_PATATA_SELECTED_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_ButtonActive]
      = ImVec4 (PATATA_IMGUI_PATATA_DARKER_COLOR);

  ImGui::GetStyle ().Colors[ImGuiCol_Header]
      = ImVec4 (PATATA_IMGUI_PATATA_INFO_DARKER_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_HeaderActive]
      = ImVec4 (PATATA_IMGUI_PATATA_INFO_COLOR);
  ImGui::GetStyle ().Colors[ImGuiCol_HeaderHovered]
      = ImVec4 (PATATA_IMGUI_PATATA_INFO_SELECTED_COLOR);

  ImGui::GetStyle ().FrameBorderSize  = 0.0f;
  ImGui::GetStyle ().WindowBorderSize = 0.0f;
  // End Patata Style

  // Menu Bar
  if (pPatataEngineInfo->ShowMainMenuBar)
    {
      ImGui::GetStyle ().Colors[ImGuiCol_Button]
          = ImVec4 (0.0f, 0.0f, 0.0f, 0.0f);
      ImGui::GetStyle ().Colors[ImGuiCol_ButtonHovered]
          = ImVec4 (PATATA_IMGUI_PATATA_SELECTED_COLOR);
      ImGui::GetStyle ().Colors[ImGuiCol_ButtonActive]
          = ImVec4 (PATATA_IMGUI_PATATA_DARKER_COLOR);

      ImGui::BeginMainMenuBar ();
      if (ImGui::Button ("(Patata Icon)"))
        {
          pPatataEngineInfo->ShowMainMenuBar = false;
        }

      if (ImGui::Button ("Info"))
        {
          if (pPatataEngineInfo->PatataInfoWindow)
            pPatataEngineInfo->PatataInfoWindow = false;
          else
            pPatataEngineInfo->PatataInfoWindow = true;
        }

      if (ImGui::Button ("Config"))
        {
          if (pPatataEngineInfo->PatataConfigWindow)
            pPatataEngineInfo->PatataConfigWindow = false;
          else
            pPatataEngineInfo->PatataConfigWindow = true;
        }
      ImGui::EndMainMenuBar ();

      ImGui::GetStyle ().Colors[ImGuiCol_Button]
          = ImVec4 (PATATA_IMGUI_PATATA_COLOR);
    }
  // End Menu Bar

  // Patata Window Info
  ImGui::SetNextWindowSize (ImVec2 (570, 550), ImGuiCond_FirstUseEver);

  if (pPatataEngineInfo->PatataInfoWindow)
    {
      ImGui::Begin ("Patata Engine | (Debug) Development",
                    &pPatataEngineInfo->PatataInfoWindow);

      ImGui::PushStyleVar (ImGuiStyleVar_CellPadding,
                           ImVec2 (PATATA_IMGUI_TABLE_PADDING));

      ImGui::Spacing ();

      // General Engine Info
      if (ImGui::CollapsingHeader ("General"))
        {
          // Tabla de Git
          if (ImGui::TreeNode ("Git"))
            {

              if (ImGui::BeginTable ("Git", 3, PATATA_IMGUI_TABLE_FLAGS))
                {
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);

                  // Git Branch
#if defined(PATATA_GIT_BRANCH)
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Branch");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (PATATA_GIT_BRANCH);
#endif

                  // Git Commit - Short And Long
#if defined(PATATA_GIT_HASH_SHORT) || defined(PATATA_GIT_HASH_LONG)
                  ImGui::TableNextRow ();
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Commit Hash");
                  ImGui::TableSetColumnIndex (1);
#endif
#if defined(PATATA_GIT_HASH_LONG)
                  ImGui::Text (PATATA_GIT_HASH_LONG);
#endif
#if defined(PATATA_GIT_HASH_SHORT)
                  ImGui::Text (PATATA_GIT_HASH_SHORT);
#endif

                  // Git Stage
#if defined(PATATA_GIT_WORK_DIR_IS_CLEAN)                                     \
    && defined(PATATA_GIT_WORK_DIR_IS_CLEAN_BOOL)                             \
    && defined(PATATA_GIT_WORK_DIR_IS_STAGED_BOOL)
                  ImGui::TableNextRow ();
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Source State");

                  ImGui::TableSetColumnIndex (1);
#if PATATA_GIT_WORK_DIR_IS_CLEAN_BOOL == 1
                  ImGui::TextColored (ImVec4 (PATATA_IMGUI_POSITIVE_VALUE),
                                      "Clean");
#else
                  ImGui::TextColored (ImVec4 (PATATA_IMGUI_WARNING_VALUE),
                                      "Dirty");
#endif

#if PATATA_GIT_WORK_DIR_IS_STAGED_BOOL == 1
                  ImGui::TextColored (ImVec4 (PATATA_IMGUI_POSITIVE_VALUE),
                                      "In Stage Area");
#else
                  ImGui::TextColored (ImVec4 (PATATA_IMGUI_WARNING_VALUE),
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
              if (ImGui::BeginTable ("Engine", 3, PATATA_IMGUI_TABLE_FLAGS))
                {
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);

                  ImGui::TableNextRow ();

                  // Version
#if defined(PATATA_ENGINE_VERSION)
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Version");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (PATATA_ENGINE_VERSION);
#endif

                  ImGui::TableNextRow ();

                  // Build Date
#if defined(__DATE__) && defined(__TIME__)
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Build Date");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text ("%s", __DATE__);

                  ImGui::TableSetColumnIndex (2);
                  ImGui::Text ("%s", __TIME__);
#endif

                  ImGui::TableNextRow ();

                  // Compiler
#if defined(PATATA_COMPILER_PROGRAM) && defined(PATATA_COMPILER)
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Compiler");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (PATATA_COMPILER_PROGRAM);

                  ImGui::TableSetColumnIndex (2);
                  ImGui::Text (PATATA_COMPILER);
#if defined(PATATA_COMPILER_VERSION)
                  ImGui::Text (PATATA_COMPILER_VERSION);
#endif
#endif

                  ImGui::TableNextRow ();

                  // Build System
#if defined(PATATA_BUILD_SYSTEM) && defined(PATATA_BUILD_SYSTEM_GENERATOR)
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Build System");

                  // Build System And Version
                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (PATATA_BUILD_SYSTEM);
#if defined(PATATA_BUILD_SYSTEM_VERSION)
                  ImGui::Text (PATATA_BUILD_SYSTEM_VERSION);
#endif

                  // Generator And Version
                  ImGui::TableSetColumnIndex (2);
                  ImGui::Text (PATATA_BUILD_SYSTEM_GENERATOR);
#if defined(PATATA_BUILD_SYSTEM_GENERATOR_VERSION)
                  ImGui::Text (PATATA_BUILD_SYSTEM_GENERATOR_VERSION);
#endif
#endif

                  ImGui::TableNextRow ();

                  // Build Type
#if defined(PATATA_BUILD_TYPE)
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Build Type");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (PATATA_BUILD_TYPE);
#endif

                  ImGui::TableNextRow ();

                  // Operating System
#if defined(PATATA_OS)
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Operating System");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (PATATA_OS);
#endif

                  ImGui::TableNextRow ();

                  // CPU Architecture Type
#if defined(PATATA_ARCH)
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("CPU Architecture");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (PATATA_ARCH);
#endif

                  // Game Name
#if PATATA_GAME_NAME != 0
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Game Name");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (PATATA_GAME_NAME);
#endif

                  ImGui::EndTable ();
                }
              ImGui::TreePop ();
            }

          // Tabla para ver las versiones de las librerias
          if (ImGui::TreeNode ("Libraries in use"))
            {
              if (ImGui::BeginTable ("Libraries", 2, PATATA_IMGUI_TABLE_FLAGS))
                {
                  ImGui::TableSetupColumn ("Name",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableSetupColumn ("Version",
                                           ImGuiTableColumnFlags_WidthFixed);

                  // Fast IO Commit Hash Short
#if defined(PATATA_FAST_IO_GIT_COMMIT_HASH_SHORT)
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Fast IO");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (PATATA_FAST_IO_GIT_COMMIT_HASH_SHORT);
#endif
#if defined(PATATA_RAPIDYAML_VERSION)
                  // RapidYaml Version
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("RapidYaml");

                  ImGui::TableSetColumnIndex (1);

                  ImGui::Text (PATATA_RAPIDYAML_VERSION);
#endif

                  ImGui::TableNextRow ();

                  // SDL Version
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("SDL");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text ("%u.%u.%u",
                               pPatataEngineInfo->WindowInfo.version.major,
                               pPatataEngineInfo->WindowInfo.version.minor,
                               pPatataEngineInfo->WindowInfo.version.patch);

                  // Vulkan Version
                  if (!pPatataEngineInfo->VkVersionInUse.empty ())
                    {
                      ImGui::TableNextRow ();

                      ImGui::TableSetColumnIndex (0);
                      ImGui::Text ("Vulkan");

                      ImGui::TableSetColumnIndex (1);
                      ImGui::Text ("%s",
                                   pPatataEngineInfo->VkVersionInUse.c_str ());
                    }

                    // ImGui Version
#if defined(IMGUI_VERSION) && defined(IMGUI_VERSION_NUM)
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("ImGui");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text ("%s (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
#endif

                  ImGui::EndTable ();
                }
              ImGui::TreePop ();
            }
          ImGui::Spacing ();
        }

      if (ImGui::CollapsingHeader ("Window"))
        {
          // Tabla acerca del escritorio
          if (ImGui::TreeNode ("Desktop"))
            {
              if (ImGui::BeginTable ("Desktop", 2, PATATA_IMGUI_TABLE_FLAGS))
                {
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);

                  ImGui::TableNextRow ();

                  // Desktop
#if defined(__linux__)
                  if (pPatataEngineInfo->Desktop != nullptr)
                    {
                      ImGui::TableSetColumnIndex (0);
                      ImGui::Text ("Desktop");

                      ImGui::TableSetColumnIndex (1);
                      ImGui::Text ("%s", pPatataEngineInfo->Desktop);

                      ImGui::TableNextRow ();
                    }
#endif

                  // Window System
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Window Subsystem");

                  ImGui::TableSetColumnIndex (1);
#if defined(_WIN64) // Windows
                  if (pPatataEngineInfo->WindowInfo.subsystem
                      == SDL_SYSWM_WINDOWS)
                    ImGui::Text ("Windows");
                  else if (pPatataEngineInfo->WindowInfo.subsystem
                           == SDL_SYSWM_WINRT)
                    ImGui::Text ("WinRT");
#else // Linux
                  if (pPatataEngineInfo->WindowInfo.subsystem == SDL_SYSWM_X11
                      && strcmp (pPatataEngineInfo->SessionType, "wayland")
                             == 0)
                    ImGui::Text ("XWayland");
                  else if (pPatataEngineInfo->WindowInfo.subsystem
                           == SDL_SYSWM_WAYLAND)
                    ImGui::Text ("Wayland");
#endif

#if defined(__linux__)
                  ImGui::TableNextRow ();

                  if (pPatataEngineInfo->SessionType != nullptr)
                    {
                      ImGui::TableSetColumnIndex (0);
                      ImGui::Text ("Session Type");

                      ImGui::TableSetColumnIndex (1);
                      ImGui::Text ("%s", pPatataEngineInfo->SessionType);
                    }
#endif
                  ImGui::EndTable ();
                }
              ImGui::TreePop ();
            }

          if (ImGui::TreeNode ("Window Manager##1"))
            {
              if (ImGui::BeginTable ("Desktop", 2, PATATA_IMGUI_TABLE_FLAGS))
                {
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);

#if defined(__linux__) // Linux
                  if (pPatataEngineInfo->WindowInfo.subsystem
                      == SDL_SYSWM_WAYLAND)
                    {
                      // Wayland
                      // Has Display?
                      if (pPatataEngineInfo->WindowInfo.info.wl.display
                          != nullptr)
                        {
                          ImGui::TableNextRow ();

                          ImGui::TableSetColumnIndex (0);
#if defined(__GNUC__) || defined(__MINGW64__)
                          ImGui::Text (
                              "%s", abi::__cxa_demangle (
                                        typeid (pPatataEngineInfo->WindowInfo
                                                    .info.wl.display)
                                            .name (),
                                        nullptr, nullptr, nullptr));
#else
                          ImGui::Text (typeid (pPatataEngineInfo->WindowInfo
                                                   .info.wl.display)
                                           .name ());
#endif

                          ImGui::TableSetColumnIndex (1);
                          ImGui::Text ("Wayland display");
                        }

                      // Has Surface?
                      if (pPatataEngineInfo->WindowInfo.info.wl.surface
                          != nullptr)
                        {
                          ImGui::TableNextRow ();

                          ImGui::TableSetColumnIndex (0);
#if defined(__GNUC__) || defined(__MINGW64__)
                          ImGui::Text (
                              "%s", abi::__cxa_demangle (
                                        typeid (pPatataEngineInfo->WindowInfo
                                                    .info.wl.surface)
                                            .name (),
                                        nullptr, nullptr, nullptr));
#else
                          ImGui::Text (typeid (pPatataEngineInfo->WindowInfo
                                                   .info.wl.surface)
                                           .name ());
#endif

                          ImGui::TableSetColumnIndex (1);
                          ImGui::Text ("Wayland surface");
                        }

                      // Has wayland xdg surface?
                      if (pPatataEngineInfo->WindowInfo.info.wl.xdg_surface
                          != nullptr)
                        {
                          ImGui::TableNextRow ();

                          ImGui::TableSetColumnIndex (0);
#if defined(__GNUC__) || defined(__MINGW64__)
                          ImGui::Text (
                              "%s", abi::__cxa_demangle (
                                        typeid (pPatataEngineInfo->WindowInfo
                                                    .info.wl.xdg_surface)
                                            .name (),
                                        nullptr, nullptr, nullptr));
#else
                          ImGui::Text (typeid (pPatataEngineInfo->WindowInfo
                                                   .info.wl.xdg_surface)
                                           .name ());
#endif

                          ImGui::TableSetColumnIndex (1);
                          ImGui::Text ("Wayland xdg surface (window "
                                       "manager handle)");
                        }

                      // Has wayland xdg popup?
                      if (pPatataEngineInfo->WindowInfo.info.wl.xdg_popup
                          != nullptr)
                        {
                          ImGui::TableNextRow ();

                          ImGui::TableSetColumnIndex (0);
#if defined(__GNUC__) || defined(__MINGW64__)
                          ImGui::Text (
                              "%s", abi::__cxa_demangle (
                                        typeid (pPatataEngineInfo->WindowInfo
                                                    .info.wl.xdg_popup)
                                            .name (),
                                        nullptr, nullptr, nullptr));
#else
                          ImGui::Text (typeid (pPatataEngineInfo->WindowInfo
                                                   .info.wl.xdg_popup)
                                           .name ());
#endif

                          ImGui::TableSetColumnIndex (1);
                          ImGui::Text ("Wayland xdg popup role");
                        }

                      // Has wayland egl window?
                      if (pPatataEngineInfo->WindowInfo.info.wl.egl_window
                          != nullptr)
                        {
                          ImGui::TableNextRow ();

                          ImGui::TableSetColumnIndex (0);
#if defined(__GNUC__) || defined(__MINGW64__)
                          ImGui::Text (
                              "%s", abi::__cxa_demangle (
                                        typeid (pPatataEngineInfo->WindowInfo
                                                    .info.wl.egl_window)
                                            .name (),
                                        nullptr, nullptr, nullptr));
#else
                          ImGui::Text (typeid (pPatataEngineInfo->WindowInfo
                                                   .info.wl.egl_window)
                                           .name ());
#endif

                          ImGui::TableSetColumnIndex (1);
                          ImGui::Text ("Wayland EGL window (native window)");
                        }
                    }
                  else if (pPatataEngineInfo->WindowInfo.subsystem
                           == SDL_SYSWM_X11)
                    {
#if defined(PATATA_LINUX_XORG_SUPPORT)
                      // X11 / Xorg
                      // Has Display?
                      if (pPatataEngineInfo->WindowInfo.info.x11.display
                          != nullptr)
                        {
                          ImGui::TableNextRow ();

                          ImGui::TableSetColumnIndex (0);
#if defined(__GNUC__) || defined(__MINGW64__)
                          ImGui::Text (
                              "%s", abi::__cxa_demangle (
                                        typeid (pPatataEngineInfo->WindowInfo
                                                    .info.x11.display)
                                            .name (),
                                        nullptr, nullptr, nullptr));
#else
                          ImGui::Text (typeid (pPatataEngineInfo->WindowInfo
                                                   .info.x11.display)
                                           .name ());
#endif

                          ImGui::TableSetColumnIndex (1);
                          ImGui::Text ("The X11 display");
                        }

                        // Has Window?
#if defined(PATATA_LINUX_XORG_SUPPORT)
                      if (pPatataEngineInfo->WindowInfo.info.x11.window)
                        {
                          ImGui::TableNextRow ();

                          ImGui::TableSetColumnIndex (0);
#if defined(__GNUC__) || defined(__MINGW64__)
                          ImGui::Text (
                              "%s", abi::__cxa_demangle (
                                        typeid (pPatataEngineInfo->WindowInfo
                                                    .info.x11.window)
                                            .name (),
                                        nullptr, nullptr, nullptr));
#else
                          ImGui::Text (typeid (pPatataEngineInfo->WindowInfo
                                                   .info.x11.window)
                                           .name ());
#endif

                          ImGui::TableSetColumnIndex (1);
                          ImGui::Text ("The X11 window");
                        }
#endif
#endif
                    }
#endif

#if defined(_WIN64) // Windows
                  if (pPatataEngineInfo->WindowInfo.subsystem
                      == SDL_SYSWM_WINDOWS)
                    {
                      if (pPatataEngineInfo->WindowInfo.info.win.window
                          != nullptr)
                        {
                          ImGui::TableNextRow ();

                          ImGui::TableSetColumnIndex (0);
#if defined(__GNUC__) || defined(__MINGW64__)
                          ImGui::Text (
                              "%s", abi::__cxa_demangle (
                                        typeid (pPatataEngineInfo->WindowInfo
                                                    .info.win.window)
                                            .name (),
                                        nullptr, nullptr, nullptr));
#else
                          ImGui::Text (typeid (pPatataEngineInfo->WindowInfo
                                                   .info.win.window)
                                           .name ());
#endif

                          ImGui::TableSetColumnIndex (1);
                          ImGui::Text ("The window handle");
                        }

                      if (pPatataEngineInfo->WindowInfo.info.win.hdc
                          != nullptr)
                        {
                          ImGui::TableNextRow ();

                          ImGui::TableSetColumnIndex (0);
#if defined(__GNUC__) || defined(__MINGW64__)
                          ImGui::Text (
                              "%s", abi::__cxa_demangle (
                                        typeid (pPatataEngineInfo->WindowInfo
                                                    .info.win.hdc)
                                            .name (),
                                        nullptr, nullptr, nullptr));
#else
                          ImGui::Text (
                              typeid (
                                  pPatataEngineInfo->WindowInfo.info.win.hdc)
                                  .name ());
#endif

                          ImGui::TableSetColumnIndex (1);
                          ImGui::Text ("The window device context");
                        }

                      if (pPatataEngineInfo->WindowInfo.info.win.hinstance
                          != nullptr)
                        {
                          ImGui::TableNextRow ();

                          ImGui::TableSetColumnIndex (0);
#if defined(__GNUC__) || defined(__MINGW64__)
                          ImGui::Text (
                              "%s", abi::__cxa_demangle (
                                        typeid (pPatataEngineInfo->WindowInfo
                                                    .info.win.hinstance)
                                            .name (),
                                        nullptr, nullptr, nullptr));
#else
                          ImGui::Text (typeid (pPatataEngineInfo->WindowInfo
                                                   .info.win.hinstance)
                                           .name ());
#endif

                          ImGui::TableSetColumnIndex (1);
                          ImGui::Text ("The instance handle");
                        }
                    }
#endif

                  ImGui::EndTable ();
                }
              ImGui::TreePop ();
            }

          // List of windows creation flags
          if (ImGui::TreeNode ("Creation flags##1"))
            {
              if (pPatataEngineInfo->WindowCreationFlags
                  & SDL_WINDOW_FULLSCREEN)
                ImGui::BulletText ("fullscreen window");

              if (pPatataEngineInfo->WindowCreationFlags & SDL_WINDOW_OPENGL)
                ImGui::BulletText ("window usable with OpenGL context");

              if (pPatataEngineInfo->WindowCreationFlags & SDL_WINDOW_SHOWN)
                ImGui::BulletText ("window is visible");

              if (pPatataEngineInfo->WindowCreationFlags & SDL_WINDOW_HIDDEN)
                ImGui::BulletText ("window is not visible");

              if (pPatataEngineInfo->WindowCreationFlags
                  & SDL_WINDOW_BORDERLESS)
                ImGui::BulletText ("no window decoration");

              if (pPatataEngineInfo->WindowCreationFlags
                  & SDL_WINDOW_RESIZABLE)
                ImGui::BulletText ("window can be resized");

              if (pPatataEngineInfo->WindowCreationFlags
                  & SDL_WINDOW_MINIMIZED)
                ImGui::BulletText ("window is minimized");

              if (pPatataEngineInfo->WindowCreationFlags
                  & SDL_WINDOW_MAXIMIZED)
                ImGui::BulletText ("window is maximized");

              if (pPatataEngineInfo->WindowCreationFlags
                  & SDL_WINDOW_MOUSE_GRABBED)
                ImGui::BulletText ("window has grabbed mouse input");

              if (pPatataEngineInfo->WindowCreationFlags
                  & SDL_WINDOW_INPUT_FOCUS)
                ImGui::BulletText ("window has input focus");

              if (pPatataEngineInfo->WindowCreationFlags
                  & SDL_WINDOW_MOUSE_FOCUS)
                ImGui::BulletText ("window has mouse focus");

              if (pPatataEngineInfo->WindowCreationFlags & SDL_WINDOW_FOREIGN)
                ImGui::BulletText ("window not created by SDL");

              if (pPatataEngineInfo->WindowCreationFlags
                  & SDL_WINDOW_ALLOW_HIGHDPI)
                ImGui::BulletText ("window should be created in "
                                   "high-DPI mode if supported");

              if (pPatataEngineInfo->WindowCreationFlags
                  & SDL_WINDOW_MOUSE_CAPTURE)
                ImGui::BulletText ("window has mouse captured "
                                   "(unrelated to MOUSE_GRABBED)");

              if (pPatataEngineInfo->WindowCreationFlags
                  & SDL_WINDOW_ALWAYS_ON_TOP)
                ImGui::BulletText ("window should always be above others");

              if (pPatataEngineInfo->WindowCreationFlags
                  & SDL_WINDOW_SKIP_TASKBAR)
                ImGui::BulletText (
                    "window should not be added to the taskbar");

              if (pPatataEngineInfo->WindowCreationFlags & SDL_WINDOW_UTILITY)
                ImGui::BulletText (
                    "window should be treated as a utility window");

              if (pPatataEngineInfo->WindowCreationFlags & SDL_WINDOW_TOOLTIP)
                ImGui::BulletText ("window should be treated as a tooltip");

              if (pPatataEngineInfo->WindowCreationFlags
                  & SDL_WINDOW_POPUP_MENU)
                ImGui::BulletText ("window should be treated as a popup menu");

              if (pPatataEngineInfo->WindowCreationFlags
                  & SDL_WINDOW_KEYBOARD_GRABBED)
                ImGui::BulletText ("window has grabbed keyboard input");

              if (pPatataEngineInfo->WindowCreationFlags & SDL_WINDOW_VULKAN)
                ImGui::BulletText ("window usable for Vulkan surface");

              if (pPatataEngineInfo->WindowCreationFlags & SDL_WINDOW_METAL)
                ImGui::BulletText ("window usable for Metal view");

              ImGui::TreePop ();
            }
          ImGui::Spacing ();
        }

      ImGui::Spacing ();
      ImGui::SeparatorText ("Raccoon Renderer");
      // vulkan
      if (ImGui::CollapsingHeader ("Vulkan"))
        {
          if (ImGui::TreeNode ("Version##1"))
            {
              if (ImGui::BeginTable ("Vulkan General Info", 2,
                                     PATATA_IMGUI_TABLE_FLAGS))
                {
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);

                  // Vulkan Loader Version
#if defined(PATATA_VULKAN_LOADER_VERSION)
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Loader");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (PATATA_VULKAN_LOADER_VERSION);
#endif

                  // Vulkan Headers Version
#if defined(PATATA_VULKAN_HEADERS_VERSION)
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Headers");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (PATATA_VULKAN_HEADERS_VERSION);
#endif

                  // VVL Version
#if defined(PATATA_VULKAN_VALIDATION_LAYERS_VERSION)
                  ImGui::TableNextRow ();

                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Validation Layers");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (PATATA_VULKAN_VALIDATION_LAYERS_VERSION);
#endif

                  if (!pPatataEngineInfo->VkVersionInUse.empty ())
                    {
                      ImGui::TableNextRow ();

                      ImGui::TableSetColumnIndex (0);
                      ImGui::Text ("Vulkan");

                      ImGui::TableSetColumnIndex (1);
                      ImGui::Text ("%s",
                                   pPatataEngineInfo->VkVersionInUse.c_str ());
                    }
                  ImGui::EndTable ();
                }
              ImGui::TreePop ();
            }

          if (ImGui::TreeNode ("Extensions and layers"))
            {
#if defined(PATATA_USE_VVL)
              if (ImGui::BeginTable ("Layers##VkLayerList", 1,
                                     PATATA_IMGUI_TABLE_FLAGS))
                {
                  ImGui::TableSetupColumn ("Layer",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableHeadersRow ();

                  for (uint16_t i = 0; i < PATATA_VK_LAYER_COUNT; ++i)
                    {
                      ImGui::TableNextRow ();

                      ImGui::TableSetColumnIndex (0);
                      ImGui::Text ("%s", pPatataEngineInfo->VkLayers[i]);
                    }

                  ImGui::EndTable ();
                }
#endif

              if (ImGui::BeginTable ("Instance##VkInstanceList", 1,
                                     PATATA_IMGUI_TABLE_FLAGS))
                {
                  ImGui::TableSetupColumn ("Instance",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableHeadersRow ();

                  for (uint16_t i = 0;
                       i < pPatataEngineInfo->VkInstanceExtensionsCount; ++i)
                    {
                      ImGui::TableNextRow ();

                      ImGui::TableSetColumnIndex (0);
                      ImGui::Text ("%s",
                                   pPatataEngineInfo->VkInstanceExtensions[i]);
                    }
                  ImGui::EndTable ();
                }

              ImGui::Spacing ();

              if (ImGui::BeginTable ("Device##VkDeviceList", 1,
                                     PATATA_IMGUI_TABLE_FLAGS))
                {
                  ImGui::TableSetupColumn ("Device",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableHeadersRow ();

                  for (uint32_t i = 0;
                       i < pPatataEngineInfo->VkDeviceExtensionsCount; ++i)
                    {
                      ImGui::TableNextRow ();

                      ImGui::TableSetColumnIndex (0);
                      ImGui::Text ("%s",
                                   pPatataEngineInfo->VkDeviceExtensions[i]);
                    }
                  ImGui::EndTable ();
                }
              ImGui::TreePop ();
            }

          if (ImGui::TreeNode ("Device"))
            {
              if (!pPatataEngineInfo->VkDeviceName.empty ())
                {
                  switch (pPatataEngineInfo->VkDeviceVendorId)
                    {
                    case 32902:
                      ImGui::TextColored (
                          ImVec4 (PATATA_IMGUI_INTEL_COLOR), "%s",
                          pPatataEngineInfo->VkDeviceName.c_str ());
                      break;
                    case 4098:
                      ImGui::TextColored (
                          ImVec4 (PATATA_IMGUI_AMD_COLOR), "%s",
                          pPatataEngineInfo->VkDeviceName.c_str ());
                      break;
                    case 4318:
                      ImGui::TextColored (
                          ImVec4 (PATATA_IMGUI_NVIDIA_COLOR), "%s",
                          pPatataEngineInfo->VkDeviceName.c_str ());
                      break;
                    case 65541:
                      ImGui::TextColored (
                          ImVec4 (PATATA_IMGUI_MESA_COLOR), "Mesa %s",
                          pPatataEngineInfo->VkDeviceName.c_str ());
                      break;
                    default:
                      ImGui::Text ("Unknown");
                      break;
                    }
                }

              if (ImGui::BeginTable ("Vulkan Device Info##1", 2,
                                     PATATA_IMGUI_TABLE_FLAGS))
                {
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);

                  if (pPatataEngineInfo->VkDeviceVendorId != 0)
                    {
                      ImGui::TableNextRow ();

                      ImGui::TableSetColumnIndex (0);
                      ImGui::Text ("Vendor");

                      ImGui::TableSetColumnIndex (1);
                      switch (pPatataEngineInfo->VkDeviceVendorId)
                        {
                        case 32902:
                          ImGui::TextColored (
                              ImVec4 (PATATA_IMGUI_INTEL_COLOR), "Intel");
                          break;
                        case 4098:
                          ImGui::TextColored (ImVec4 (PATATA_IMGUI_AMD_COLOR),
                                              "AMD");
                          break;
                        case 4318:
                          ImGui::TextColored (
                              ImVec4 (PATATA_IMGUI_NVIDIA_COLOR), "Nvidia");
                          break;
                        case 65541:
                          ImGui::TextColored (ImVec4 (PATATA_IMGUI_MESA_COLOR),
                                              "Mesa");
                          break;
                        default:
                          ImGui::TextColored (
                              ImVec4 (PATATA_IMGUI_UNKNOWN_VENDOR_COLOR),
                              "Unknown");
                          break;
                        }

                      ImGui::TableNextRow ();

                      ImGui::TableSetColumnIndex (0);
                      ImGui::Text ("Vendor Id");

                      ImGui::TableSetColumnIndex (1);
                      switch (pPatataEngineInfo->VkDeviceVendorId)
                        {
                        case 32902:
                          ImGui::TextColored (
                              ImVec4 (PATATA_IMGUI_INTEL_COLOR), "%u  0x%X",
                              pPatataEngineInfo->VkDeviceVendorId,
                              pPatataEngineInfo->VkDeviceVendorId);
                          break;
                        case 4098:
                          ImGui::TextColored (
                              ImVec4 (PATATA_IMGUI_AMD_COLOR), "%u  0x%X",
                              pPatataEngineInfo->VkDeviceVendorId,
                              pPatataEngineInfo->VkDeviceVendorId);
                          break;
                        case 4318:
                          ImGui::TextColored (
                              ImVec4 (PATATA_IMGUI_NVIDIA_COLOR), "%u  0x%X",
                              pPatataEngineInfo->VkDeviceVendorId,
                              pPatataEngineInfo->VkDeviceVendorId);
                          break;
                        case 65541:
                          ImGui::TextColored (
                              ImVec4 (PATATA_IMGUI_MESA_COLOR), "%u  0x%X",
                              pPatataEngineInfo->VkDeviceVendorId,
                              pPatataEngineInfo->VkDeviceVendorId);
                          break;
                        default:
                          ImGui::TextColored (
                              ImVec4 (PATATA_IMGUI_UNKNOWN_VENDOR_COLOR),
                              "%u  0x%X", pPatataEngineInfo->VkDeviceVendorId,
                              pPatataEngineInfo->VkDeviceVendorId);
                          break;
                        }
                    }

                  if (!pPatataEngineInfo->VkDeviceType.empty ())
                    {
                      ImGui::TableNextRow ();

                      ImGui::TableSetColumnIndex (0);
                      ImGui::Text ("Type");

                      ImGui::TableSetColumnIndex (1);
                      ImGui::Text ("%s",
                                   pPatataEngineInfo->VkDeviceType.c_str ());
                    }

                  ImGui::EndTable ();
                }
              ImGui::TreePop ();
            }

          if (ImGui::TreeNode ("Driver"))
            {
              if (!pPatataEngineInfo->VkDriverName.empty ())
                {
                  ImGui::Text ("%s", pPatataEngineInfo->VkDriverName.c_str ());
                }

              if (ImGui::BeginTable ("Vulkan Driver Info##1", 2,
                                     PATATA_IMGUI_TABLE_FLAGS))
                {
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);

                  if (!pPatataEngineInfo->VkDriverId.empty ())
                    {
                      ImGui::TableNextRow ();

                      ImGui::TableSetColumnIndex (0);
                      ImGui::Text ("Id");

                      ImGui::TableSetColumnIndex (1);
                      ImGui::Text ("%s",
                                   pPatataEngineInfo->VkDriverId.c_str ());
                    }

                  if (!pPatataEngineInfo->VkDriverInfo.empty ())
                    {
                      ImGui::TableNextRow ();

                      ImGui::TableSetColumnIndex (0);
                      ImGui::Text ("Info");

                      ImGui::TableSetColumnIndex (1);
                      ImGui::Text ("%s",
                                   pPatataEngineInfo->VkDriverInfo.c_str ());
                    }

                  if (!pPatataEngineInfo->VkDriverVersion.empty ())
                    {
                      ImGui::TableNextRow ();

                      ImGui::TableSetColumnIndex (0);
                      ImGui::Text ("Version");

                      ImGui::TableSetColumnIndex (1);
                      ImGui::Text (
                          "%s", pPatataEngineInfo->VkDriverVersion.c_str ());
                    }

                  ImGui::EndTable ();
                }
              ImGui::TreePop ();
            }

          if (ImGui::TreeNode ("SwapChain"))
            {
              if (ImGui::BeginTable ("Vulkan SwapChain Info##1", 2,
                                     PATATA_IMGUI_TABLE_FLAGS))
                {
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);
                  ImGui::TableSetupColumn ("",
                                           ImGuiTableColumnFlags_WidthFixed);

                  ImGui::TableNextRow ();

                  // Present Mode
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Present Mode");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (
                      "%s",
                      vk::to_string (pPatataEngineInfo->VkSwapchainPresentMode)
                          .c_str ());

                  if ((pPatataEngineInfo->VkSwapchainPresentMode
                           == vk::PresentModeKHR::eFifo
                       || pPatataEngineInfo->VkSwapchainPresentMode
                              == vk::PresentModeKHR::eFifoRelaxed
                       || pPatataEngineInfo->VkSwapchainPresentMode
                              == vk::PresentModeKHR::eSharedContinuousRefresh)
                      && (pConfiguration->Vsync))
                    ImGui::TextColored (ImVec4 (PATATA_IMGUI_POSITIVE_VALUE),
                                        "Vertical Sync");
                  else if ((pPatataEngineInfo->VkSwapchainPresentMode
                                == vk::PresentModeKHR::eMailbox
                            || pPatataEngineInfo->VkSwapchainPresentMode
                                   == vk::PresentModeKHR::eImmediate
                            || pPatataEngineInfo->VkSwapchainPresentMode
                                   == vk::PresentModeKHR::eSharedDemandRefresh)
                           && (!pConfiguration->Vsync))
                    ImGui::TextColored (
                        ImVec4 (PATATA_IMGUI_WARNING_VALUE),
                        "Not synchronized with the refresh rate");

                  ImGui::TableNextRow ();

                  // Images
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Images");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text ("%u", SwapChainImageCount);

                  ImGui::TableNextRow ();

                  // Drawable Size
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Drawable Size");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text ("%u x %u", SwapChainExtent.width,
                               SwapChainExtent.height);

                  ImGui::TableNextRow ();

                  // Image Color Format
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Image Color Format");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (
                      "%s", vk::to_string (
                                pPatataEngineInfo->VkSwapchainImageColorFormat)
                                .c_str ());

                  ImGui::TableNextRow ();

                  // Image Color Space
                  ImGui::TableSetColumnIndex (0);
                  ImGui::Text ("Image Color Space");

                  ImGui::TableSetColumnIndex (1);
                  ImGui::Text (
                      "%s", vk::to_string (
                                pPatataEngineInfo->VkSwapchainImageColorSpace)
                                .c_str ());

                  ImGui::EndTable ();
                }
              ImGui::TreePop ();
            }
        }

      /*
      ImGui::Spacing();
      ImGui::SeparatorText("Raccoon Sound");
      ImGui::Text("Actual Backend in use");
      if (ImGui::CollapsingHeader("'Audio Backend Name'")) {

      }*/

      ImGui::PopStyleVar ();

      ImGui::End ();
    }
  // End Patata Window Info

  // Patata Window Configuration
  if (pPatataEngineInfo->PatataConfigWindow)
    {
      ImGui::Begin ("Configuration", &pPatataEngineInfo->PatataConfigWindow);
      if (ImGui::Checkbox ("Show Fatal Error Messagebox",
                           &pConfiguration->ShowFatalErrorMessagebox))
        {
          pConfiguration->TriggeredChange
              = &pConfiguration->ShowFatalErrorMessagebox;
        }

#if defined(__linux__)
      if (ImGui::Checkbox ("Prefer Wayland", &pConfiguration->PreferWayland))
        {
          pConfiguration->TriggeredChange = &pConfiguration->PreferWayland;
        }
#endif

      if (ImGui::Checkbox ("Vsync", &pConfiguration->Vsync))
        {
          pConfiguration->TriggeredChange = &pConfiguration->Vsync;
        }

      if (ImGui::Checkbox ("10 Bit Depth", &pConfiguration->BitDepth10))
        {
          pConfiguration->TriggeredChange = &pConfiguration->BitDepth10;
        }
      ImGui::End ();
    }
  // End Patata Window Configuration

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
