DIST_SUBDIRS = callbacks transitions initializations instanceupdates data 

# if !BOOST_TEST and !TRAINING then ELY_ELF 
if ELY_ELF
ELYELFDIRS = $(DIST_SUBDIRS)
endif 

SUBDIRS = $(ELYELFDIRS)

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

noinst_HEADERS = \
		Ely.h
	
BUILT_SOURCES = Ely_ini.h

nodist_noinst_HEADERS =\
		Ely_ini.h

#ely
bin_PROGRAMS = ely

ely_ltlibraries = \
		$(top_builddir)/src/libgame.la \
		$(top_builddir)/src/libaicomponents.la \
		$(top_builddir)/src/libaudiocomponents.la \
		$(top_builddir)/src/libbehaviorcomponents.la \
		$(top_builddir)/src/libcontrolcomponents.la \
		$(top_builddir)/src/libphysicscomponents.la \
		$(top_builddir)/src/libscenecomponents.la \
		$(top_builddir)/src/libobjectmodel.la \
		$(top_builddir)/src/libutilities.la

ely_libraries = \
		$(top_builddir)/src/libsupport.a
		
whole_archives = \
		-Wl,$(top_builddir)/src/libsupport.a

#ely
ely_SOURCES = \
		Ely.cpp
		
ely_LDFLAGS = $(AM_LDFLAGS) -export-dynamic \
		-Wl,--whole-archive \
		$(whole_archives) \
		-Wl,--no-whole-archive
		
ely_LDADD = \
		$(ely_ltlibraries) \
		$(ely_libraries) \
		$(top_builddir)/tinyxml2/libtinyxml2.la \
		-dlopen $(builddir)/callbacks/callbacks.la \
		-dlopen $(builddir)/initializations/initializations.la \
		-dlopen $(builddir)/transitions/transitions.la \
		$(ELY_LIBS)
		
dist_pkgdata_DATA = game.xml config.prc

EXTRA_DIST = Ely_ini.h.in config.prc.in game.xml.in

if !ELY_DEVEL
substDataDir = sed -e 's|@elydatadir[@]|$(pkgdatadir)|g'
substLibDir = sed -e 's|@elylibdir[@]|$(pkglibdir)|g'
substCallbacks = sed -e 's/@callbacks[@]\///g'
substTransitions = sed -e 's/@transitions[@]\///g'
substInitializations = sed -e 's/@initializations[@]\///g'
substInstanceupdates = sed -e 's/@instanceupdates[@]\///g'
else
substDataDir = sed -e 's|@elydatadir[@]|$(abs_top_srcdir)/ely/data|g'
substLibDir = sed -e 's|@elylibdir[@]|$(abs_top_builddir)/ely|g'
substCallbacks = sed -e 's/@callbacks[@]/callbacks/g'
substTransitions = sed -e 's/@transitions[@]/transitions/g'
substInitializations = sed -e 's/@initializations[@]/initializations/g'
substInstanceupdates = sed -e 's/@instanceupdates[@]/instanceupdates/g'
endif 

Ely_ini.h : $(top_srcdir)/ely/Ely_ini.h.in Makefile
	$(substDataDir) $(top_srcdir)/ely/$@.in | \
	$(substLibDir) | \
	$(substCallbacks) | \
	$(substTransitions) | \
	$(substInitializations) | \
	$(substInstanceupdates) > $@

config.prc : $(top_srcdir)/ely/config.prc.in Makefile
	$(substDataDir) $(top_srcdir)/ely/$@.in > $@

game.xml : $(top_srcdir)/ely/game.xml.in Makefile
	$(substDataDir) $(top_srcdir)/ely/$@.in > $@

CLEANFILES = Ely_ini.h config.prc game.xml
		