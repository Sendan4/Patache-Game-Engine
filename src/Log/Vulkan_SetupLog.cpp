#include "Vulkan_SetupLog.hpp"

void
Patache::VulkanList (const char * const pList[], const std::uint32_t & rSize,
                     const char * const pMessage)
{
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                        fast_io::mnp::os_c_str (pMessage), " : ", PATACHE_TERM_RESET, rSize);

  for (std::uint32_t i = 0; i < rSize; ++i)
    {
      fast_io::io::println (PATACHE_FASTIO_BUFFOUT, "  ", PATACHE_TERM_DIM,
                            fast_io::mnp::os_c_str (pList[i]), PATACHE_TERM_RESET);
    }

  fast_io::io::println (PATACHE_FASTIO_BUFFOUT);
}

void
Patache::VulkanCheck (const char * const pMessage, const vk::Result & rResult)
{
  if (rResult != vk::Result::eSuccess && rResult != vk::Result::eSuboptimalKHR)
    {
      fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                            fast_io::mnp::os_c_str (pMessage), " : ", PATACHE_TERM_RESET,
                            PATACHE_TERM_COLOR_YELLOW, vk::to_string (rResult), PATACHE_TERM_RESET);
    }
  else
    {
      fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                            fast_io::mnp::os_c_str (pMessage), " : ", PATACHE_TERM_RESET,
                            PATACHE_TERM_COLOR_GREEN, vk::to_string (rResult), PATACHE_TERM_RESET);
    }
}
