#pragma once

#ifndef YAML_CPP_API
#define YAML_CPP_API
#endif
#include <SDL.h>
#include <yaml-cpp/yaml.h>

#include "Config.hpp"
#if defined(DEBUG)
#include "EngineInfo.hpp"
#endif

namespace Patata
{
namespace Log
{
void StartMapache (void);
void StartPatataLogInfo (void);
#if defined(DEBUG)
void WindowLog (SDL_Window *, Patata::EngineInfo &);
#else
void WindowLog (SDL_Window *);
#endif

// Vulkan Setup
void VulkanCheck (const std::string &, const vk::Result &);
void VulkanList (const char * List[], const size_t &, const std::string &);

// Errors
void YamlFileErrorMessage (void);
void FatalErrorMessage (const std::string &, const std::string &, const Patata::Config &);
void ErrorMessage (const std::string &);
void WarningMessage (const std::string &);
}
}
