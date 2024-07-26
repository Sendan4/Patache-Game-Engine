project('fast_io', 'cpp',
	version : '804d943',
	default_options : ['cpp_std=c++20'])

meson.get_compiler('c').check_header('stdio.h', required : false)
meson.get_compiler('cpp').check_header('version', required : true)
meson.get_compiler('cpp').check_header('cstddef', required : true)
meson.get_compiler('cpp').check_header('type_traits', required : true)
meson.get_compiler('cpp').check_header('concepts', required : true)
meson.get_compiler('cpp').check_header('bit', required : true)
meson.get_compiler('cpp').check_header('limits', required : true)
meson.get_compiler('cpp').check_header('cstdint', required : true)
meson.get_compiler('cpp').check_header('source_location', required : false)

Fast_io_Headers = include_directories('include')

project_dep = declare_dependency(
	include_directories : Fast_io_Headers)

set_variable(meson.project_name() + '_dep', project_dep)
