#include <future>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#if PATATA_DEBUG == 1
#include <imgui_impl_vulkan.h>
#endif

// Patata Engine
#include "PatataEngine/PatataEngine.hpp"
#include "ColorTerminal.hpp"
#include "Log.hpp"
