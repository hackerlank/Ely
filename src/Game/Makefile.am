AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = $(ELY_LIBS)

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LIBRARIES = \
	libGame.a

#library sources
libGame_a_SOURCES = \
	GameAIManager.cpp \
	GameAudioManager.cpp \
	GameBehaviorManager.cpp \
	GameControlManager.cpp \
	GameManager.cpp \
	GamePhysicsManager.cpp \
	GameSceneManager.cpp
