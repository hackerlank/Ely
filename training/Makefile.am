AM_CPPFLAGS = -I$(top_srcdir)/include
AM_CXXFLAGS = $(ELY_CXXFLAGS)

noinst_PROGRAMS = training
nodist_training_SOURCES = \
		training.cpp \
		ai.cpp \
		anim.cpp \
		network.cpp \
		physics.cpp \
		threads.cpp
		
training_LDFLAGS = $(ELY_LDFLAGS)
training_LDADD = \
		$(ELY_LIBS)