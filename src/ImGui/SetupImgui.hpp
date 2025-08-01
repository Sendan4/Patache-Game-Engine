#include <future>
#include <functional>

#include <fast_io.h>
#if PATACHE_DEBUG == 1
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>
#endif
#include <SDL3/SDL.h>

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "ColorTerminal.hpp"
#include "Log.hpp"
#include "fast_io_buff.hpp"

void InitImgui (const Patache::Config &);
bool InitImguiVulkan (Patache::Engine * const);
bool CreateImguiDescriptorPool (Patache::VulkanBackend &);
bool CreateImguiPipelineCache (Patache::VulkanBackend &);
