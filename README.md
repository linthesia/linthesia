# linthesia

![Build Status](https://github.com/linthesia/linthesia/actions/workflows/c-cpp.yml/badge.svg)


Linthesia is a fork of the Windows/Mac game called Synthesia. It is a game of playing music using a MIDI keyboard (or your PC keyboard), following a .mid file.

Synthesia up to version 0.6.1a is Open Source. This project uses the latest source from sourceforge.

## Compile

To compile, you need a basic c++ toolchain, and satisfy all dependences which are on BUILD-DEPENDS file. Then, just:

    $ ./autogen.sh

Here you must choose:

 a) For developers

    $ mkdir build
    $ cd build     # Isolate compilation to speed future compilations
    $ ../configure

 b) For general public

    $ ../configure --prefix=/usr

Then:

    $ make
    $ sudo make install

## Troubleshooting

### What libs/things do I need to compile this thing ?

I've tested the full install on a fresh install of Ubuntu 20.04. (LAST version as of October 2021, AFAIK).
before autogen, run :

    $ sudo apt-get install libtool-bin build-essential autoconf libgconf2-dev libasound2-dev libsqlite3-dev libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev

## Credits

Visit https://github.com/linthesia/linthesia for more info.
