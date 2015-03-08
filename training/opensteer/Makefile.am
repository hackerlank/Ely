# Ely opensteer targets
AM_CPPFLAGS = -DTESTING -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)
LDADD = -lp3framework -lpandaai -lpanda -lpandafx -lpandaexpress \
	-lp3dtoolconfig -lp3pystub -lp3dtool -lp3direct -lpandabullet \
	-lp3openal_audio -lpandaegg -lp3tinydisplay -lp3vision -lpandagl \
	-lpandaode -lpandaphysics -lpandaskel -lp3ptloader -lOpenSteer

noinst_LTLIBRARIES = libopensteerplugins.la

libopensteerplugins_la_SOURCES = \
	Draw.cpp \
	DrawMeshDrawer.cpp \
	LowSpeedTurn.cpp \
	Pedestrian.cpp \
	PedestriansWalkingAnEight.cpp \
	CaptureTheFlag.cpp \
	Boids.cpp \
	Soccer.cpp \
	MapDrive.cpp
		
noinst_PROGRAMS = ely_opensteer
		
#ely_opensteer
nodist_ely_opensteer_SOURCES = \
	main.cpp

ely_opensteer_LDADD = \
	$(LDADD) \
	libopensteerplugins.la
		