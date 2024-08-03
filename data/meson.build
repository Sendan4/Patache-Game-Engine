if get_option('use_bin_dir')
  fs = import('fs')
  if not fs.is_dir(join_paths(meson.global_build_root(), '../bin/'))
    run_command('mkdir', bin_dir, check : true)
  endif
endif

run_command('cp', 'patata.yaml', meson.global_build_root(), check : true)
run_command('cp', '-r', 'LICENSES/', bin_dir, check : true)

if get_option('game_name') != ''
  run_command('mv',
    meson.global_build_root() + '/patata.yaml',
    meson.global_build_root() + '/' + get_option('game_name') + '.yaml',
    check : true)

  if get_option('use_bin_dir')
    run_command('cp', meson.global_build_root() + '/' + get_option('game_name') + '.yaml', bin_dir, check : true)
  endif
else
  if get_option('use_bin_dir')
    run_command('cp', meson.global_build_root() + '/patata.yaml', bin_dir, check : true)
  endif
endif
