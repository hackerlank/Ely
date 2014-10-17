DIST_SUBDIRS = callbacks transitions initializations instanceupdates data 

# if !BOOST_TEST and !TRAINING then ELY_ELF 
if ELY_ELF
ELYELFDIRS = $(DIST_SUBDIRS)
endif 

SUBDIRS = $(ELYELFDIRS)

AM_CPPFLAGS = -iquote$(top_srcdir)/include $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

noinst_HEADERS = \
		Ely.h
	
BUILT_SOURCES = Ely_ini.h

nodist_noinst_HEADERS =\
		Ely_ini.h

#elygame
bin_PROGRAMS = elygame

elygame_ltlibraries = \
		$(top_builddir)/src/libgame.la \
		$(top_builddir)/src/libaicomponents.la \
		$(top_builddir)/src/libaudiocomponents.la \
		$(top_builddir)/src/libbehaviorcomponents.la \
		$(top_builddir)/src/libcommoncomponents.la \
		$(top_builddir)/src/libcontrolcomponents.la \
		$(top_builddir)/src/libphysicscomponents.la \
		$(top_builddir)/src/libphysicscontrolcomponents.la \
		$(top_builddir)/src/libscenecomponents.la \
		$(top_builddir)/src/libobjectmodel.la \
		$(top_builddir)/src/libutilities.la

elygame_libraries = \
		$(top_builddir)/src/libbullet.a \
		$(top_builddir)/src/librecastnavigation.a \
		$(top_builddir)/src/librocket.a \
		$(top_builddir)/src/libtinyxml2.a
whole_archives = \
		-Wl,$(top_builddir)/src/libsupport.a \
		-Wl,$(top_builddir)/src/libopensteer.a

#elygame
elygame_SOURCES = \
		Ely.cpp
		
elygame_LDFLAGS = $(AM_LDFLAGS) -export-dynamic \
		-Wl,--whole-archive \
		$(whole_archives) \
		-Wl,--no-whole-archive
		
elygame_LDADD = \
		$(elygame_ltlibraries) \
		$(elygame_libraries) \
		-dlopen $(builddir)/callbacks/callbacks.la \
		-dlopen $(builddir)/initializations/initializations.la \
		-dlopen $(builddir)/instanceupdates/instanceupdates.la \
		-dlopen $(builddir)/transitions/transitions.la \
		$(ELY_LIBS)
		
dist_pkgdata_DATA = game.xml config.prc

EXTRA_DIST = Ely_ini.h.in config.prc.in game.xml.in

if !ELY_DEVEL
substDataDir = sed -e 's|@elygamedatadir[@]|$(pkgdatadir)/data|g'
substShareDir = sed -e 's|@elygamesharedir[@]|$(pkgdatadir)|g'
substLibDir = sed -e 's|@elygamelibdir[@]|$(pkglibdir)|g'
substCallbacks = sed -e 's/@callbacks[@]\///g'
substTransitions = sed -e 's/@transitions[@]\///g'
substInitializations = sed -e 's/@initializations[@]\///g'
substInstanceupdates = sed -e 's/@instanceupdates[@]\///g'
else
substDataDir = sed -e 's|@elygamedatadir[@]|$(abs_top_srcdir)/elygame/data|g'
substShareDir = sed -e 's|@elygamesharedir[@]|$(abs_top_builddir)/elygame|g'
substLibDir = sed -e 's|@elygamelibdir[@]|$(abs_top_builddir)/elygame|g'
substCallbacks = sed -e 's/@callbacks[@]/callbacks/g'
substTransitions = sed -e 's/@transitions[@]/transitions/g'
substInitializations = sed -e 's/@initializations[@]/initializations/g'
substInstanceupdates = sed -e 's/@instanceupdates[@]/instanceupdates/g'
endif 

#Ely_ini.h rules
Ely_ini.h : $(top_srcdir)/elygame/Ely_ini.h.in Makefile
	$(substDataDir) $(top_srcdir)/elygame/$@.in | \
	$(substShareDir) | \
	$(substLibDir) | \
	$(substCallbacks) | \
	$(substTransitions) | \
	$(substInitializations) | \
	$(substInstanceupdates) > $@

#config.prc rules
if !ELY_THREAD
substMultiThreadRenderPipe = sed -e 's|@multithreadrenderpipe[@]|threading-model Cull\/Draw|g'
else
substMultiThreadRenderPipe = sed -e 's|@multithreadrenderpipe[@]|threading-model|g'
endif 

config.prc : $(top_srcdir)/elygame/config.prc.in Makefile
	$(substDataDir) $(top_srcdir)/elygame/$@.in | \
	$(substMultiThreadRenderPipe) > $@

#game.xml rules
game.xml : $(top_srcdir)/elygame/game.xml.in Makefile
	$(substDataDir) $(top_srcdir)/elygame/$@.in > $@

CLEANFILES = Ely_ini.h config.prc game.xml
		