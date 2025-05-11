#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
#include <cxxabi.h>
#endif
#if PATACHE_DEBUG == 1
#include <cstdio>
#include <future>
#include <cstring>
#endif

#include <fast_io.h>
#include <SDL3/SDL.h>
#if PATACHE_DEBUG == 1
#include <imgui_impl_vulkan.h>
#endif
#if defined(_WIN64)
#include <windows.h>
#endif

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "ColorTerminal.hpp"
#include "Log.hpp"
#include "fast_io_buff.hpp"
#include "CstringWrapped.hpp"

void VulkanInfo (Patache::Engine * const, const Patache::SwapChainInfo &);
