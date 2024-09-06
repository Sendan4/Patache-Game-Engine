// All internal functions that will be used publicly must be wrapped
// Engine Implementation (Wrapping) -> Public API (Wrapping)

#include <vulkan/vulkan.hpp>
#include "PatataEngineImpl.hpp"
#include "PatataEngine/PatataEngine.hpp"

// Public API
Patata::Engine::Engine (const std::string & WindowTitle)
    : pPatataEngine (new EngineImpl (WindowTitle))
{
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

void
Patata::Engine::BeginRender (SDL_Event & Event)
{
  pPatataEngine->BeginRender (Event);
}

void
Patata::Engine::EndRender (SDL_Event & Event)
{
  pPatataEngine->EndRender (Event);
}

void
Patata::Engine::ClearColor (const float & R, const float & G, const float & B, const float & A)
{
  pPatataEngine->ClearColor (R, G, B, A);
}
