# linthesia

![Build Status](https://github.com/linthesia/linthesia/actions/workflows/c-cpp.yml/badge.svg)


Linthesia is a fork of the Windows/Mac game called Synthesia. It is a game of playing music using a MIDI keyboard (or your PC keyboard), following a .mid file.

Synthesia up to version 0.6.1a is Open Source. This project uses the latest source from sourceforge.

## Build

To compile, you need a basic c++ toolchain, and satisfy all dependences which are on BUILD-DEPENDS file. Then, just:

    $ meson --prefix /usr build
    $ ninja -C build
    $ ninja -C build install #probably with sudo

## Build deb

    $ debuild -b -uc -us

## Themes support

Textures for a new theme are in folder graphics-xxx, e. g. graphics-blue.

Select a theme "blue":

    $ linthesia --theme blue

Select default gray theme:

    $ linthesia --theme

## Creating new themes for installed program

Create directory for new theme e.g. /usr/share/linthesia/graphics-red.

Create color.txt wich contains a hex RGB value for background.

Run createlinks.sh

    $ /usr/share/linthesia/createlinks.sh

Replace one or more textures with your own textures.

Select a theme "red":

    $ linthesia --theme red

## Credits

Visit https://github.com/linthesia/linthesia for more info.

Join the chat at https://gitter.im/linthesia/linthesia
