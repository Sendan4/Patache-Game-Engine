#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include "PatacheEngine/VmaUsage.hpp"
#include <SDL3/SDL.h>
#include <imgui.h>

#include "PatacheEngine/PatacheEngine.hpp"
#include "StartLogInfo.hpp"

// Custom ImGui Styles
// Vulkan Vendor
#define PATACHE_IMGUI_INTEL_COLOR          0.314F, 0.759F, 0.950F, 1.0F
#define PATACHE_IMGUI_AMD_COLOR            1.0F, 0.396F, 0.396F, 1.0F
#define PATACHE_IMGUI_NVIDIA_COLOR         0.463F, 0.725F, 0.0F, 1.0F
#define PATACHE_IMGUI_MESA_COLOR           0.844F, 0.867F, 0.870F, 1.0F
#define PATACHE_IMGUI_UNKNOWN_VENDOR_COLOR 0.640F, 0.646F, 0.646F, 1.0F

// table
#define PATACHE_IMGUI_TABLE_PADDING     10, 4
#define PATACHE_IMGUI_TITLE_TABLE_COLOR 0.930F, 0.718F, 0.223F, 1.0F
#define PATACHE_IMGUI_TABLE_FLAGS                                                                  \
  ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersH                 \
      | ImGuiTableFlags_BordersV

// Error, Warning and positive values
#define PATACHE_IMGUI_POSITIVE_VALUE 0.634F, 0.990F, 0.657F, 1.0F
#define PATACHE_IMGUI_WARNING_VALUE  0.984F, 0.990F, 0.634F, 1.0F
#define PATACHE_IMGUI_ERROR_VALUE    0.990F, 0.669F, 0.634F, 1.0F

// Accent Color
#define PATACHE_IMGUI_PATACHE_COLOR  0.596F, 0.463F, 0.278F, 1.0F
#define PATACHE_IMGUI_SELECTED_COLOR 0.868F, 0.663F, 0.380F, 1.0F
#define PATACHE_IMGUI_DARKER_COLOR   0.353F, 0.267F, 0.153F, 1.0F

// Collapse Header color
#define PATACHE_IMGUI_INFO_COLOR          0.341F, 0.341F, 0.341F, 1.0F
#define PATACHE_IMGUI_INFO_DARKER_COLOR   0.200F, 0.200F, 0.200F, 1.0F
#define PATACHE_IMGUI_INFO_SELECTED_COLOR 0.404F, 0.404F, 0.404F, 1.0F

#define PATACHE_IMGUI_CORNERBORDER_COLOR 1.0F, 1.0F, 1.0F, 1.0F
#define PATACHE_IMGUI_FRAMEBG_COLOR                                                                \
  0.21176470588235294F, 0.1803921568627451F, 0.12549019607843137F, 1.0F
#define PATACHE_IMGUI_FRAMEBG_HOVERED_COLOR                                                        \
  0.36470588235294116F, 0.28627450980392155F, 0.1450980392156863F, 1.0F
#define PATACHE_IMGUI_FRAMEBG_ACTIVE_COLOR                                                         \
  0.5372549019607843F, 0.4235294117647059F, 0.21568627450980393F, 1.0F
#define PATACHE_IMGUI_CHECKMARK_COLOR                                                              \
  0.9647058823529412F, 0.7568627450980392F, 0.3803921568627451F, 1.0F
#define PATACHE_IMGUI_SLIDERGRAB_COLOR                                                             \
  0.8156862745098039F, 0.6392156862745098F, 0.3176470588235294F, 1.0F
#define PATACHE_IMGUI_SLIDERGRABACTIVE_COLOR                                                       \
  0.8941176470588236F, 0.6980392156862745F, 0.3411764705882353F, 1.0F
#define PATACHE_IMGUI_NONE 0.0F, 0.0F, 0.0F, 1.0F

namespace Patache
{
void DrawDebugUI (Patache::Engine * const);
}
