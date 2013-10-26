# Ely training targets
AM_CPPFLAGS = -DTESTING -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)
LDADD = $(ELY_LIBS)

SUBDIRS = recastnavigation interrogate

noinst_PROGRAMS = ely_training 

#ely_training
nodist_ely_training_SOURCES = \
		training.cpp \
		ai.cpp \
		anim.cpp \
		network.cpp \
		physics.cpp \
		threads.cpp \
		manual_example.cpp \
		draw_geometry.cpp \
		test_wo_local_scale.cpp \
		meshdrawer.cpp
