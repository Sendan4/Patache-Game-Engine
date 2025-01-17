if get_option('use_bin_dir')
  fs = import('fs')
  if not fs.is_dir(join_paths(meson.global_build_root(), '../bin/'))
    run_command('mkdir', bin_dir, check : true)
  endif
endif

run_command('cp', 'config.yaml', meson.global_build_root(), check : true)
run_command('cp', '-r', 'LICENSES/', bin_dir, check : true)

if get_option('use_bin_dir')
  run_command('cp', meson.global_build_root() + '/config.yaml', bin_dir, check : true)
endif