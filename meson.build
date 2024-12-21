project('Patata_Engine',
  'cpp', 'c',
  version : '1',
  meson_version : '>=1.4.0',
  default_options : [
    'buildtype=release',
    'cpp_std=c++20',
    'werror=false',
    'warning_level=3'],
  subproject_dir : 'externals',
  license : 'MIT',
  license_files : 'LICENSE.txt')

# Version Macros
add_project_arguments(
  '-DPATATA_ENGINE_VERSION_MAYOR=1',
  '-DPATATA_ENGINE_VERSION_MINOR=0',
  '-DPATATA_ENGINE_VERSION_PATCH=0',
  language : 'cpp')

cpp = meson.get_compiler('cpp')

if cpp.get_id() == 'gcc' or cpp.get_id() == 'clang'
  if not cpp.has_argument('-std=c++20')
    error('your compiler does not support C++20')
  else
    cpp_arguments = ['-std=c++20']
  endif

  # Fast_io warnings
  if cpp.get_id() == 'clang' or cpp.get_id() == 'gcc'
    if cpp.has_argument('-std=c++23')
      cpp_arguments += ['-std=c++23']
    endif
  endif

  if cpp.has_argument('-Wall')
    cpp_arguments += ['-Wall']
  endif
  if cpp.has_argument('-Wextra')
    cpp_arguments += ['-Wextra']
  endif
  if cpp.has_argument('-Wpedantic')
    cpp_arguments += ['-Wpedantic']
  endif
  if cpp.has_argument('-fno-exceptions')
    cpp_arguments += ['-fno-exceptions']
  endif
  if cpp.has_argument('-fno-strict-aliasing')
    cpp_arguments += ['-fno-strict-aliasing']
  endif
  if cpp.has_argument('-fno-rtti') and (get_option('default_library') == 'release' or get_option('default_library') == 'minsize')
    cpp_arguments += ['-fno-rtti']
  endif
endif

if get_option('default_library') == 'static' or get_option('prefer_static') == true
  cpp.has_argument('-static-libgcc')
  cpp.has_argument('-static-libstdc++')

  cpp_arguments = ['-static-libgcc', '-static-libstdc++']
endif

bin_dir = meson.global_build_root()
if get_option('use_bin_dir')
  bin_dir = join_paths(meson.global_build_root(), '../bin')
endif

if (cpp.has_link_argument('-ffast-math'))
  cpp_arguments += ['-ffast-math']
endif

cpp.check_header('cstdint', required : true)
if not cpp.has_type('uint8_t', prefix : '#include <cstdint>')
  error('type uint8_t not usable in cstdint header')
endif
if not cpp.has_type('uint16_t', prefix : '#include <cstdint>')
  error('type uint16_t not usable in cstdint header')
endif
if not cpp.has_type('uint32_t', prefix : '#include <cstdint>')
  error('type uint32_t not usable in cstdint header')
endif
if not cpp.has_type('uint64_t', prefix : '#include <cstdint>')
  error('type uint64_t not usable in cstdint header')
endif
cpp.has_header_symbol('cstdint', 'UINT64_MAX', required : true)

cpp.check_header('cstdlib', required : true)
if build_machine.system() == 'linux'
  if not cpp.has_function('getenv')
    error('function getenv not usable')
  endif
  if not cpp.has_function('setenv')
    error('function setenv not usable')
  endif
endif

if cpp.get_id() == 'gcc'
  cpp.check_header('cxxabi.h', required : true)
endif

cpp.check_header('string', required : true)
if not cpp.has_type('std::string_view', prefix : '#include <string>')
  error('type std::string_view not usable in string header')
endif
if not cpp.has_type('std::string', prefix : '#include <string>')
  error('type std::string not usable in string header')
endif

cpp.check_header('future', required : true)
if not cpp.has_type('std::future<void>', prefix : '#include <future>')
  error('type std::future not usable in future header')
endif
if not cpp.has_type('std::future<bool>', prefix : '#include <future>')
  error('type std::future not usable in future header')
