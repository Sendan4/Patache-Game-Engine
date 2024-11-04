// All internal functions that will be used publicly must be wrapped
// Engine Implementation (Wrapping) -> Public API (Wrapping)

#include <vulkan/vulkan.hpp>
#include "PatataEngineImpl.hpp"
#include "PatataEngine/PatataEngine.hpp"

// Public API
Patata::Engine::Engine (const std::string & WindowTitle)
    : pPatataEngine (new EngineImpl (WindowTitle))
{
    pClearColor = &pPatataEngine->ClearColor;
}

Patata::Engine::~Engine ()
{
  delete pPatataEngine;
  pPatataEngine = nullptr;
}

void
Patata::Engine::HandleEvent (SDL_Event & Event)
{
  pPatataEngine->HandleEvent (Event);
}

bool
Patata::Engine::BeginRender (SDL_Event & Event)
{
  return pPatataEngine->BeginRender (Event);
}

void
Patata::Engine::EndRender (SDL_Event & Event)
{
  pPatataEngine->EndRender (Event);
}
