# Ely training targets
AM_CPPFLAGS = -DTESTING -iquote$(top_srcdir)/include $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

STDCpp11_CPPFLAGS = $(AM_CPPFLAGS) -D__GXX_EXPERIMENTAL_CXX0X__
STDCpp11_CXXFLAGS = $(AM_CXXFLAGS) -std=c++11

AUTOMAKE_OPTIONS = subdir-objects

LDADD = -lp3framework -lpanda -lpandaexpress -lp3dtool -lp3dtoolconfig -lp3pystub \
		-lpandabullet -lp3rocket -lXxf86dga -lpython2.7 -lpandaphysics
#    -lpandaai -lpandafx -lp3direct -lp3openal_audio -lpandaegg -lp3tinydisplay \
#    -lp3vision -lpandagl -lpandaode -lpandaphysics -lpandaskel -lp3ptloader  \
#    -lBulletDynamics -lBulletCollision -lLinearMath -lBulletSoftBody -lRakNet

#SUBDIRS = recastnavigation
#SUBDIRS = interrogate
#SUBDIRS = opensteer
#SUBDIRS = networking

#particles
#SUBDIRS = particles
#ely_training_LDADD = $(builddir)/particles/libparticles.la $(LDADD)
#ely_training_LDFLAGS = $(AM_LDFLAGS) -rpath $(pkglibdir)
#

#ely_training
noinst_PROGRAMS = ely_training 
ely_training_CPPFLAGS=$(STDCpp11_CPPFLAGS)
ely_training_CXXFLAGS=$(STDCpp11_CXXFLAGS)

nodist_ely_training_SOURCES = \
	training.cpp \
	octree.cpp

## available sources
#	particles.cpp
#	ai.cpp
#	anim.cpp
#	draw_geometry.cpp
#	generic_test.cpp
#	manual_example.cpp
#	memory_pool.cpp
#	memory_pool/MemoryPool.cpp
#	meshdrawer.cpp
#	network.cpp
#	octree.cpp
#	physics.cpp
#	render-to-texture.cpp
#	rocket.cpp
#	threads.cpp
#	terrain_sample.cpp
		