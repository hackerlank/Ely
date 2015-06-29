# Ely ely_python_embed targets
#WARNING: build Devel_Debug_Thread configuration first!
AM_CPPFLAGS = -DTESTING -iquote$(top_srcdir)/include $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

STDCpp11_CPPFLAGS = $(AM_CPPFLAGS) -D__GXX_EXPERIMENTAL_CXX0X__
STDCpp11_CXXFLAGS = $(AM_CXXFLAGS) -std=c++11

PYTHON2CFLAGS = -I/usr/include/python2.7 -I/usr/include/x86_64-linux-gnu/python2.7  \
	-fno-strict-aliasing -D_FORTIFY_SOURCE=2 -g -fstack-protector-strong \
	-Wformat -Werror=format-security  -DNDEBUG -g -fwrapv -O2 -Wall \
	-Wstrict-prototypes
PYTHON2LDFLAGS = -L/usr/lib/python2.7/config-x86_64-linux-gnu -L/usr/lib \
	-lpython2.7 -lpthread -ldl  -lutil -lm  -Xlinker -export-dynamic \
	-Wl,-O1 -Wl,-Bsymbolic-functions

AUTOMAKE_OPTIONS = subdir-objects

LDADD = -lp3framework -lpandaai -lpanda -lpandafx -lpandaexpress \
	-lp3dtoolconfig -lp3pystub -lp3dtool -lp3direct -lpandabullet -lp3rocket \
	-lp3openal_audio -lpandaegg -lp3tinydisplay -lp3vision -lpandagl \
	-lpandaode -lpandaphysics -lpandaskel -lp3ptloader -lpython2.7 \
	-lBulletDynamics -lBulletCollision -lLinearMath -lBulletSoftBody \
	-lDebugUtils -lDetour -lDetourCrowd -lDetourTileCache -lRecast -lOpenSteer \
	$(top_srcdir)/build-Devel_Debug_Thread/src/.libs/libely.a
	
#ely_python_embed
noinst_PROGRAMS = ely_python_embed 
ely_python_embed_CPPFLAGS=-DELY_THREAD  $(STDCpp11_CPPFLAGS) $(PYTHON2CFLAGS)
ely_python_embed_CXXFLAGS=$(STDCpp11_CXXFLAGS) $(PYTHON2LDFLAGS)

nodist_ely_python_embed_SOURCES = \
	python_embed.cpp \
	Sound3d1.cpp \
	Sound3d1Py.cpp
