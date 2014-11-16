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
		$(top_builddir)/src/Game/libgame.la \
		$(top_builddir)/src/AIComponents/libaicomponents.la \
		$(top_builddir)/src/AudioComponents/libaudiocomponents.la \
		$(top_builddir)/src/BehaviorComponents/libbehaviorcomponents.la \
		$(top_builddir)/src/CommonComponents/libcommoncomponents.la \
		$(top_builddir)/src/ControlComponents/libcontrolcomponents.la \
		$(top_builddir)/src/PhysicsComponents/libphysicscomponents.la \
		$(top_builddir)/src/PhysicsControlComponents/libphysicscontrolcomponents.la \
		$(top_builddir)/src/SceneComponents/libscenecomponents.la \
		$(top_builddir)/src/ObjectModel/libobjectmodel.la \
		$(top_builddir)/src/Utilities/libutilities.la \
		$(top_builddir)/src/Support/libbullet.la \
		$(top_builddir)/src/Support/librecastnavigation.la \
		$(top_builddir)/src/Support/librocket.la \
		$(top_builddir)/src/Support/libtinyxml2.la \
		$(top_builddir)/src/Support/libsupport.la \
		$(top_builddir)/src/Support/libopensteer.la

#elygame
elygame_SOURCES = \
		Ely.cpp

if ELY_DEVEL
ldrpath = -rpath $(abs_top_builddir)/src/.libs
else
ldrpath = -rpath $(pkglibdir)
endif

elygame_LDFLAGS = $(AM_LDFLAGS) -export-dynamic $(ldrpath)
		
elygame_LDADD = \
		-dlopen $(builddir)/callbacks/callbacks.la \
		-dlopen $(builddir)/initializations/initializations.la \
		-dlopen $(builddir)/instanceupdates/instanceupdates.la \
		-dlopen $(builddir)/transitions/transitions.la \
		$(elygame_ltlibraries) \
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
		