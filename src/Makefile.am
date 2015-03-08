#last one must be .
SUBDIRS = \
	Support \
	Utilities \
	AIComponents \
	AudioComponents \
	BehaviorComponents \
	ControlComponents \
	CommonComponents \
	Game \
	ObjectModel \
	PhysicsComponents \
	PhysicsControlComponents \
	SceneComponents \
	.

#ely library
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS) -version-info 1:0:0

LIBS = $(ELY_LIBS)

AUTOMAKE_OPTIONS = subdir-objects

#ltlibraries: libely.la
pkglib_LTLIBRARIES = \
	libely.la

libely_la_LIBADD = \
	$(builddir)/Support/libSupport.la \
	$(builddir)/Utilities/libUtilities.la \
	$(builddir)/AIComponents/libAIComponents.la \
	$(builddir)/AudioComponents/libAudioComponents.la \
	$(builddir)/BehaviorComponents/libBehaviorComponents.la \
	$(builddir)/ControlComponents/libControlComponents.la \
	$(builddir)/CommonComponents/libCommonComponents.la \
	$(builddir)/Game/libGame.la \
	$(builddir)/ObjectModel/libObjectModel.la \
	$(builddir)/PhysicsComponents/libPhysicsComponents.la \
	$(builddir)/PhysicsControlComponents/libPhysicsControlComponents.la \
	$(builddir)/SceneComponents/libSceneComponents.la

libely_la_SOURCES =
