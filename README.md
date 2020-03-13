# linthesia

[![Build Status](https://api.travis-ci.org/mans17/linthesia.svg?branch=master)](https://travis-ci.org/mans17/linthesia)

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

I've tested the full install on a fresh install of Ubuntu 18.04. (LAST version as of March 2020, AFAIK).
before autogen, run :

    $ sudo apt-get install libgtkglextmm-x11-1.2 libtool-bin build-essential autoconf libgconfmm-2.6-dev libasound2-dev libsqlite3-dev

### 'no text' issue with ATI Mobility Radeon HD 5470

Setting LIBGL_ALWAYS_SOFTWARE might fix the problem without any noticable adverse effects.

To run linthesia, use :
`LIBGL_ALWAYS_SOFTWARE=1 linthesia`


## Credits

Visit https://github.com/linthesia/linthesia for more info.
