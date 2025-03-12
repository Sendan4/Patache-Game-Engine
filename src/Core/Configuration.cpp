#include "Configuration.hpp"

bool
Patata::Engine::LoadConfiguration (void)
{
  fast_io::dir_file ExecutableDirectory (
      static_cast<std::string> (SDL_GetBasePath ()));
  fast_io::native_file_loader YamlConfigFile (at (ExecutableDirectory),
                                              "config.yaml");

  // .data() to get char *
  ryml::Tree TreeConfig = ryml::parse_in_place (YamlConfigFile.data ());
  ryml::ConstNodeRef YamlConfig = TreeConfig.crootref ();

  // Invalid or unreadable Yaml file
  if (YamlConfig.invalid () || !YamlConfig.readable ())
    {
      std::future<void> Err
          = std::async (std::launch::async, Patata::Log::ErrorMessage,
                        "Yaml Config File is invalid or inreadable");
    }
  else
    {
      // General
      // Bool Type
      if (YamlConfig["patata-engine"]["show-fatal-error-messagebox"].val ()
              == "true"
          || YamlConfig["patata-engine"]["show-fatal-error-messagebox"].val ()
                 == "false"
          || YamlConfig["patata-engine"]["show-fatal-error-messagebox"].val ()
                 == "1"
          || YamlConfig["patata-engine"]["show-fatal-error-messagebox"].val ()
                 == "0")
        YamlConfig["patata-engine"]["show-fatal-error-messagebox"]
            >> configuration.ShowFatalErrorMessagebox;
      else
        {
          char ErrorText[PATATA_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (ErrorText, PATATA_ERROR_TEXT_SIZE - 1,
                         "in YAML configure file (%s%s)\n'patata-engine : "
                         "show-fatal-error-messagebox' must "
                         "be a boolean value (True or False)",
                         SDL_GetBasePath (), GAME_CONFIG_FILE_NAME);

          std::future<void> Err = std::async (
              std::launch::async, Patata::Log::ErrorMessage, ErrorText);
        }

      // Raccoon Renderer
      // Bool Type
      // Vsync
      if (YamlConfig["patata-engine"]["raccoon-renderer"]["vsync"].val ()
              == "true"
          || YamlConfig["patata-engine"]["raccoon-renderer"]["vsync"].val ()
                 == "false"
          || YamlConfig["patata-engine"]["raccoon-renderer"]["vsync"].val ()
                 == "1"
          || YamlConfig["patata-engine"]["raccoon-renderer"]["vsync"].val ()
                 == "0")
        YamlConfig["patata-engine"]["raccoon-renderer"]["vsync"]
            >> configuration.Vsync;
      else
        {
          char ErrorText[PATATA_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (ErrorText, PATATA_ERROR_TEXT_SIZE - 1,
                         "in YAML configure file (%s%s)\n'patata-engine : "
                         "raccoon-renderer : vsync' must "
                         "be a boolean value (True or False)",
                         SDL_GetBasePath (), GAME_CONFIG_FILE_NAME);

          std::future<void> Err = std::async (
              std::launch::async, Patata::Log::ErrorMessage, ErrorText);
        }

      // Bool Type
      // 10 Bit Depth
      if (YamlConfig["patata-engine"]["raccoon-renderer"]["10bit-depth"].val ()
              == "true"
          || YamlConfig["patata-engine"]["raccoon-renderer"]["10bit-depth"]
                     .val ()
                 == "false"
          || YamlConfig["patata-engine"]["raccoon-renderer"]["10bit-depth"]
                     .val ()
                 == "1"
          || YamlConfig["patata-engine"]["raccoon-renderer"]["10bit-depth"]
                     .val ()
                 == "0")
        YamlConfig["patata-engine"]["raccoon-renderer"]["10bit-depth"]
            >> configuration.BitDepth10;
      else
        {
          char ErrorText[PATATA_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (ErrorText, PATATA_ERROR_TEXT_SIZE - 1,
                         "in YAML configure file (%s%s)\n'patata-engine : "
                         "raccoon-renderer : 10bit-depth' must "
                         "be a boolean value (True or False)",
                         SDL_GetBasePath (), GAME_CONFIG_FILE_NAME);

          std::future<void> Err = std::async (
              std::launch::async, Patata::Log::ErrorMessage, ErrorText);
        }

      // std::uint8_t type
      // Add Image Count
      if (!ryml::overflows<std::uint8_t> (
              YamlConfig["patata-engine"]["raccoon-renderer"]
                        ["add-image-count"]
                            .val ())
          && YamlConfig["patata-engine"]["raccoon-renderer"]["add-image-count"]
                 .val ()
                 .is_integer ())
        YamlConfig["patata-engine"]["raccoon-renderer"]["add-image-count"]
            >> configuration.AddImageCount;
      else
        {
          char ErrorText[PATATA_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (ErrorText, PATATA_ERROR_TEXT_SIZE - 1,
                         "in YAML configure file (%s%s)\n'patata-engine : "
                         "raccoon-renderer : add-image-count' must "
                         "be a 8bit unsigned interger (0 - 4)",
                         SDL_GetBasePath (), GAME_CONFIG_FILE_NAME);

          std::future<void> Err = std::async (
              std::launch::async, Patata::Log::ErrorMessage, ErrorText);
        }

      if (configuration.AddImageCount > 4)
        {
          char ErrorText[PATATA_ERROR_TEXT_SIZE]{ 0 };

          std::snprintf (ErrorText, PATATA_ERROR_TEXT_SIZE - 1,
                         "in YAML configure file (%s%s)\n'patata-engine : "
                         "raccoon-renderer : add-image-count' The allowed "
                         "range is (0 - 4)",
                         SDL_GetBasePath (), GAME_CONFIG_FILE_NAME);

          std::future<void> Err = std::async (
              std::launch::async, Patata::Log::ErrorMessage, ErrorText);

          configuration.AddImageCount = 0;
        }
    }

  return true;
}
