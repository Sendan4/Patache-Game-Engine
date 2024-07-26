#include "ImGUISetupBackend.hpp"

void
Patata::Engine::EngineImpl::SetupImGUIBackend (void)
{
  IMGUI_CHECKVERSION ();
  ImGui::CreateContext ();
  ImGuiIO & io = ImGui::GetIO ();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.IniSavingRate = 0;
  io.IniFilename   = nullptr;
  io.LogFilename   = nullptr;

  ImGui::StyleColorsDark ();
  ImGuiStyle * style     = &ImGui::GetStyle ();
  style->FrameBorderSize = 0;
}
