# Ely testinterrogate target
#see https://www.panda3d.org/forums/viewtopic.php?f=12&t=16164
AM_CPPFLAGS = -DTESTING -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = -module -avoid-version $(ELY_LDFLAGS)

lib_LTLIBRARIES = testinterrogate.la

testinterrogate_la_LIBADD = $(ELY_LIBS)

MODLIB_NAME = testinterrogate

INTERR_CMD = /usr/bin/interrogate
INTERR_CPPFLAGS = $(AM_CPPFLAGS) \
	-S/usr/include/panda3d/parser-inc -S/usr/include/panda3d \
	-D__inline -DCPPPARSER -D__cplusplus -Dvolatile=
INTERR_OPTS = -fnames -string -refcount -assert -python-native \
	-module $(MODLIB_NAME) -library $(MODLIB_NAME)

INTERR_MODULE_CMD = $(INTERR_CMD)_module
INTERR_MODULE_OPTS = -python-native \
	-module $(MODLIB_NAME) -library $(MODLIB_NAME)

testinterrogate_la_SOURCES = \
	testinterrogate.cpp \
	testinterrogate_igate.cpp \
	testinterrogate_module.cpp

testinterrogate_igate.cpp: testinterrogate.h
	$(INTERR_CMD) $(INTERR_CPPFLAGS) $(INTERR_OPTS) \
	-od testinterrogate.in -oc $@ \
	$(srcdir)/testinterrogate.h 
	 
testinterrogate_module.cpp: testinterrogate_igate.cpp
	$(INTERR_MODULE_CMD) $(INTERR_MODULE_OPTS) -oc $@ \
	testinterrogate.in 
		
nodist_noinst_HEADERS = \
	testinterrogate.h

CLEANFILES = testinterrogate_igate.cpp testinterrogate_module.cpp testinterrogate.in
