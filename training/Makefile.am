# Ely training targets
AM_CPPFLAGS = -DTESTING -iquote$(top_srcdir)/include $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

AUTOMAKE_OPTIONS = subdir-objects

LDADD = -lp3framework -lpandaai -lpanda -lpandafx -lpandaexpress \
	-lp3dtoolconfig -lp3pystub -lp3dtool -lp3direct \
	-lp3openal_audio -lpandaegg -lp3tinydisplay -lp3vision -lpandagl \
	-lpandaode -lpandaphysics -lpandaskel -lp3ptloader -lpython2.7 \
	-lBulletDynamics -lBulletCollision -lLinearMath -lBulletSoftBody \
	-lRakNet

#SUBDIRS = recastnavigation
#SUBDIRS = interrogate
#SUBDIRS = opensteer

noinst_PROGRAMS = ely_training 

#ely_training
nodist_ely_training_SOURCES = \
	training.cpp \
	terrain_sample.cpp
#	raknet.cpp
#	octree.cpp
#	memory_pool.cpp
#	memory_pool/MemoryPool.cpp
#	generic_test.cpp
#	ai.cpp
#	anim.cpp
#	network.cpp
#	threads.cpp
#	manual_example.cpp
#	draw_geometry.cpp
#	meshdrawer.cpp
#	rocket.cpp
#	render-to-texture.cpp
#	physics.cpp
		