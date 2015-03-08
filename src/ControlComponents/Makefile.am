AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = $(ELY_LIBS)

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LIBRARIES = \
	libControlComponents.a

#library sources
libControlComponents_a_SOURCES = \
	Chaser.cpp \
	ChaserTemplate.cpp \
	Driver.cpp \
	DriverTemplate.cpp
