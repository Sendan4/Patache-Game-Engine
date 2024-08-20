#include <yaml-cpp/yaml.h>
#include <SDL.h>

#include "Config.hpp"
#include "PatataEngineImpl.hpp"
#include "Log.hpp"

#if defined(PATATA_GAME_NAME)
#define GAME_CONFIG_FILE_NAME PATATA_GAME_NAME ".yaml"
#else
#define GAME_CONFIG_FILE_NAME "patata.yaml"
#endif