endif
cpp.has_header_symbol('future', 'std::async', required : true)
cpp.check_header('limits', required : true)
cpp.has_header_symbol('limits', 'std::numeric_limits', required : true)

c = meson.get_compiler('c')
if target_machine.system() == 'windows'
  c.check_header('windows.h', required : true)

  if not cpp.has_function('SetEnvironmentVariable', prefix : '#include <windows.h>', required : true)
    error('function SetEnvironmentVariable not usable')
  endif
endif

if get_option('use_git')
  git = find_program('git', native: true, required : false)
endif

if get_option('use_git') and git.found()
  # commit macro
  # Git Commit Hashes
  get_commit_hash_long = run_command('git', 'rev-parse', 'HEAD', check: true)
  git_commit_long_hash = get_commit_hash_long.stdout().strip()

  get_commit_hash_short = run_command('git', 'rev-parse', '--short', 'HEAD', check: true)
  git_commit_short_hash = get_commit_hash_short.stdout().strip()

  add_project_arguments('-DPATATA_GIT_HASH_LONG="'+git_commit_long_hash+'"', language : 'cpp')
  add_project_arguments('-DPATATA_GIT_HASH_SHORT="'+git_commit_short_hash+'"', language : 'cpp')

  # Git Branch
  branchCommand = run_command('git', 'branch', '--show-current', check : true)
  branchCOutput = branchCommand.stdout().strip()

  add_project_arguments('-DPATATA_GIT_BRANCH="'+branchCOutput+'"', language : 'cpp')

  # The commit is Dirty?
  git_work_dir_is_clean_command = run_command('git', 'diff', 'HEAD', '--shortstat', check : true)
  git_work_dir_is_clean_text_out = git_work_dir_is_clean_command.stdout().strip()

  git_work_dir_is_clean = ''
  if git_work_dir_is_clean_text_out != ''
    git_work_dir_is_clean = 'Dirty'
    add_project_arguments('-DPATATA_GIT_WORK_DIR_IS_CLEAN_BOOL=0', language : 'cpp')
  else
    git_work_dir_is_clean = 'Clean'
    add_project_arguments('-DPATATA_GIT_WORK_DIR_IS_CLEAN_BOOL=1', language : 'cpp')
  endif

  add_project_arguments('-DPATATA_GIT_WORK_DIR_IS_CLEAN="'+git_work_dir_is_clean+'"', language : 'cpp')

  # The Changes are in the stage?
  git_commit_stage_command = run_command('git', 'diff', 'HEAD', '--shortstat', '--staged', check : true)
  git_commit_stage_text_out = git_commit_stage_command.stdout().strip()

  git_is_commit_in_the_stage = ''
  if git_commit_stage_text_out != ''
    git_is_commit_in_the_stage = 'Staged'
    add_project_arguments('-DPATATA_GIT_WORK_DIR_IS_STAGED_BOOL=1', language : 'cpp')
  else
    git_is_commit_in_the_stage = 'Not Staged'
    add_project_arguments('-DPATATA_GIT_WORK_DIR_IS_STAGED_BOOL=0', language : 'cpp')
  endif

  add_project_arguments('-DPATATA_GIT_WORK_DIR_IS_STAGED="'+git_is_commit_in_the_stage+'"', language : 'cpp')
endif

if get_option('default_library') == 'shared' or get_option('default_library') == 'both'
  add_project_arguments('-DSHARED_LIBRARY_EXPORT_DEFINE=1', language : 'cpp')
endif

ninja = find_program('ninja', native: true, required : true)

if cpp.get_id() == 'gcc'
  compiler = find_program('g++', native: true, required : true)
elif cpp.get_id() == 'clang'
  compiler = find_program('clang++', native: true, required : true)
elif cpp.get_id() == 'msvc'
  compiler = find_program('cl', native: true, required : true)
endif

fs = import('fs')
compiler_name = fs.name(compiler.full_path())

