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
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, fast_io::mnp::os_c_str (Patache::sTermBold),
                        fast_io::mnp::os_c_str (pMessage), " : ",
                        fast_io::mnp::os_c_str (Patache::sTermReset), rSize);

  for (std::uint32_t i{ 0U }; i < rSize; ++i)
    {
      fast_io::io::println (
          PATACHE_FASTIO_BUFFOUT, "  ", fast_io::mnp::os_c_str (Patache::sTermDim),
          fast_io::mnp::os_c_str (pList[i]), fast_io::mnp::os_c_str (Patache::sTermReset));
    }

  fast_io::io::println (PATACHE_FASTIO_BUFFOUT);
}

void
Patache::VulkanCheck (const char * const pMessage, const VkResult & rResult)
{
  fast_io::io::println (PATACHE_FASTIO_BUFFOUT, fast_io::mnp::os_c_str (Patache::sTermBold),
                        fast_io::mnp::os_c_str (pMessage), " : ",
                        fast_io::mnp::os_c_str (Patache::sTermReset),
                        (rResult != VK_SUCCESS && rResult != VK_SUBOPTIMAL_KHR)
                            ? fast_io::mnp::os_c_str (Patache::sTermColorYellow)
                            : fast_io::mnp::os_c_str (Patache::sTermColorGreen),
                        fast_io::mnp::os_c_str (string_VkResult (rResult)),
                        fast_io::mnp::os_c_str (Patache::sTermReset));
}
