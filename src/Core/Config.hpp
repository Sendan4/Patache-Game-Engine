#pragma once

namespace Patata {
    struct Config {
        // General
        bool ShowFatalErrorMessagebox;

        // Specific Linux
        #if defined(__linux__)
        bool PreferWayland;
        bool EnableMangoHud;
        #endif

        // Raccoon Renderer
        bool Vsync;
        bool BitDepth10;
    };
}
