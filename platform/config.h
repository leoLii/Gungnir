#pragma once

#include "common/base.h"

#include <string>

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace app {

struct Config {
    std::string title;
    std::string iblDirectory;
    std::string dirt;
    float scale = 1.0f;
    bool splitView = false;
    //mutable filament::Engine::Backend backend = filament::Engine::Backend::DEFAULT;
    //filament::camutils::Mode cameraMode = filament::camutils::Mode::ORBIT;
    bool resizeable = true;
    bool headless = false;
};

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
