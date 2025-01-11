#if defined(__GNUC__) || defined(__MINGW64__)
#include <cxxabi.h>
#endif
#include <future>
#include <cstdint>
#include <string>

#include <fast_io.h>
#include <vulkan/vulkan.hpp>
#include <SDL.h>
#include <SDL_vulkan.h>
#if PATATA_DEBUG == 1
#include <SDL_syswm.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>
#endif

#include "Start_Patata_Log_Info.hpp"
#if PATATA_DEBUG == 1
#include "StructEngineInfo.hpp"
#endif
#include "PatataEngine/StructClearColor.hpp"
#include "StructConfig.hpp"
#include "VulkanBackend.hpp"
#include "RaccoonRenderer.hpp"
#include "ColorTerminal.hpp"
#include "Log.hpp"
