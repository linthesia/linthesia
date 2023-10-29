# linthesia

![Build Status](https://github.com/linthesia/linthesia/actions/workflows/c-cpp.yml/badge.svg)


Linthesia is a fork of the Windows/Mac game called Synthesia. It is a game of playing music using a MIDI keyboard (or your PC keyboard), following a .mid file.

Synthesia up to version 0.6.1a is Open Source. This project uses the latest source from sourceforge.

## Build

To compile, you need a basic c++ toolchain, and satisfy all dependences which are on BUILD-DEPENDS file. Then, just:

    $ meson --prefix /usr build
    $ ninja -C build
    $ ninja -C build install #probably with sudo

## Themes support

Textures for a new theme are in folder graphics-xxx, e. g. graphics-blue.

Select a theme "blue":

    $ linthesia --theme blue

Select default gray theme:

    $ linthesia --theme

TODO:

Add post install step to copy a new theme folder to {prefix}/share/linthesia/.

Add post install step to create link to common textures in {prefix}/share/linthesia/graphics-xxx.

At the moment you must manually create links.

    $ cd /usr/share/linthesia/graphics-blue/
    $ ln -s ../graphics/* .

## Credits

Visit https://github.com/linthesia/linthesia for more info.

Join the chat at https://gitter.im/linthesia/linthesia
