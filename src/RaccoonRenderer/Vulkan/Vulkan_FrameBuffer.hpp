#include <cstdio>
#include <future>

#include <SDL3/SDL.h>

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "Log.hpp"

bool CreateFrameBuffer (Patache::VulkanBackend &);

#define PATACHE_ERROR_TEXT_SIZE 64
