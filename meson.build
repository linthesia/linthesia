project('linthesia', 'cpp',
  version : '0.8.0',
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
subdir('po')
subdir('src')

meson.add_install_script(
  'meson_post_install.py',
  project_name,
  prefix / datadir
)

