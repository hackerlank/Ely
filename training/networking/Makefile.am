# Ely training targets
AM_CPPFLAGS = -DTESTING -iquote$(top_srcdir)/include $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

STDCpp98_CPPFLAGS = $(AM_CPPFLAGS) -U__GXX_EXPERIMENTAL_CXX0X__
STDCpp98_CXXFLAGS = $(AM_CXXFLAGS) -std=c++98

AUTOMAKE_OPTIONS = subdir-objects

LDADD = -lp3framework -lpandaai -lpanda -lpandafx -lpandaexpress \
	-lp3dtoolconfig -lp3pystub -lp3dtool -lp3direct \
	-lp3openal_audio -lpandaegg -lp3tinydisplay -lp3vision -lpandagl \
	-lpandaode -lpandaphysics -lpandaskel -lp3ptloader -lpython2.7 \
	-lBulletDynamics -lBulletCollision -lLinearMath -lBulletSoftBody \
	-lRakNet

#ely_client & ely_server
noinst_PROGRAMS = ely_client ely_server
#
ely_client_CPPFLAGS=$(STDCpp98_CPPFLAGS)
ely_client_CXXFLAGS=$(STDCpp98_CXXFLAGS)

nodist_ely_client_SOURCES = \
	client.cpp
#	
ely_server_CPPFLAGS=$(STDCpp98_CPPFLAGS)
ely_server_CXXFLAGS=$(STDCpp98_CXXFLAGS)

nodist_ely_server_SOURCES = \
	server.cpp
