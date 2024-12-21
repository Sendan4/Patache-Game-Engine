project('imgui', 'cpp',
  version : 'v1.91.5-docking',
  meson_version : '>=1.2.0')

dependencies = []

sdl2_dep = dependency('sdl2',
  required : true,
  static : false,
  version : '>=2.24.1',
  modules : ['SDL2::SDL2'])

Vulkan = dependency('Vulkan',
    static : false,
    required : true)

dependencies += [Vulkan, sdl2_dep]

meson.get_compiler('cpp').check_header('stdio.h', required : true)
meson.get_compiler('cpp').check_header('stdint.h', required : true)
meson.get_compiler('cpp').check_header('float.h', required : true)
meson.get_compiler('cpp').check_header('stdarg.h', required : true)
meson.get_compiler('cpp').check_header('stddef.h', required : true)
meson.get_compiler('cpp').check_header('string.h', required : true)
meson.get_compiler('cpp').check_header('assert.h', required : false)
meson.get_compiler('cpp').check_header('stdlib.h', required : true)
meson.get_compiler('cpp').check_header('limits.h', required : true)
meson.get_compiler('cpp').check_header('math.h', required : true)

if target_machine.system() == 'windows'
  meson.get_compiler('cpp').check_header('Windows.h', required : true)
endif

Sources = ['imgui.cpp',
    'imgui_draw.cpp',
    'imgui_demo.cpp',
    'imgui_tables.cpp',
    'imgui_widgets.cpp']

Include = include_directories('.')

backends = include_directories('backends/')

if Vulkan.found()
    meson.get_compiler('cpp').check_header('vulkan/vulkan.h', required : true)
    Sources += ['backends/imgui_impl_vulkan.cpp']
endif

if sdl2_dep.found()
    Sources += ['backends/imgui_impl_sdl2.cpp']
endif

librarytype = ''

force_static = false
if get_option('libs_shared_build') == true and build_machine.system() == 'windows'
    warning('Prefer the library type as static for ImGui')

    force_static = true
endif

if get_option('libs_shared_build') == true and force_static == false
    lib = shared_library(meson.project_name(),
        Sources,
        include_directories : [Include, backends],
        d_import_dirs : [Include, backends],
        dependencies : [dependencies],
        soversion : meson.project_version(),
        install : true,
        install_dir : join_paths(meson.global_build_root(),'../bin'))

    librarytype = 'Shared'
elif get_option('libs_shared_build') == false or force_static == true
    lib = static_library(meson.project_name(),
        Sources,
        include_directories : [Include, backends],
        d_import_dirs : [Include, backends],
        dependencies : [dependencies],
        install : true,
        install_dir : join_paths(meson.global_build_root(),'../bin'))

    librarytype = 'Static'
endif

project_dep = declare_dependency(
  include_directories : [Include, backends],
  d_import_dirs : [Include, backends],
  link_with : lib
)

set_variable(meson.project_name() + '_dep', project_dep)

# Manualy Set
Info = {
  'Build Type' : get_option('buildtype'),
  'Library Type' : librarytype
}

summary({
  'Build Type' : Info['Build Type'],
  'Library Type' : Info['Library Type']},
  bool_yn : true,
  section : 'Library')
