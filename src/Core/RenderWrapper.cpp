// Raccoon Renderer Functions (Wrapping) -> Engine Implementation (Wrapping)
// Patata Engine
#include "RenderWrapper.hpp"

void
Patata::Engine::EngineImpl::BeginRender (void)
{
	RaccoonRenderer->BeginRender();
}

void
Patata::Engine::EngineImpl::EndRender (void)
{
	RaccoonRenderer->EndRender();
}

void
Patata::Engine::EngineImpl::ClearColor (const float & R, const float & G, const float & B, const float & A)
{
	RaccoonRenderer->ClearColor(R, G, B, A);
}
