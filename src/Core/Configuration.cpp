#include "Configuration.hpp"

bool
Patata::Engine::EngineImpl::LoadConfig (Patata::Config & Config) {
    fast_io::dir_file ExecutableDirectory(static_cast<std::string>(SDL_GetBasePath ()));
    fast_io::native_file_loader ConfigFile(at(ExecutableDirectory), GAME_CONFIG_FILE_NAME);

    // .data() to get char *
    ryml::Tree TreeConfig = ryml::parse_in_place(ConfigFile.data());
    ryml::ConstNodeRef YamlConfig = TreeConfig.crootref();

    if (YamlConfig.invalid() || !YamlConfig.readable()) {
        std::future<void> Err = std::async(std::launch::async,
            Patata::Log::ErrorMessage,
            "Yaml Config File is invalid or inreadable");
    }
    else {
        // General
        // Bool Type
        if (YamlConfig["patata-engine"]["show-fatal-error-messagebox"].val() == "true"
            || YamlConfig["patata-engine"]["show-fatal-error-messagebox"].val() == "false"
            || YamlConfig["patata-engine"]["show-fatal-error-messagebox"].val().is_number() == true
            || YamlConfig["patata-engine"]["show-fatal-error-messagebox"].val().is_number() == false)
            YamlConfig["patata-engine"]["show-fatal-error-messagebox"] >> Config.ShowFatalErrorMessagebox;
        else {
            std::future<void> Err = std::async (
                std::launch::async, Patata::Log::ErrorMessage,
                std::string (
                    "in YAML configure file ("
                    + static_cast<std::string> (SDL_GetBasePath ())
                    + GAME_CONFIG_FILE_NAME
                    + ")\n'patata-engine : show-fatal-error-messagebox' must "
                      "be a boolean value (True or False)"));
        }

        // Specific Linux
        #if defined(__linux__)
        // Bool Type
        if (YamlConfig["patata-engine"]["prefer-wayland"].val() == "true"
            || YamlConfig["patata-engine"]["prefer-wayland"].val() == "false"
            || YamlConfig["patata-engine"]["prefer-wayland"].val().is_number() == true
            || YamlConfig["patata-engine"]["prefer-wayland"].val().is_number() == false)
            YamlConfig["patata-engine"]["prefer-wayland"] >> Config.PreferWayland;
        else {
            std::future<void> Err = std::async (
                std::launch::async, Patata::Log::ErrorMessage,
                std::string (
                    "in YAML configure file ("
                    + static_cast<std::string> (SDL_GetBasePath ())
                    + GAME_CONFIG_FILE_NAME
                    + ")\n'patata-engine : prefer-wayland' must "
                      "be a boolean value (True or False)"));
        }

        // Bool Type
        if (YamlConfig["patata-engine"]["enable-mangohud"].val() == "true"
            || YamlConfig["patata-engine"]["enable-mangohud"].val() == "false"
            || YamlConfig["patata-engine"]["enable-mangohud"].val().is_number() == true
            || YamlConfig["patata-engine"]["enable-mangohud"].val().is_number() == false)
            YamlConfig["patata-engine"]["enable-mangohud"] >> Config.EnableMangoHud;
        else {
            std::future<void> Err = std::async (
                std::launch::async, Patata::Log::ErrorMessage,
                std::string (
                    "in YAML configure file ("
                    + static_cast<std::string> (SDL_GetBasePath ())
                    + GAME_CONFIG_FILE_NAME
                    + ")\n'patata-engine : enable-mangohud' must "
                      "be a boolean value (True or False)"));
        }
        #endif

        // Raccoon Renderer
        // Bool Type
        if (YamlConfig["patata-engine"]["raccoon-renderer"]["vsync"].val() == "true"
            || YamlConfig["patata-engine"]["raccoon-renderer"]["vsync"].val() == "false"
            || YamlConfig["patata-engine"]["raccoon-renderer"]["vsync"].val().is_number() == true
            || YamlConfig["patata-engine"]["raccoon-renderer"]["vsync"].val().is_number() == false)
            YamlConfig["patata-engine"]["raccoon-renderer"]["vsync"] >> Config.Vsync;
        else {
            std::future<void> Err = std::async (
                std::launch::async, Patata::Log::ErrorMessage,
                std::string (
                    "in YAML configure file ("
                    + static_cast<std::string> (SDL_GetBasePath ())
                    + GAME_CONFIG_FILE_NAME
                    + ")\n'patata-engine : raccoon-renderer : vsync' must "
                      "be a boolean value (True or False)"));
        }

        // Bool Type
        if (YamlConfig["patata-engine"]["raccoon-renderer"]["10bit-depth"].val() == "true"
            || YamlConfig["patata-engine"]["raccoon-renderer"]["10bit-depth"].val() == "false"
            || YamlConfig["patata-engine"]["raccoon-renderer"]["10bit-depth"].val().is_number() == true
            || YamlConfig["patata-engine"]["raccoon-renderer"]["10bit-depth"].val().is_number() == false)
            YamlConfig["patata-engine"]["raccoon-renderer"]["10bit-depth"] >> Config.BitDepth10;
        else {
            std::future<void> Err = std::async (
                std::launch::async, Patata::Log::ErrorMessage,
                std::string (
                    "in YAML configure file ("
                    + static_cast<std::string> (SDL_GetBasePath ())
                    + GAME_CONFIG_FILE_NAME
                    + ")\n'patata-engine : raccoon-renderer : 10bit-depth' must "
                      "be a boolean value (True or False)"));
        }
    }

    return true;
}
