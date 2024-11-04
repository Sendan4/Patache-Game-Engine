// Raccoon Renderer Functions (Wrapping) -> Engine Implementation (Wrapping)
// Patata Engine
#include "RenderWrapper.hpp"

bool
Patata::Engine::EngineImpl::BeginRender (SDL_Event & Event)
{
	return RaccoonRenderer->BeginRender(Event);
}

void
Patata::Engine::EngineImpl::EndRender (SDL_Event & Event)
{
	RaccoonRenderer->EndRender(Event);
}
