# Ely opensteer targets
AM_CPPFLAGS = -DTESTING -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)
LDADD = $(ELY_LIBS) -lOpenSteer -lOpenSteerPlugins

noinst_PROGRAMS = ely_opensteer
		
#ely_opensteer
nodist_ely_opensteer_SOURCES = \
		main.cpp \
		OpenSteerDemo.cpp \
		Draw.cpp
