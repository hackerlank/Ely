AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = $(ELY_LIBS)

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LIBRARIES = \
	libAudioComponents.a

#library sources
libAudioComponents_a_SOURCES = \
	Listener.cpp \
	ListenerTemplate.cpp \
	Sound3d.cpp \
	Sound3dTemplate.cpp
