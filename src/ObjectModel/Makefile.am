AM_CPPFLAGS = -iquote$(top_srcdir)/include -iquote$(top_srcdir)/include/Support/tinyxlm2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

#ltlibraries
pkglib_LTLIBRARIES = \
		libobjectmodel.la

#ltlibraries sources
libobjectmodel_la_SOURCES = \
		Component.cpp \
		ComponentTemplate.cpp \
		ComponentTemplateManager.cpp \
		Object.cpp \
		ObjectTemplate.cpp \
		ObjectTemplateManager.cpp
