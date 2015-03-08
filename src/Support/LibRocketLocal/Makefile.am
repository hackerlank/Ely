AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = $(ELY_LIBS)

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LTLIBRARIES = \
	libRocketLocal.la

#library sources
libRocketLocal_la_SOURCES = \
	config_rocket.cxx \
	rocketSystemInterface.cxx \
	rocketInputHandler.cxx \
	rocketRenderInterface.cxx \
	rocketRegion.cxx \
	rocketFileInterface.cxx
