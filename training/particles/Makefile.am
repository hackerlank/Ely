AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = --std=c++11 $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = 

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LTLIBRARIES = \
	libparticles.la

#library sources
libparticles_la_SOURCES = \
	ForceGroup.cpp \
	GameParticlesManager.cpp \
	GlobalForceGroup.cpp \
	ParticleEffect.cpp \
	Particles.cpp \
	SpriteParticleRendererExt.cpp

nodist_libparticles_la_SOURCES = \
	Scannerbase.h \
	lex.cc \
	Scanner.h \
	Scanner.ih

BUILT_SOURCES = $(nodist_libparticles_la_SOURCES)

#nodist_libparticles_la_SOURCES rules
$(nodist_libparticles_la_SOURCES) : $(srcdir)/lexer $(srcdir)/Scanner_Skel.h $(srcdir)/Scanner_Skel.ih
	@echo "Creating $@ ..."
	-rm -f $(nodist_libparticles_la_SOURCES)
	-$(ELY_FLEXCPP) --class-skeleton=$(srcdir)/Scanner_Skel.h \
		--implementation-skeleton=$(srcdir)/Scanner_Skel.ih \
		--namespace='ely' $(srcdir)/lexer

EXTRA_DIST = lexer Scanner_Skel.h Scanner_Skel.ih

CLEANFILES = $(nodist_libparticles_la_SOURCES)
