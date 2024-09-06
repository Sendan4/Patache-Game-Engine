// Raccoon Renderer Functions (Wrapping) -> Engine Implementation (Wrapping)
// Patata Engine
#include "RenderWrapper.hpp"

void
Patata::Engine::EngineImpl::BeginRender (SDL_Event & Event)
{
	RaccoonRenderer->BeginRender(Event);
}

void
Patata::Engine::EngineImpl::EndRender (SDL_Event & Event)
{
	RaccoonRenderer->EndRender(Event);
}

void
Patata::Engine::EngineImpl::ClearColor (const float & R, const float & G, const float & B, const float & A)
{
	RaccoonRenderer->ClearColor(R, G, B, A);
}
