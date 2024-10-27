# linthesia

![Build Status](https://github.com/linthesia/linthesia/actions/workflows/c-cpp.yml/badge.svg)


Linthesia is a fork of the Windows/Mac game called Synthesia. It is a game of playing music using a MIDI keyboard (or your PC keyboard), following a .mid file.

Synthesia up to version 0.6.1a is Open Source. This project uses the latest source from sourceforge.

## Build

To compile, you need a basic c++ toolchain, and satisfy all dependences which are on BUILD-DEPENDS file. Then, just:

    $ meson --prefix /usr build
    $ ninja -C build
    $ ninja -C build install #probably with sudo

## How to use
- In most case, just run `/usr/bin/linthesia` binary, and Enjoy :-)
- Linthesia provides you with several musics that will be installed in `/usr/share/linthesia/music/` you can use `--lib-path` to define an other path when required. (read `linthesia -h` since last used path is preserved).
- Linthesia will use anything it finds to play sound. You might want to install `fluidsynth` or `TiMidity++` to run the midi server required for it to produce a sound. You keyboard might provide output that can (probably) be used too, if wanted.

## Credits

Visit https://github.com/linthesia/linthesia for more info.

Join the chat at https://gitter.im/linthesia/linthesia
