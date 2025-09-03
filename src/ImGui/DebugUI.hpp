#include <SDL3/SDL.h>

#include "PatacheEngine/PatacheEngine.hpp"
#include "StartLogInfo.hpp"
// Custom ImGui Styles
// Vulkan Vendor
#define PATACHE_IMGUI_INTEL_COLOR          0.314f, 0.759f, 0.950f, 1.0f
#define PATACHE_IMGUI_AMD_COLOR            1.0f, 0.396f, 0.396f, 1.0f
#define PATACHE_IMGUI_NVIDIA_COLOR         0.463f, 0.725f, 0.0f, 1.0f
#define PATACHE_IMGUI_MESA_COLOR           0.844f, 0.867f, 0.870f, 1.0f
#define PATACHE_IMGUI_UNKNOWN_VENDOR_COLOR 0.640f, 0.646f, 0.646f, 1.0f

// table
#define PATACHE_IMGUI_TABLE_PADDING     10, 4
#define PATACHE_IMGUI_TITLE_TABLE_COLOR 0.930f, 0.718f, 0.223f, 1.0f
#define PATACHE_IMGUI_TABLE_FLAGS                                             \
  ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_RowBg                       \
      | ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV

// Error, Warning and positive values
#define PATACHE_IMGUI_POSITIVE_VALUE 0.634f, 0.990f, 0.657f, 1.0f
#define PATACHE_IMGUI_WARNING_VALUE  0.984f, 0.990f, 0.634f, 1.0f
#define PATACHE_IMGUI_ERROR_VALUE    0.990f, 0.669f, 0.634f, 1.0f

// Accent Color
#define PATACHE_IMGUI_PATACHE_COLOR  0.596f, 0.463f, 0.278f, 1.0f
#define PATACHE_IMGUI_SELECTED_COLOR 0.868f, 0.663f, 0.380f, 1.0f
#define PATACHE_IMGUI_DARKER_COLOR   0.353f, 0.267f, 0.153f, 1.0f

// Collapse Header color
#define PATACHE_IMGUI_INFO_COLOR          0.341f, 0.341f, 0.341f, 1.0f
#define PATACHE_IMGUI_INFO_DARKER_COLOR   0.200f, 0.200f, 0.200f, 1.0f
#define PATACHE_IMGUI_INFO_SELECTED_COLOR 0.404f, 0.404f, 0.404f, 1.0f

#define PATACHE_IMGUI_CORNERBORDER_COLOR 1.0f, 1.0f, 1.0f, 1.0f
#define PATACHE_IMGUI_FRAMEBG_COLOR                                           \
  0.21176470588235294f, 0.1803921568627451f, 0.12549019607843137f, 1.0f
#define PATACHE_IMGUI_FRAMEBG_HOVERED_COLOR                                   \
  0.36470588235294116f, 0.28627450980392155f, 0.1450980392156863f, 1.0f
#define PATACHE_IMGUI_FRAMEBG_ACTIVE_COLOR                                    \
  0.5372549019607843f, 0.4235294117647059f, 0.21568627450980393f, 1.0f
#define PATACHE_IMGUI_CHECKMARK_COLOR                                         \
  0.9647058823529412f, 0.7568627450980392f, 0.3803921568627451f, 1.0f
#define PATACHE_IMGUI_SLIDERGRAB_COLOR                                        \
  0.8156862745098039f, 0.6392156862745098f, 0.3176470588235294f, 1.0f
#define PATACHE_IMGUI_SLIDERGRABACTIVE_COLOR                                  \
  0.8941176470588236f, 0.6980392156862745f, 0.3411764705882353f, 1.0f
#define PATACHE_IMGUI_NONE 0.0f, 0.0f, 0.0f, 1.0f

namespace Patache
{
void DrawDebugUI (Patache::Engine * const engine);
}
