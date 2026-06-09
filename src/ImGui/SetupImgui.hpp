#ifndef SETUPIMGUI_HPP
#define SETUPIMGUI_HPP

namespace Patache
{
void InitImGuiCore (const Patache::Config &, Patache::EngineInfo &);
bool InitImGuiVulkan (Patache::Engine * const);
bool CreateImguiDescriptorPool (Patache::VulkanBackend &);
bool CreateImguiPipelineCache (Patache::VulkanBackend &);
}

#endif
