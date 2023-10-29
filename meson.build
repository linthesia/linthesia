project('linthesia', 'cpp',
  version : '0.8.0',
  meson_version: '>=0.57.0',
  default_options : ['warning_level=3', 'cpp_std=c++14'])

project_name = meson.project_name()
prefix = get_option('prefix')
datadir = get_option('datadir')

common_flags = []

musicdir = datadir / project_name / 'music'
if get_option('musicdir') != ''
  musicdir = get_option('musicdir')
endif
common_flags += [
  '-DMUSICDIR="@0@"'.format(prefix / musicdir)
]

install_subdir('music', install_dir: datadir / project_name )

graphdir = datadir / project_name / 'graphics'
if get_option('graphdir') != ''
  graphdir = get_option('graphdir')
endif

graphdir_blue = datadir / project_name / 'graphics-blue'
if get_option('graphdir_blue') != ''
  graphdir_blue = get_option('graphdir_blue')
endif

common_flags += [
  '-DGRAPHDIR="@0@"'.format(prefix / graphdir)
]

add_project_arguments(common_flags, language: 'cpp')

i18n = import('i18n')
gettext = project_name

source_root = meson.current_source_dir()
po_dir = source_root / 'po'

subdir('extra')
subdir('graphics')
subdir('graphics-blue')
subdir('po')
subdir('src')

import('gnome').post_install(
  glib_compile_schemas: true,
  gtk_update_icon_cache: true,
)
meson.add_install_script(
  'createlinks.sh',
  project_name,
  prefix / datadir
  )