# Macros
add_project_arguments(
  '-DPATATA_BUILD_SYSTEM="Meson"',
  '-DPATATA_BUILD_SYSTEM_VERSION="'+meson.version()+'"',
  '-DPATATA_BUILD_SYSTEM_GENERATOR="Ninja"',
  '-DPATATA_BUILD_SYSTEM_GENERATOR_VERSION="'+ninja.version()+'"',
  '-DPATATA_ENGINE_VERSION="'+meson.project_version()+'"',
  '-DPATATA_ENGINE_NAME="'+meson.project_name()+'"',
  '-DPATATA_COMPILER_PROGRAM="'+compiler_name+'"',
  '-DPATATA_COMPILER_VERSION="'+compiler.version()+'"',
  '-DPATATA_ARCH="'+build_machine.cpu_family()+'"',
  '-DVULKAN_HPP_NO_EXCEPTIONS',
  '-DVULKAN_HPP_NO_SMART_HANDLE',
  language : 'cpp')

if cpp.get_id() == 'gcc' or cpp.get_id() == 'clang'
  add_project_arguments('-DPATATA_BUILD_SYSTEM_GENERATOR="Ninja"', language : 'cpp')
endif

# Build Type macro
if (get_option('buildtype') == 'release')
  add_project_arguments('-DPATATA_BUILD_TYPE="Release"', language : 'cpp')
elif (get_option('buildtype') == 'debug')
  add_project_arguments('-DPATATA_BUILD_TYPE="Debug"', '-DDEBUG=1', language : 'cpp')
elif (get_option('buildtype') == 'debugoptimized')
  add_project_arguments('-DPATATA_BUILD_TYPE="Debug Optimized"', '-DDEBUG=1', language : 'cpp')
elif (get_option('buildtype') == 'minsize')
  add_project_arguments('-DPATATA_BUILD_TYPE="Minimal Size Release"', language : 'cpp')
endif

sdl2_dep = dependency('sdl2',
  required : true,
  static : false,
  version : '>=2.30',
  modules : ['SDL2::SDL2'])

rapidyaml = dependency('ryml',
  static : false,
  required : true)

Vulkan = dependency('Vulkan',
  static : false,
  required : true)

Fast_io_dep = subproject('fast_io').get_variable('fast_io_dep')

dependencies = [Vulkan, Fast_io_dep, sdl2_dep, rapidyaml]

if (get_option('buildtype') == 'debug' or get_option('buildtype') == 'debugoptimized')
  imgui_dep = subproject('imgui').get_variable('imgui_dep')

  add_project_arguments('-DPATATA_IMGUI_VERSION="'+imgui_dep.version()+'"', language : 'cpp')

  dependencies += [imgui_dep]

  if get_option('use_vvl') == true
    add_project_arguments('-DPATATA_USE_VVL', language : 'cpp')

    if target_machine.system() == 'linux'
      cpp.find_library('VkLayer_khronos_validation', required : true)
    endif
  endif
endif

add_project_arguments('-DPATATA_LINUX_XORG_SUPPORT=1', language : 'cpp')

if Fast_io_dep.found()
  cpp.check_header('fast_io.h',
    dependencies : Fast_io_dep,
    args : '-std=c++20',
    include_directories : include_directories('externals/fast_io/include'),
    required : true)

  cpp.has_header_symbol('fast_io.h', 'fast_io::io::print',
    include_directories : include_directories('externals/fast_io/include'),
    dependencies : Fast_io_dep,
    args : '-std=c++20',
    required : true)
  cpp.has_header_symbol('fast_io.h', 'fast_io::io::println',
    include_directories : include_directories('externals/fast_io/include'),
    dependencies : Fast_io_dep,
    args : '-std=c++20',
    required : true)
  cpp.has_header_symbol('fast_io.h', 'fast_io::io::perr',
    include_directories : include_directories('externals/fast_io/include'),
    dependencies : Fast_io_dep,
    args : '-std=c++20',
    required : true)
  cpp.has_header_symbol('fast_io.h', 'fast_io::io::perrln',
    include_directories : include_directories('externals/fast_io/include'),
    dependencies : Fast_io_dep,
    args : '-std=c++20',
    required : true)
  cpp.has_header_symbol('fast_io.h', 'fast_io::mnp::hex',
    include_directories : include_directories('externals/fast_io/include'),
    dependencies : Fast_io_dep,
    args : '-std=c++20',
    required : true)
