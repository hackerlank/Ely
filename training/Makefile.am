# Ely training target
AM_CPPFLAGS = -DTESTING -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)

noinst_PROGRAMS = ely_training
nodist_ely_training_SOURCES = \
		training.cpp \
		ai.cpp \
		anim.cpp \
		network.cpp \
		physics.cpp \
		threads.cpp \
		manual_example.cpp
		
ely_training_LDFLAGS = $(ELY_LDFLAGS)
ely_training_LDADD = \
		$(ELY_LIBS)
