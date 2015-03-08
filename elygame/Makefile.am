DIST_SUBDIRS = callbacks transitions initializations instanceupdates data 

# if !BOOST_TEST and !TRAINING then ELY_ELF 
if ELY_ELF
ELYELFDIRS = $(DIST_SUBDIRS)
else
ELYELFDIRS =
endif 

SUBDIRS = $(ELYELFDIRS)

AM_CPPFLAGS = -iquote$(top_srcdir)/include $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

noinst_HEADERS = \
	elygame.h
	
BUILT_SOURCES = elygame_ini.h

nodist_noinst_HEADERS =\
	elygame_ini.h

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
	$(top_builddir)/src/Support/BulletLocal/libbullet.la \
	$(top_builddir)/src/Support/RecastNavigationLocal/librecastnavigation.la \
	$(top_builddir)/src/Support/LibRocketLocal/librocket.la \
	$(top_builddir)/src/Support/MemoryPool/libmemorypool.la \
	$(top_builddir)/src/Support/tinyxml2/libtinyxml2.la \
	$(top_builddir)/src/Support/libsupport.la \
	$(top_builddir)/src/Support/OpenSteerLocal/libopensteer.la

#elygame
elygame_SOURCES = \
	elygame.cpp \
	elygame_memory_pools.cpp

elygame_CXXFLAGS = $(AM_CXXFLAGS) -frepo

if ELY_DEVEL
ldrpath = \
	-rpath $(abs_top_builddir)/src/Game/.libs \
	-rpath $(abs_top_builddir)/src/AIComponents/.libs \
	-rpath $(abs_top_builddir)/src/AudioComponents/.libs \
	-rpath $(abs_top_builddir)/src/BehaviorComponents/.libs \
	-rpath $(abs_top_builddir)/src/CommonComponents/.libs \
	-rpath $(abs_top_builddir)/src/ControlComponents/.libs \
	-rpath $(abs_top_builddir)/src/PhysicsComponents/.libs \
	-rpath $(abs_top_builddir)/src/PhysicsControlComponents/.libs \
	-rpath $(abs_top_builddir)/src/SceneComponents/.libs \
	-rpath $(abs_top_builddir)/src/ObjectModel/.libs \
	-rpath $(abs_top_builddir)/src/Utilities/.libs \
	-rpath $(abs_top_builddir)/src/Support/BulletLocal/.libs \
	-rpath $(abs_top_builddir)/src/Support/RecastNavigationLocal/.libs \
	-rpath $(abs_top_builddir)/src/Support/LibRocketLocal/.libs \
	-rpath $(abs_top_builddir)/src/Support/MemoryPool/.libs \
	-rpath $(abs_top_builddir)/src/Support/tinyxml2/.libs \
	-rpath $(abs_top_builddir)/src/Support/.libs \
	-rpath $(abs_top_builddir)/src/Support/OpenSteerLocal/.libs
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

EXTRA_DIST = elygame_ini.h.in config.prc.in game.xml.in

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

#elygame_ini.h rules
elygame_ini.h : $(top_srcdir)/elygame/elygame_ini.h.in Makefile
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

CLEANFILES = elygame_ini.h config.prc game.xml
		