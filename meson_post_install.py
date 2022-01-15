#!/usr/bin/env python3

import glob
import os
import re
import subprocess
import sys

if not os.environ.get('DESTDIR'):
  project_name = sys.argv[1]
  datadir = sys.argv[2]
  icondir = os.path.join(datadir, 'icons', 'hicolor')

  name_pattern = re.compile('hicolor_(?:apps)_(?:\d+x\d+|scalable)_(.*)')
  search_pattern = '/**/hicolor_*'

  [os.rename(file, os.path.join(os.path.dirname(file), name_pattern.search(file).group(1)))
   for file in glob.glob(icondir + search_pattern, recursive=True)]

  print('Update icon cache...')
  subprocess.call(['gtk-update-icon-cache', '-f', '-t', '-q', icondir])

  icondir = os.path.join(datadir, project_name, 'icons', 'hicolor')
  subprocess.call(['gtk-update-icon-cache', '-f', '-t', '-q', icondir])

  schemadir = os.path.join(datadir, 'glib-2.0', 'schemas')
  print('Compiling gsettings schemas...')
  subprocess.call(['glib-compile-schemas', schemadir])

print('DESTDIR')