endif

if sdl2_dep.found()
  cpp.check_header('SDL.h', dependencies : sdl2_dep, required : true)
  cpp.check_header('SDL_vulkan.h', dependencies : sdl2_dep, required : true)
  cpp.has_header_symbol('SDL_vulkan.h', 'SDL_Vulkan_CreateSurface', dependencies : sdl2_dep, required : true)
endif

if Vulkan.found()
  cpp.check_header('vulkan/vulkan.hpp', dependencies : Vulkan, required : true)
  cpp.has_header_symbol('vulkan/vulkan.hpp', 'vk::createInstance', dependencies : Vulkan, required : true)
  cpp.has_type('vk::Instance', dependencies : Vulkan, prefix : '#include <vulkan/vulkan.hpp>')
  cpp.has_header_symbol('vulkan/vulkan.hpp', 'vkCreateDevice', dependencies : Vulkan, required : true)
  cpp.has_type('vk::Device', dependencies : Vulkan, prefix : '#include <vulkan/vulkan.hpp>')
  cpp.has_header_symbol('vulkan/vulkan.hpp', 'vkEnumeratePhysicalDevices', dependencies : Vulkan, required : true)
  cpp.has_header_symbol('vulkan/vulkan.hpp', 'vkCreateSwapchainKHR', dependencies : Vulkan, required : true)
  cpp.has_type('vk::SwapchainKHR', dependencies : Vulkan, prefix : '#include <vulkan/vulkan.hpp>')
  cpp.has_header_symbol('vulkan/vulkan.hpp', 'vkGetSwapchainImagesKHR', dependencies : Vulkan, required : true)
  cpp.has_header_symbol('vulkan/vulkan.hpp', 'vk::to_string', dependencies : Vulkan, required : true)
endif

if (get_option('buildtype') == 'debug' or get_option('buildtype') == 'debugoptimized')
if (imgui_dep.found())
  cpp.check_header('imgui.h', include_directories : include_directories('externals/imgui/'), required : true)
  cpp.has_header_symbol('imgui.h', 'ImGui::CreateContext',
    include_directories : include_directories('externals/imgui/'),
    required : true)
  cpp.has_header_symbol('imgui.h', 'ImGui::StyleColorsDark',
    include_directories : include_directories('externals/imgui/'))
  cpp.has_type('ImGuiStyle', include_directories : include_directories('externals/imgui'), prefix : '#include <imgui.h>')

  cpp.has_header('imgui_impl_vulkan.h', include_directories : include_directories('externals/imgui/backends'), required : true)
  cpp.has_header('imgui_impl_sdl2.h', include_directories : include_directories('externals/imgui/backends'), required : true)
endif
endif

add_project_arguments(
  '-DPATATA_FAST_IO_GIT_COMMIT_HASH_SHORT="'+Fast_io_dep.version()+'"',
  '-DPATATA_RAPIDYAML_VERSION="'+rapidyaml.version()+'"',
  '-DPATATA_VULKAN_LOADER_VERSION="'+Vulkan.version()+'"',
  language : 'cpp')

#Game name
if get_option('game_name') != ''
  add_project_arguments('-DPATATA_GAME_NAME="'+get_option('game_name')+'"', language : 'cpp')
endif

subdir('data/assets')
subdir('data')

include = include_directories('include/')
subdir('src')

info = {
  'Game Name' : get_option('game_name'),
  'Icon' : get_option('enable_icon'),
  'Custom Icon' : get_option('use_custom_icon'),
  'Branch' : branchCOutput,
  'Commit Hash' : git_commit_long_hash,
  'Build Type' : get_option('buildtype'),
  'Library Type' : get_option('default_library')
}

summary({
  'Branch' : info['Branch'],
  'Commit Hash' : info['Commit Hash'],
  'Game Name' : info['Game Name'],
  'Icon' : info['Icon'],
  'Custom Icon' : info['Custom Icon'],
  'Build Type' : info['Build Type'],
  'Library Type' : info['Library Type']},
  bool_yn : true)
