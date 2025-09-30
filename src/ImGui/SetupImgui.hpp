#include <future>
#include <functional>

#if PATACHE_DEBUG == 1
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>
#include <imgui_freetype.h>
#endif
// #include <fast_io.h>
#include <SDL3/SDL.h>

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "Message.hpp"
#include "Vulkan_SetupLog.hpp"

#include "InterDisplay-Medium.hpp"

void InitImGuiCore (const Patache::Config &, Patache::EngineInfo &);
bool InitImGuiVulkan (Patache::Engine * const);
bool CreateImguiDescriptorPool (Patache::VulkanBackend &);
bool CreateImguiPipelineCache (Patache::VulkanBackend &);
