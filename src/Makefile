# -*- mode: makefile-gmake; coding: utf-8 -*-

GRAPHDIR?=../graphics

CXX = g++
CXXFLAGS =  -I . -I libmidi -ggdb -Wall -ansi
CXXFLAGS += `pkg-config --cflags gtkmm-2.4 gconfmm-2.6 gtkglextmm-1.2 alsa` -DGRAPHDIR="\"$(GRAPHDIR)\""
LDFLAGS = `pkg-config --libs gtkmm-2.4 gconfmm-2.6 gtkglextmm-1.2 alsa`
TARGET = linthesia

all: $(TARGET)

$(TARGET): main.o LinthesiaError.o UserSettings.o \
	CompatibleSystem.o FileSelector.o Renderer.o \
	Tga.o GameState.o TitleState.o TextWriter.o \
	MenuLayout.o DeviceTile.o StringTile.o MidiComm.o \
	TrackSelectionState.o TrackTile.o PlayingState.o \
	StatsState.o KeyboardDisplay.o libmidi/libmidi.a
	$(CXX) $(LDFLAGS) -o $@ $^

libmidi/libmidi.a:
	$(MAKE) -C libmidi

install: all
	-mkdir -p $(DESTDIR)/usr/games
	install -m 755 $(TARGET) -g root -o root $(DESTDIR)/usr/games/

.PHONY:clean
clean:
	find . -name "*.o" -delete
	find . -name "*~" -delete
	$(RM) $(TARGET)
