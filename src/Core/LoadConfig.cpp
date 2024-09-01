#include "LoadConfig.hpp"

bool
Patata::Engine::EngineImpl::LoadConfig (Patata::Config & Config) {
    YAML::Node ConfigurationSaved;

    try
      {
        ConfigurationSaved = YAML::LoadFile (strcpy (SDL_GetBasePath (), GAME_CONFIG_FILE_NAME));
      }
    catch (const YAML::BadFile BadFile)
      {
        Patata::Log::YamlFileErrorMessage ();
        return false;
      }

    // General
    Config.ShowFatalErrorMessagebox = ConfigurationSaved["patata-engine"]["show-fatal-error-messagebox"].as<bool>();

    // Specific Linux
    #if defined(__linux__)
    Config.PreferWayland = ConfigurationSaved["patata-engine"]["prefer-wayland"].as<bool>();
    Config.EnableMangoHud = ConfigurationSaved["patata-engine"]["enable-mangohud"].as<bool>();
    #endif

    // Raccoon Renderer
    Config.Vsync = ConfigurationSaved["patata-engine"]["raccoon-renderer"]["vsync"].as<bool>();
    Config.BitDepth10 = ConfigurationSaved["patata-engine"]["raccoon-renderer"]["10bit-depth"].as<bool>();

    return true;
}
