#if defined(__GNUC__) || defined(__MINGW64__) && !defined(__clang__)
#include <cxxabi.h>
#endif

#include <fast_io.h>
#include <vulkan/vulkan.hpp>
#include <SDL.h>
#include <SDL_vulkan.h>
#if PATATA_DEBUG == 1
#include <SDL_syswm.h>
#include <imgui_impl_vulkan.h>
#endif
#if defined(_WIN64)
#include <windows.h>
#endif

#if PATATA_DEBUG == 1
#include "StructEngineInfo.hpp"
#endif
#include "PatataEngine/StructClearColor.hpp"
#include "StructConfig.hpp"
#include "VulkanBackend.hpp"
#include "RaccoonRenderer.hpp"
#include "ColorTerminal.hpp"
#include "Log.hpp"
