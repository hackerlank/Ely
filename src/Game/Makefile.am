AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = 

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LTLIBRARIES = \
	libGame.la

#library sources
libGame_la_SOURCES = \
	GameAIManager.cpp \
	GameAudioManager.cpp \
	GameBehaviorManager.cpp \
	GameControlManager.cpp \
	GameGUIManager.cpp \
	GameManager.cpp \
	GamePhysicsManager.cpp \
	GameSceneManager.cpp
