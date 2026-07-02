#ifndef VMAUSAGE_HPP
#define VMAUSAGE_HPP
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#define VMA_STATIC_VULKAN_FUNCTIONS  0
#if defined(_MSC_VER)
  #pragma warning(push)
  #pragma warning(disable : 4100)
  #pragma warning(disable : 4189)
  #pragma warning(disable : 4127)
  #pragma warning(disable : 4324)
  #include <vk_mem_alloc.h>
  #pragma warning(pop)
#else
  #pragma GCC diagnostic push

  #if __GNUC__
    #if __clang__
      #pragma GCC diagnostic ignored "-Wnullability-extension"
      #pragma GCC diagnostic ignored "-Wunused-private-field"
      #pragma GCC diagnostic ignored "-Wnullability-completeness"
    #endif
  #endif

  #pragma GCC diagnostic ignored "-Wunused-parameter"
  #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
  #pragma GCC diagnostic ignored "-Wunused-variable"
  #include <vk_mem_alloc.h>
  #pragma GCC diagnostic pop
#endif
#endif
