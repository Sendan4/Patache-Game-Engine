#include <string>
#include <future>

#include <fast_io.h>
#include <fast_io_device.h>
#include <ryml.hpp>
#include <SDL.h>

// Patata Engine
#include "PatataEngine/PatataEngine.hpp"
#include "Log.hpp"

#if PATATA_GAME_NAME != 0
#define GAME_CONFIG_FILE_NAME PATATA_GAME_NAME ".yaml"
#else
#define GAME_CONFIG_FILE_NAME "patata.yaml"
#endif
