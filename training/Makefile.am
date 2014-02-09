# Ely training targets
AM_CPPFLAGS = -DTESTING -iquote$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)
LDADD = -lp3framework -lpandaai -lpanda -lpandafx -lpandaexpress \
	-lp3dtoolconfig -lp3pystub -lp3dtool -lp3direct -lpandabullet \
	-lp3openal_audio -lpandaegg -lp3tinydisplay -lp3vision -lpandagl \
	-lpandaode -lpandaphysics -lpandaskel -lp3ptloader

#SUBDIRS = recastnavigation
#SUBDIRS = interrogate
#SUBDIRS = opensteer

noinst_PROGRAMS = ely_training 

#ely_training
nodist_ely_training_SOURCES = \
		training.cpp \
		generic_test.cpp \
		ai.cpp \
		anim.cpp \
		network.cpp \
		physics.cpp \
		threads.cpp \
		manual_example.cpp \
		draw_geometry.cpp \
		test_wo_local_scale.cpp \
		meshdrawer.cpp \
		pgGUI.cpp
