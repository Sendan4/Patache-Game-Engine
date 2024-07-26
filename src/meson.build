source = files([
  'Core/PatataEngine.cpp',
  'Core/Input.cpp',
  'Core/Render.cpp',
  'Window/Window.cpp',
  'RaccoonRenderer/RaccoonRenderer.cpp',
  'RaccoonRenderer/Vulkan/Setup_Vulkan_Renderer.cpp',
  'RaccoonRenderer/Vulkan/Vulkan_Instance.cpp',
  'RaccoonRenderer/Vulkan/Vulkan_Select_Device.cpp',
  'RaccoonRenderer/Vulkan/Vulkan_Logical_And_Queue.cpp',
  'RaccoonRenderer/Vulkan/Vulkan_SwapChain.cpp',
  'RaccoonRenderer/Vulkan/Vulkan_DepthBuffer.cpp',
  'RaccoonRenderer/Vulkan/Vulkan_Render.cpp',
  'RaccoonRenderer/Vulkan/Vulkan_PipeLine.cpp',
  'RaccoonRenderer/Vulkan/Vulkan_RenderPass.cpp',
  'Log/YamlErrors.cpp',
  'Log/Messages.cpp'])

if target_machine.system() == 'windows'
  source += files([
    'Log/Win/Start_Patata_Log_Info_Win.cpp',
    'Log/Win/WindowLogWin.cpp',
    'Log/Win/Setup_Vulkan_Log_Win.cpp',
    'Log/Win/Vulkan_Info_Win.cpp'])
else
  source += files([
    'Log/Start_Patata_Log_Info.cpp',
    'Log/WindowLog.cpp',
    'Log/Setup_Vulkan_Log.cpp',
    'Log/Vulkan_Info.cpp'])
endif

if get_option('buildtype') == 'debug' or get_option('buildtype') == 'debugoptimized'
  source +=files([
    'ImGUI/ImGUISetupBackend.cpp',
    'ImGUI/ImGUIFrame.cpp',
    'ImGUI/DrawDebugUI.cpp'])
endif

lib = library(meson.project_name(),
  source,
  include_directories : [
    include,
    include_directories('Log'),
    include_directories('Core'),
    include_directories('RaccoonRenderer'),
    include_directories('RaccoonRenderer/Vulkan'),
    include_directories('ImGUI'),
    include_directories('Window')],
  d_import_dirs : include,
  dependencies : dependencies,
  soversion : meson.project_version(),
  build_by_default : true,
  install : true,
  cpp_args : cpp_arguments,
  install_dir : bin_dir,
  install_rpath : '.',
  build_rpath : '.')

project_dep = declare_dependency(
  include_directories : [include],
  d_import_dirs : [include],
  link_with : lib)

set_variable(meson.project_name() + '_dep', project_dep)
