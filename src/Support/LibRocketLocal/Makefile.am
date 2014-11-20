AM_CPPFLAGS = -iquote$(top_srcdir)/include -iquote$(top_srcdir)/include/Support/tinyxlm2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

#ltlibraries
pkglib_LTLIBRARIES = \
		librocket.la

#ltlibraries sources
librocket_la_SOURCES = \
		config_rocket.cxx \
		rocketSystemInterface.cxx \
		rocketInputHandler.cxx \
		rocketRenderInterface.cxx \
		rocketRegion.cxx \
		rocketFileInterface.cxx
