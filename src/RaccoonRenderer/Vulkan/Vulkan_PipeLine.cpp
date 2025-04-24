#include "Vulkan_PipeLine.hpp"

bool
Patata::Engine::CreatePipeline (void)
{
  // Load Shaders
  char ShaderDirectory_str[2056]{ 0 };

  PATATA_STRNCPY (ShaderDirectory_str, SDL_GetBasePath (), 2055);
  PATATA_STRNCAT (ShaderDirectory_str, "Shaders/", 2055);

  fast_io::dir_file ShaderDirectory (ShaderDirectory_str);

  fast_io::native_file_loader VertexShader (at (ShaderDirectory),
                                            "Triangle.vert.spv");

  fast_io::native_file_loader FragmentShader (at (ShaderDirectory),
                                              "Triangle.frag.spv");

  if (VertexShader.data () == nullptr)
    return false;

  if (FragmentShader.data () == nullptr)
    return false;

  vk::ShaderModule VertexShaderModule{ VK_NULL_HANDLE },
      FragmentShaderModule{ VK_NULL_HANDLE };

  // Create Shader Modules
  vk::ShaderModuleCreateInfo ShaderModuleInfo[2]{
    { .codeSize = VertexShader.size (),
      .pCode
      = reinterpret_cast<const std::uint32_t *> (VertexShader.data ()) },
    { .codeSize = FragmentShader.size (),
      .pCode
      = reinterpret_cast<const std::uint32_t *> (FragmentShader.data ()) }
  };

  // Vertex Shader Module
  vk::Result Result = Vulkan.Device.createShaderModule (
      &ShaderModuleInfo[0], nullptr, &VertexShaderModule);

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Vertex Shader Module", Result);

      return false;
    }

  // Fragment Shader Module
  Result = Vulkan.Device.createShaderModule (&ShaderModuleInfo[1], nullptr,
                                             &FragmentShaderModule);

  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Fragment Shader Module", Result);

      return false;
    }

  // Create Pipeline
  vk::PipelineShaderStageCreateInfo ShaderStageInfo[2]{
    { .stage  = vk::ShaderStageFlagBits::eVertex,
      .module = VertexShaderModule,
      .pName  = "main" },
    { .stage  = vk::ShaderStageFlagBits::eFragment,
      .module = FragmentShaderModule,
      .pName  = "main" }
  };

  // Vertex Input & Input Assembly
  /*Vertex Triangle{};

  vk::VertexInputBindingDescription BindingDescription{
    .binding   = 0,
    .stride    = static_cast<std::uint32_t> (sizeof (Vertex)),
    .inputRate = vk::VertexInputRate::eVertex
  };

  vk::VertexInputAttributeDescription VertexInputAttrib[2]{
    // Position
    { .location = 0,
      .binding  = 0,
      .format   = vk::Format::eR32G32Sfloat,
      .offset   = offsetof (Vertex, pos) },
    // Color
    { .location = 1,
      .binding  = 0,
      .format   = vk::Format::eR32G32B32Sfloat,
      .offset   = offsetof (Vertex, color) }
  };*/

  const vk::PipelineVertexInputStateCreateInfo VertexInputStateInfo{
    .vertexBindingDescriptionCount   = 0,
    .pVertexBindingDescriptions      = nullptr,
    .vertexAttributeDescriptionCount = 0,
    .pVertexAttributeDescriptions    = nullptr
  };

  constexpr const vk::PipelineInputAssemblyStateCreateInfo
      InputAssemblyStateInfo{ .topology = vk::PrimitiveTopology::eTriangleList,
                              .primitiveRestartEnable = VK_FALSE };

  // Dynamic State
  constexpr const vk::DynamicState DynamicStates[2]{
    vk::DynamicState::eViewport, vk::DynamicState::eScissor
  };

  const vk::PipelineDynamicStateCreateInfo DynamicStateInfo{
    .dynamicStateCount = 2, .pDynamicStates = DynamicStates
  };

  // Viewport and Scissor
  Vulkan.Viewport.x      = 0;
  Vulkan.Viewport.y      = 0;
  Vulkan.Viewport.width  = static_cast<float> (Vulkan.SwapChainExtent.width);
  Vulkan.Viewport.height = static_cast<float> (Vulkan.SwapChainExtent.height);
  Vulkan.Viewport.minDepth = 0.0f;
  Vulkan.Viewport.maxDepth = 1.0f;

  Vulkan.Scissor.offset = vk::Offset2D (0, 0);
  Vulkan.Scissor.extent = Vulkan.SwapChainExtent;

  const vk::PipelineViewportStateCreateInfo ViewportStateInfo{
    .viewportCount = 1,
    .pViewports    = &Vulkan.Viewport,
    .scissorCount  = 1,
    .pScissors     = &Vulkan.Scissor
  };

  // Rasterizer
  constexpr const vk::PipelineRasterizationStateCreateInfo RasterizerStateInfo{
    .depthClampEnable        = VK_FALSE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode             = vk::PolygonMode::eFill,
    .cullMode                = vk::CullModeFlagBits::eBack,
    .frontFace               = vk::FrontFace::eClockwise,
    .depthBiasEnable         = VK_FALSE,
    .depthBiasConstantFactor = 0.0f,
    .depthBiasClamp          = 0.0f,
    .depthBiasSlopeFactor    = 0.0f,
    .lineWidth               = 1.0f
  };

  // MSAA Multisampling
  const vk::PipelineMultisampleStateCreateInfo MultisamplingStateInfo{
    .rasterizationSamples  = vk::SampleCountFlagBits::e1,
    .sampleShadingEnable   = VK_FALSE,
    .minSampleShading      = 1.0f,
    .pSampleMask           = nullptr,
    .alphaToCoverageEnable = VK_FALSE,
    .alphaToOneEnable      = VK_FALSE
  };

  // Depth and stencil testing
  // Not Yet

  // Color Blending
  constexpr const vk::PipelineColorBlendAttachmentState
                  ColorBlendAttachmentState{
                    .blendEnable         = VK_FALSE,
                    .srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
                    .dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
                    .colorBlendOp        = vk::BlendOp::eAdd,
                    .srcAlphaBlendFactor = vk::BlendFactor::eOne,
                    .dstAlphaBlendFactor = vk::BlendFactor::eZero,
                    .alphaBlendOp        = vk::BlendOp::eAdd,
                    .colorWriteMask
        = vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eG
          | vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eA
      };

  const vk::PipelineColorBlendStateCreateInfo ColorBlendStateInfo{
    .logicOpEnable   = VK_FALSE,
    .logicOp         = vk::LogicOp::eCopy,
    .attachmentCount = 1,
    .pAttachments    = &ColorBlendAttachmentState
  };

  const vk::PipelineLayoutCreateInfo PipelineLayoutInfo{
    .setLayoutCount         = 0,
    .pSetLayouts            = nullptr,
    .pushConstantRangeCount = 0,
    .pPushConstantRanges    = nullptr
  };

  Result = Vulkan.Device.createPipelineLayout (&PipelineLayoutInfo, nullptr,
                                               &Vulkan.PipelineLayout);
