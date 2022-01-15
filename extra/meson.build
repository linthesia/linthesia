i18n.merge_file(
  type: 'desktop',
  input: 'linthesia.desktop',
  output: '@0@.desktop'.format(project_name),
  po_dir: po_dir,
  install: true,
  install_dir: datadir / 'applications',
)

i18n.merge_file(
  type: 'xml',
  input: 'linthesia.metainfo.xml',
  output: '@0@.metainfo.xml'.format(project_name),
  po_dir: po_dir,
  install: true,
  install_dir: datadir / 'metainfo',
)

install_data('linthesia.xpm', install_dir: datadir / project_name / 'icons')
