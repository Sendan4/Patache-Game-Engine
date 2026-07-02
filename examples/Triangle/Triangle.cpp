#include <vulkan/vulkan.h>
#define VMA_IMPLEMENTATION
#include <PatacheEngine/VmaUsage.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <PatacheEngine/PatacheEngine.hpp>

int
main ([[maybe_unused]] int argc, [[maybe_unused]] char * argv[])
{
  constexpr Patache::EngineCreateInfo info{};
  bool                                result{ false };

  Patache::Engine patache (info, &result);

  if (!result)
    return -1;

  patache.ClearColor (Patache::Color::eRed);

  Patache::Triangle exampleTriangle{ Patache::Triangle () };

  patache.BufferSubAlloc (&exampleTriangle);

  while (true)
    {
      SDL_Event event{};

      while (SDL_PollEvent (&event))
        {
          // Patache Events
          patache.HandleEvent (event);

          // Your Events
          if (event.type == SDL_EVENT_QUIT)
            {
              goto FINISHLOOP;
            }
        }

      if (patache.BeginRender (event))
        {
          patache.Draw (&exampleTriangle);
          patache.EndRender (event);
        }
    }
FINISHLOOP:

  patache.BufferSubFree (&exampleTriangle);

  return 0;
}
