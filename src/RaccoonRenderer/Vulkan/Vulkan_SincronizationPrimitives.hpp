#include <cstdio>
#include <future>
#include <functional>

#include <SDL3/SDL.h>

// Patache Engine
#include "PatacheEngine/PatacheEngine.hpp"
#include "Log.hpp"

bool CreateSemaphore (Patache::VulkanBackend &);
bool CreateFence (Patache::VulkanBackend &);

#define PATACHE_ERROR_TEXT_SIZE 64
