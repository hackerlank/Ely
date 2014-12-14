SUBDIRS = \
		BulletLocal \
		LibRocketLocal \
		MemoryPool \
		OpenSteerLocal \
		RecastNavigationLocal \
		tinyxml2

#Miscellaneous utilities
AM_CPPFLAGS = -iquote$(top_srcdir)/include -iquote$(top_srcdir)/include/Support/tinyxlm2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

AUTOMAKE_OPTIONS = subdir-objects

#ltlibraries
pkglib_LTLIBRARIES = \
		libsupport.la

#ltlibraries sources
libsupport_la_SOURCES = \
		FSM.cpp \
		Picker.cpp \
		Raycaster.cpp \
		Distributed/ClientRepositoryBase.cpp \
		Distributed/DistributedObjectBase.cpp
