// All internal functions that will be used publicly must be wrapped
// Engine Implementation (Wrapping) -> Public API (Wrapping)

#include <SDL.h>

#include "PatataEngine/PatataEngine.hpp"
#include "PatataEngineImpl.hpp"

// Public API
Patata::Engine::Engine (const std::string & WindowTitle,
                        const uint32_t & Width, const uint32_t & Height)
    : pPatataEngine (new EngineImpl (WindowTitle, Width, Height))
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
Patata::Engine::BeginRender (void)
{
  pPatataEngine->BeginRender ();
}

void
Patata::Engine::EndRender (void)
{
  pPatataEngine->EndRender ();
}

void
Patata::Engine::ClearColor (const float & R, const float & G, const float & B, const float & A)
{
  pPatataEngine->ClearColor (R, G, B, A);
}
