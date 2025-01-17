#if defined(__GNUC__) || defined(__MINGW64__)
#include <cxxabi.h>
#endif
#include <future>
#include <cstdint>

#include <SDL.h>
#if PATATA_DEBUG == 1
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>
#endif

#include "PatataEngine/PatataEngine.hpp"
#include "Start_Patata_Log_Info.hpp"
#include "Log.hpp"
