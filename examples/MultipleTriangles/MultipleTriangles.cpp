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

  patache.ClearColor (Patache::Color::eBlack);

  Patache::Triangle exampleTriangle[8]{ Patache::Triangle (), Patache::Triangle (),
                                        Patache::Triangle (), Patache::Triangle (),
                                        Patache::Triangle (), Patache::Triangle (),
                                        Patache::Triangle (), Patache::Triangle () };

  exampleTriangle[0].SetColorRGB (0, 0.0F, 0.0F, 1.0F);
  exampleTriangle[0].SetColorRGB (1, 0.0F, 0.0F, 1.0F);
  exampleTriangle[0].SetColorRGB (2, 0.0F, 0.0F, 1.0F);
  exampleTriangle[0].vertex[0].pos[0] = -0.8F;
  exampleTriangle[0].vertex[0].pos[1] = -1.0F;
  exampleTriangle[0].vertex[1].pos[0] = -0.6F;
  exampleTriangle[0].vertex[1].pos[1] = -0.6F;
  exampleTriangle[0].vertex[2].pos[0] = -1.0F;
  exampleTriangle[0].vertex[2].pos[1] = -0.6F;

  exampleTriangle[1].SetColorRGB (0, 0.0F, 1.0F, 0.0F);
  exampleTriangle[1].SetColorRGB (1, 0.0F, 1.0F, 0.0F);
  exampleTriangle[1].SetColorRGB (2, 0.0F, 1.0F, 0.0F);
  exampleTriangle[1].vertex[0].pos[0] = -0.6F;
  exampleTriangle[1].vertex[0].pos[1] = -0.8F;
  exampleTriangle[1].vertex[1].pos[0] = -0.4F;
  exampleTriangle[1].vertex[1].pos[1] = -0.4F;
  exampleTriangle[1].vertex[2].pos[0] = -0.8F;
  exampleTriangle[1].vertex[2].pos[1] = -0.4F;

  exampleTriangle[2].SetColorRGB (0, 1.0F, 0.0F, 0.0F);
  exampleTriangle[2].SetColorRGB (1, 1.0F, 0.0F, 0.0F);
  exampleTriangle[2].SetColorRGB (2, 1.0F, 0.0F, 0.0F);
  exampleTriangle[2].vertex[0].pos[0] = -0.4F;
  exampleTriangle[2].vertex[0].pos[1] = -0.6F;
  exampleTriangle[2].vertex[1].pos[0] = -0.2F;
  exampleTriangle[2].vertex[1].pos[1] = -0.2F;
  exampleTriangle[2].vertex[2].pos[0] = -0.6F;
  exampleTriangle[2].vertex[2].pos[1] = -0.2F;

  exampleTriangle[3].SetColorRGB (0, 1.0F, 1.0F, 0.0F);
  exampleTriangle[3].SetColorRGB (1, 1.0F, 1.0F, 0.0F);
  exampleTriangle[3].SetColorRGB (2, 1.0F, 1.0F, 0.0F);
  exampleTriangle[3].vertex[0].pos[0] = -0.2F;
  exampleTriangle[3].vertex[0].pos[1] = -0.4F;
  exampleTriangle[3].vertex[1].pos[0] = 0.0F;
  exampleTriangle[3].vertex[1].pos[1] = 0.0F;
  exampleTriangle[3].vertex[2].pos[0] = -0.4F;
  exampleTriangle[3].vertex[2].pos[1] = 0.0F;

  exampleTriangle[4].SetColorRGB (0, 1.0F, 0.0F, 1.0F);
  exampleTriangle[4].SetColorRGB (1, 1.0F, 0.0F, 1.0F);
  exampleTriangle[4].SetColorRGB (2, 1.0F, 0.0F, 1.0F);
  exampleTriangle[4].vertex[0].pos[0] = 0.0F;
  exampleTriangle[4].vertex[0].pos[1] = -0.2F;
  exampleTriangle[4].vertex[1].pos[0] = 0.2F;
  exampleTriangle[4].vertex[1].pos[1] = 0.2F;
  exampleTriangle[4].vertex[2].pos[0] = -0.2F;
  exampleTriangle[4].vertex[2].pos[1] = 0.2F;

  exampleTriangle[5].SetColorRGB (0, 0.0F, 1.0F, 1.0F);
  exampleTriangle[5].SetColorRGB (1, 0.0F, 1.0F, 1.0F);
  exampleTriangle[5].SetColorRGB (2, 0.0F, 1.0F, 1.0F);
  exampleTriangle[5].vertex[0].pos[0] = 0.2F;
  exampleTriangle[5].vertex[0].pos[1] = 0.0F;
  exampleTriangle[5].vertex[1].pos[0] = 0.4F;
  exampleTriangle[5].vertex[1].pos[1] = 0.4F;
  exampleTriangle[5].vertex[2].pos[0] = 0.0F;
  exampleTriangle[5].vertex[2].pos[1] = 0.4F;

  exampleTriangle[6].SetColorRGB (0, 0.490F, 0.253F, 0.0833F);
  exampleTriangle[6].SetColorRGB (1, 0.490F, 0.253F, 0.0833F);
  exampleTriangle[6].SetColorRGB (2, 0.490F, 0.253F, 0.0833F);
  exampleTriangle[6].vertex[0].pos[0] = 0.4F;
  exampleTriangle[6].vertex[0].pos[1] = 0.2F;
  exampleTriangle[6].vertex[1].pos[0] = 0.6F;
  exampleTriangle[6].vertex[1].pos[1] = 0.6F;
  exampleTriangle[6].vertex[2].pos[0] = 0.2F;
  exampleTriangle[6].vertex[2].pos[1] = 0.6F;

  exampleTriangle[7].SetColorRGB (0, 0.494F, 0.0392F, 0.980F);
  exampleTriangle[7].SetColorRGB (1, 0.494F, 0.0392F, 0.980F);
  exampleTriangle[7].SetColorRGB (2, 0.494F, 0.0392F, 0.980F);
  exampleTriangle[7].vertex[0].pos[0] = 0.6F;
  exampleTriangle[7].vertex[0].pos[1] = 0.4F;
  exampleTriangle[7].vertex[1].pos[0] = 0.8F;
  exampleTriangle[7].vertex[1].pos[1] = 0.8F;
  exampleTriangle[7].vertex[2].pos[0] = 0.4F;
  exampleTriangle[7].vertex[2].pos[1] = 0.8F;

  for (std::uint8_t i{ 0U }; i < 8; ++i)
    {
      patache.BufferSubAlloc (&exampleTriangle[i]);
    }

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
          for (std::uint8_t i{ 0U }; i < 8; ++i)
            {
              patache.Draw (&exampleTriangle[i]);
            }

          patache.EndRender (event);
        }
    }
FINISHLOOP:

  for (std::uint8_t i{ 0U }; i < 8; ++i)
    {
      patache.BufferSubFree (&exampleTriangle[i]);
    }

  return 0;
}