#if PATATA_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Pipeline Layout", Result);
      return false;
    }
#endif

  vk::GraphicsPipelineCreateInfo GraphicsPipelineInfo{
    .stageCount          = 2,
    .pStages             = ShaderStageInfo,
    .pVertexInputState   = &VertexInputStateInfo,
    .pInputAssemblyState = &InputAssemblyStateInfo,
    .pViewportState      = &ViewportStateInfo,
    .pRasterizationState = &RasterizerStateInfo,
    .pMultisampleState   = &MultisamplingStateInfo,
    .pDepthStencilState  = nullptr,
    .pColorBlendState    = &ColorBlendStateInfo,
    .pDynamicState       = &DynamicStateInfo,
    .layout              = Vulkan.PipelineLayout,
    .renderPass          = Vulkan.RenderPass
  };

  Result = Vulkan.Device.createGraphicsPipelines (
      VK_NULL_HANDLE, 1, &GraphicsPipelineInfo, nullptr,
      &Vulkan.GraphicsPipeline);
#if PATATA_DEBUG == 1
  if (Result != vk::Result::eSuccess)
    {
      std::future<void> ReturnVulkanCheck
          = std::async (std::launch::async, Patata::Log::VulkanCheck,
                        "Graphics Pipeline", Result);

      return false;
    }
#endif

  Vulkan.Device.destroyShaderModule (VertexShaderModule);
  Vulkan.Device.destroyShaderModule (FragmentShaderModule);

  return true;
}
