subdir('libmidi')

conf_data = configuration_data()
conf_data.set_quoted('PACKAGE_VERSION', meson.project_version())
config = configure_file(output : 'config.h',
                        configuration : conf_data)

dependencies = [
  dependency('alsa'),
  dependency('gl'),
  dependency('sqlite3'),
  dependency('gconf-2.0'),
  dependency('SDL2'),
  dependency('SDL2_ttf'),
  dependency('SDL2_image'),
  dependency('threads')
]

executable('linthesia',
           config,
           'MenuLayout.cpp',
           'CompatibleSystem.cpp',
           'StringTile.cpp',
           'TitleState.cpp',
           'Tga.cpp',
           'SongTile.cpp',
           'TrackTile.cpp',
           'UserSettings.cpp',
           'TextWriter.cpp',
           'EnumTile.cpp',
           'PlayingState.cpp',
           'KeyboardDisplay.cpp',
           'TrackSelectionState.cpp',
           'GameState.cpp',
           'Keyboard.cpp',
           'MidiComm.cpp',
           'Renderer.cpp',
           'DeviceTile.cpp',
           'FrameAverage.cpp',
           'StatsState.cpp',
           'LinthesiaError.cpp',
           'main.cpp',
           'SongLibState.cpp',
           'DpmsThread.cpp',
           'FileSelector.cpp',
           dependencies : dependencies,
           link_with: libmidi,
           install : true)
