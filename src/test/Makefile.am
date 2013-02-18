# Ely Boost Unit Tests
AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)

noinst_PROGRAMS = ely_test
nodist_ely_test_SOURCES =\
		Runner.cpp \
		aicomponents/AISuiteFixture.h \
		aicomponents/Steering_test.cpp \
		audiocomponents/AudioSuiteFixture.h \
		audiocomponents/Sound3d_test.cpp \
		behaviorcomponents/Activity_test.cpp \
		behaviorcomponents/BehaviorSuiteFixture.h \
		controlcomponents/Chaser_test.cpp \
		controlcomponents/ControlSuiteFixture.h \
		controlcomponents/Driver_test.cpp \
		ely/ElySuiteFixture.h \
		ely/Ely_test.cpp \
		game/GameManagers_test.cpp \
		game/GameSuiteFixture.h \
		objectmodel/ComponentTemplateManager_test.cpp \
		objectmodel/ObjectModelSuiteFixture.h \
		objectmodel/ObjectTemplateManager_test.cpp \
		objectmodel/Object_test.cpp \
		physicscomponents/CharacterController_test.cpp \
		physicscomponents/PhysicsSuiteFixture.h \
		physicscomponents/RigidBody_test.cpp \
		scenecomponents/InstanceOf_test.cpp \
		scenecomponents/Model_test.cpp \
		scenecomponents/SceneSuiteFixture.h \
		scenecomponents/Terrain_test.cpp \
		support/FirstPersonCamera_test.cpp \
		support/FSM_test.cpp \
		support/Picker_test.cpp \
		support/SupportSuiteFixture.h

ely_test_CPPFLAGS = -DBOOST_TEST_DYN_LINK -DTESTING $(AM_CPPFLAGS)
ely_test_LDFLAGS = $(ELY_LDFLAGS)
ely_test_LDADD = \
		../libgame.la \
		../libaudiocomponents.la \
		../libaicomponents.la \
		../libbehaviorcomponents.la \
		../libcontrolcomponents.la \
		../libphysicscomponents.la \
		../libscenecomponents.la \
		../libobjectmodel.la \
		../libsupport.la \
		../libutilities.la \
		$(top_srcdir)/tinyxml2/libtinyxml2.la \
		$(ELY_LIBS) \
		-lboost_unit_test_framework
		