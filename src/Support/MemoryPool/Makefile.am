AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = $(ELY_LIBS)

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LIBRARIES = \
	libMemoryPool.a

#library sources
libMemoryPool_a_SOURCES = \
	MemoryPool.cpp
