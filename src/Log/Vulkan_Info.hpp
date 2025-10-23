#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
  #include <cxxabi.h>
#endif
#if PATACHE_DEBUG == 1
  #include <cstdio>
  #include <future>
  #include <cstring>
#endif

#include <fast_io.h>
#include <vulkan/vulkan.hpp>
#include "PatacheEngine/VmaUsage.hpp"
#include <SDL3/SDL.h>
#if PATACHE_DEBUG == 1
  #include <imgui_impl_vulkan.h>
#endif

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "ColorTerminal.hpp"
#include "FastIOBuff.hpp"
#include "LogMacroUtils.hpp"
#include "CstringWrapped.hpp"

void VulkanInfo (Patache::Engine * const, const Patache::SwapchainInfo &);
