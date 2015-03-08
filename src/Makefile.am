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
lib_LTLIBRARIES = \
	libely.la

libely_la_LIBADD = \
	$(builddir)/Support/libSupport.a \
	$(builddir)/Utilities/libUtilities.a \
	$(builddir)/AIComponents/libAIComponents.a \
	$(builddir)/AudioComponents/libAudioComponents.a \
	$(builddir)/BehaviorComponents/libBehaviorComponents.a \
	$(builddir)/ControlComponents/libControlComponents.a \
	$(builddir)/CommonComponents/libCommonComponents.a \
	$(builddir)/Game/libGame.a \
	$(builddir)/ObjectModel/libObjectModel.a \
	$(builddir)/PhysicsComponents/libPhysicsComponents.a \
	$(builddir)/PhysicsControlComponents/libPhysicsControlComponents.a \
	$(builddir)/SceneComponents/libSceneComponents.a

libely_la_SOURCES =
