#include <fast_io.h>
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include "PatacheEngine/VmaUsage.hpp"

// Patache Engine
#include "ColorTerminal.hpp"
#include "FastIOBuff.hpp"

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
Patache::VulkanCheck (const char * const pMessage, const VkResult & rResult)
{
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, PATACHE_TERM_BOLD,
                        fast_io::mnp::os_c_str (pMessage), " : ", PATACHE_TERM_RESET,
                        (rResult != VK_SUCCESS && rResult != VK_SUBOPTIMAL_KHR)
                            ? PATACHE_TERM_COLOR_YELLOW
                            : PATACHE_TERM_COLOR_GREEN,
                        fast_io::mnp::os_c_str (string_VkResult (rResult)), PATACHE_TERM_RESET);
}
