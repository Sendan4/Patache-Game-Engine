#include "Configuration.hpp"

bool
LoadConfiguration (Patache::Config & configuration)
{
  fast_io::dir_file ExecutableDirectory (
      fast_io::mnp::os_c_str (SDL_GetBasePath ()));
  fast_io::native_file_loader YamlConfigFile (at (ExecutableDirectory),
                                              "config.yaml");

  // .data() to get char *
  ryml::Tree TreeConfig = ryml::parse_in_place (YamlConfigFile.data ());
  ryml::ConstNodeRef YamlConfig = TreeConfig.crootref ();

  // Invalid or unreadable Yaml file
  if (YamlConfig.invalid () || !YamlConfig.readable ())
    {
      std::future<void> Err = std::async (
          std::launch::async, Patache::Log::FatalErrorMessage,
          "Patache Engine - Yaml", "Yaml Config File is invalid or inreadable",
          configuration);

      return false;
    }
  else
    {
      // General
      // Bool Type
      if (YamlConfig["patache-engine"]["show-fatal-error-messagebox"].val ()
              == "true"
          || YamlConfig["patache-engine"]["show-fatal-error-messagebox"].val ()
                 == "false"
          || YamlConfig["patache-engine"]["show-fatal-error-messagebox"].val ()
                 == "1"
          || YamlConfig["patache-engine"]["show-fatal-error-messagebox"].val ()
                 == "0")
        {
          YamlConfig["patache-engine"]["show-fatal-error-messagebox"]
              >> configuration.ShowFatalErrorMessagebox;
        }
      else
        {
          char ErrorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (
              ErrorText, PATACHE_ERROR_TEXT_SIZE - 1,
              "in YAML configure file (%s/config.yaml)\n'patache-engine : "
              "show-fatal-error-messagebox' must "
              "be a boolean value (True or False)",
              SDL_GetBasePath ());

          std::future<void> Err = std::async (
              std::launch::async, Patache::Log::ErrorMessage, ErrorText);
        }

      // Raccoon Renderer
      // Bool Type
      // Vsync
      if (YamlConfig["patache-engine"]["raccoon-renderer"]["vsync"].val ()
              == "true"
          || YamlConfig["patache-engine"]["raccoon-renderer"]["vsync"].val ()
                 == "false"
          || YamlConfig["patache-engine"]["raccoon-renderer"]["vsync"].val ()
                 == "1"
          || YamlConfig["patache-engine"]["raccoon-renderer"]["vsync"].val ()
                 == "0")
        {
          YamlConfig["patache-engine"]["raccoon-renderer"]["vsync"]
              >> configuration.Vsync;
        }
      else
        {
          char ErrorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (
              ErrorText, PATACHE_ERROR_TEXT_SIZE - 1,
              "in YAML configure file (%s/config.yaml)\n'patache-engine : "
              "raccoon-renderer : vsync' must "
              "be a boolean value (True or False)",
              SDL_GetBasePath ());

          std::future<void> Err = std::async (
              std::launch::async, Patache::Log::ErrorMessage, ErrorText);
        }

      // Bool Type
      // 10 Bit Depth
      if (YamlConfig["patache-engine"]["raccoon-renderer"]["10bit-depth"]
                  .val ()
              == "true"
          || YamlConfig["patache-engine"]["raccoon-renderer"]["10bit-depth"]
                     .val ()
                 == "false"
          || YamlConfig["patache-engine"]["raccoon-renderer"]["10bit-depth"]
                     .val ()
                 == "1"
          || YamlConfig["patache-engine"]["raccoon-renderer"]["10bit-depth"]
                     .val ()
                 == "0")
        {
          YamlConfig["patache-engine"]["raccoon-renderer"]["10bit-depth"]
              >> configuration.BitDepth10;
        }
      else
        {
          char ErrorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (
              ErrorText, PATACHE_ERROR_TEXT_SIZE - 1,
              "in YAML configure file (%s/config.yaml)\n'patache-engine : "
              "raccoon-renderer : 10bit-depth' must "
              "be a boolean value (True or False)",
              SDL_GetBasePath ());

          std::future<void> Err = std::async (
              std::launch::async, Patache::Log::ErrorMessage, ErrorText);
        }

      // std::uint8_t type
      // Add Image Count
      if (!ryml::overflows<std::uint8_t> (
              YamlConfig["patache-engine"]["raccoon-renderer"]
                        ["add-image-count"]
                            .val ())
          && YamlConfig["patache-engine"]["raccoon-renderer"]
                       ["add-image-count"]
                           .val ()
                           .is_integer ())
        {
          YamlConfig["patache-engine"]["raccoon-renderer"]["add-image-count"]
              >> configuration.AddImageCount;
        }
      else
        {
          char ErrorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (
              ErrorText, PATACHE_ERROR_TEXT_SIZE - 1,
              "in YAML configure file (%s/config.yaml)\n'patache-engine : "
              "raccoon-renderer : add-image-count' must "
              "be a 8bit unsigned interger (0 - 4)",
              SDL_GetBasePath ());

          std::future<void> Err = std::async (
              std::launch::async, Patache::Log::ErrorMessage, ErrorText);
        }

      if (configuration.AddImageCount > 4)
        {
          char ErrorText[PATACHE_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (
              ErrorText, PATACHE_ERROR_TEXT_SIZE - 1,
              "in YAML configure file (%s/config.yaml)\n'patache-engine : "
              "raccoon-renderer : add-image-count' The allowed "
              "range is (0 - 4)",
              SDL_GetBasePath ());

          std::future<void> Err = std::async (
              std::launch::async, Patache::Log::ErrorMessage, ErrorText);

          configuration.AddImageCount = 0;
        }
    }

  return true;
}
