#include <cstdio>

#include <SDL3/SDL.h>
#include <fast_io.h>
#include <fast_io_device.h>
#if __clang__
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wnan-infinity-disabled"
  #include <ryml.hpp>
  #pragma clang diagnostic pop
#else
  #include <ryml.hpp>
#endif

// Patache Engine
#include "PatacheEngine/StructConfig.hpp"
#include "Message.hpp"

namespace Patache
{
static constexpr std::uint32_t errorTextSize{ 1024U };
static constexpr std::uint32_t errorTextSizeExtraNull{ 10245 };
}

#include "Configuration.hpp"

bool
Patache::LoadConfiguration (Patache::Config & rConfiguration)
{
  fast_io::dir_file           executableDirectory (fast_io::mnp::os_c_str (SDL_GetBasePath ()));
  fast_io::native_file_loader yamlConfigFile (at (executableDirectory), "config.yaml");

  ryml::Tree         tree       = ryml::parse_in_place (yamlConfigFile.data ());
  ryml::ConstNodeRef yamlConfig = tree.crootref ();

  // Invalid or unreadable Yaml file
  if (yamlConfig.invalid () || !yamlConfig.readable ())
    {
      Patache::FatalErrorMessage ("Patache Engine - Yaml",
                                  "Yaml Config File is invalid or inreadable", rConfiguration);

      return false;
    }
  else
    {
      // General
      // Bool Type
      if (yamlConfig["patache-engine"]["show-fatal-error-messagebox"].val () == "true"
          || yamlConfig["patache-engine"]["show-fatal-error-messagebox"].val () == "false"
          || yamlConfig["patache-engine"]["show-fatal-error-messagebox"].val () == "1"
          || yamlConfig["patache-engine"]["show-fatal-error-messagebox"].val () == "0")
        {
          yamlConfig["patache-engine"]["show-fatal-error-messagebox"]
              >> rConfiguration.showFatalErrorMessagebox;
        }
      else
        {
          char errorText[Patache::errorTextSizeExtraNull]{};

          std::snprintf (errorText, Patache::errorTextSize,
                         "in YAML configure file (%s/config.yaml)\n'patache-engine : "
                         "show-fatal-error-messagebox' must "
                         "be a boolean value (True or False)",
                         SDL_GetBasePath ());

          Patache::ErrorMessage (errorText);
        }

      // Bool Type
      // Vsync
      if (yamlConfig["patache-engine"]["raccoon-renderer"]["vsync"].val () == "true"
          || yamlConfig["patache-engine"]["raccoon-renderer"]["vsync"].val () == "false"
          || yamlConfig["patache-engine"]["raccoon-renderer"]["vsync"].val () == "1"
          || yamlConfig["patache-engine"]["raccoon-renderer"]["vsync"].val () == "0")
        {
          yamlConfig["patache-engine"]["raccoon-renderer"]["vsync"] >> rConfiguration.vsync;
        }
      else
        {
          char errorText[Patache::errorTextSizeExtraNull]{};

          std::snprintf (errorText, Patache::errorTextSize,
                         "in YAML configure file (%s/config.yaml)\n'patache-engine : "
                         "raccoon-renderer : vsync' must "
                         "be a boolean value (True or False)",
                         SDL_GetBasePath ());

          Patache::ErrorMessage (errorText);
        }

      // Bool Type
      // 10 Bit Depth
      if (yamlConfig["patache-engine"]["raccoon-renderer"]["10bit-depth"].val () == "true"
          || yamlConfig["patache-engine"]["raccoon-renderer"]["10bit-depth"].val () == "false"
          || yamlConfig["patache-engine"]["raccoon-renderer"]["10bit-depth"].val () == "1"
          || yamlConfig["patache-engine"]["raccoon-renderer"]["10bit-depth"].val () == "0")
        {
          yamlConfig["patache-engine"]["raccoon-renderer"]["10bit-depth"]
              >> rConfiguration.bitDepth10;
        }
      else
        {
          char errorText[Patache::errorTextSizeExtraNull]{};

          std::snprintf (errorText, Patache::errorTextSize,
                         "in YAML configure file (%s/config.yaml)\n'patache-engine : "
                         "raccoon-renderer : 10bit-depth' must "
                         "be a boolean value (True or False)",
                         SDL_GetBasePath ());

          Patache::ErrorMessage (errorText);
        }

      // std::uint8_t type
      // Add Image Count
      if (!ryml::overflows<std::uint8_t> (
              yamlConfig["patache-engine"]["raccoon-renderer"]["add-image-count"].val ())
          && yamlConfig["patache-engine"]["raccoon-renderer"]["add-image-count"]
                 .val ()
                 .is_integer ())
        {
          yamlConfig["patache-engine"]["raccoon-renderer"]["add-image-count"]
              >> rConfiguration.addImageCount;
        }
      else
        {
          char errorText[Patache::errorTextSizeExtraNull]{};

          std::snprintf (errorText, Patache::errorTextSize,
                         "in YAML configure file (%s/config.yaml)\n'patache-engine : "
                         "raccoon-renderer : add-image-count' must "
                         "be a 8bit unsigned interger (0 - 4)",
                         SDL_GetBasePath ());

          Patache::ErrorMessage (errorText);
        }

      if (rConfiguration.addImageCount > 4)
        {
          char errorText[Patache::errorTextSizeExtraNull]{};

          std::snprintf (errorText, Patache::errorTextSize,
                         "in YAML configure file (%s/config.yaml)\n'patache-engine : "
                         "raccoon-renderer : add-image-count' The allowed "
                         "range is (0 - 4)",
                         SDL_GetBasePath ());

          Patache::ErrorMessage (errorText);

          rConfiguration.addImageCount = 0;
        }
    }

  return true;
}
