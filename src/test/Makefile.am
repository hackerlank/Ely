# Ely Boost Unit Tests
AM_CPPFLAGS = -DBOOST_TEST_DYN_LINK -DTESTING -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)

noinst_LTLIBRARIES = \
		libtestaicomponents.la \
		libtestaudiocomponents.la \
		libtestbehaviorcomponents.la \
		libtestcontrolcomponents.la \
		libtestely.la \
		libtestphysicscomponents.la \
		libtestscenecomponents.la \
		libtestgame.la \
		libtestobjectmodel.la \
		libtestsupport.la

noinst_PROGRAMS = ely_test
nodist_ely_test_SOURCES =\
		Runner.cpp \
		$(top_srcdir)/tinyxml2/tinyxml2.cpp \
		$(top_srcdir)/src/Utilities/Tools.cpp
		 
ely_test_LDFLAGS = $(ELY_LDFLAGS)
ely_test_LDADD = \
		$(noinst_LTLIBRARIES) \
		-lboost_unit_test_framework \
		$(ELY_LIBS)

libtestaicomponents_la_SOURCES = \
		aicomponents/AISuiteFixture.h \
		aicomponents/Steering_test.cpp \
		$(top_srcdir)/src/AIComponents/Steering.cpp  \
		$(top_srcdir)/src/AIComponents/SteeringTemplate.cpp
		
libtestaudiocomponents_la_SOURCES = \
		audiocomponents/AudioSuiteFixture.h \
		audiocomponents/Sound3d_test.cpp \
		$(top_srcdir)/src/AudioComponents/Listener.cpp \
		$(top_srcdir)/src/AudioComponents/ListenerTemplate.cpp \
		$(top_srcdir)/src/AudioComponents/Sound3d.cpp \
		$(top_srcdir)/src/AudioComponents/Sound3dTemplate.cpp

libtestbehaviorcomponents_la_SOURCES = \
		behaviorcomponents/Activity_test.cpp \
		behaviorcomponents/BehaviorSuiteFixture.h \
		$(top_srcdir)/src/BehaviorComponents/Activity.cpp \
		$(top_srcdir)/src/BehaviorComponents/ActivityTemplate.cpp

libtestcontrolcomponents_la_SOURCES = \
		controlcomponents/Chaser_test.cpp \
		controlcomponents/ControlSuiteFixture.h \
		controlcomponents/Driver_test.cpp \
		$(top_srcdir)/src/ControlComponents/Chaser.cpp \
		$(top_srcdir)/src/ControlComponents/ChaserTemplate.cpp \
		$(top_srcdir)/src/ControlComponents/Driver.cpp \
		$(top_srcdir)/src/ControlComponents/DriverTemplate.cpp

libtestely_la_SOURCES = \
		ely/ElySuiteFixture.h \
		ely/Ely_test.cpp

libtestgame_la_SOURCES = \
		game/GameManagers_test.cpp \
		game/GameSuiteFixture.h \
		$(top_srcdir)/src/Game/GameAIManager.cpp \
		$(top_srcdir)/src/Game/GameAudioManager.cpp \
		$(top_srcdir)/src/Game/GameControlManager.cpp \
		$(top_srcdir)/src/Game/GamePhysicsManager.cpp \
		$(top_srcdir)/src/Game/GameSceneManager.cpp \
		$(top_srcdir)/src/Game/GameManager.cpp

libtestobjectmodel_la_SOURCES = \
		objectmodel/ComponentTemplateManager_test.cpp \
		objectmodel/ObjectModelSuiteFixture.h \
		objectmodel/ObjectTemplateManager_test.cpp \
		objectmodel/Object_test.cpp \
		$(top_srcdir)/src/ObjectModel/Component.cpp \
		$(top_srcdir)/src/ObjectModel/ComponentTemplate.cpp \
		$(top_srcdir)/src/ObjectModel/ComponentTemplateManager.cpp \
		$(top_srcdir)/src/ObjectModel/Object.cpp \
		$(top_srcdir)/src/ObjectModel/ObjectTemplate.cpp \
		$(top_srcdir)/src/ObjectModel/ObjectTemplateManager.cpp

libtestphysicscomponents_la_SOURCES = \
		physicscomponents/CharacterController_test.cpp \
		physicscomponents/PhysicsSuiteFixture.h \
		physicscomponents/RigidBody_test.cpp \
		$(top_srcdir)/src/PhysicsComponents/CharacterController.cpp \
		$(top_srcdir)/src/PhysicsComponents/CharacterControllerTemplate.cpp \
		$(top_srcdir)/src/PhysicsComponents/RigidBody.cpp \
		$(top_srcdir)/src/PhysicsComponents/RigidBodyTemplate.cpp

libtestscenecomponents_la_SOURCES = \
		scenecomponents/InstanceOf_test.cpp \
		scenecomponents/Model_test.cpp \
		scenecomponents/SceneSuiteFixture.h \
		scenecomponents/Terrain_test.cpp \
		$(top_srcdir)/src/SceneComponents/InstanceOf.cpp \
		$(top_srcdir)/src/SceneComponents/InstanceOfTemplate.cpp \
		$(top_srcdir)/src/SceneComponents/Model.cpp \
		$(top_srcdir)/src/SceneComponents/ModelTemplate.cpp \
		$(top_srcdir)/src/SceneComponents/NodePathWrapper.cpp \
		$(top_srcdir)/src/SceneComponents/NodePathWrapperTemplate.cpp \
		$(top_srcdir)/src/SceneComponents/Terrain.cpp \
		$(top_srcdir)/src/SceneComponents/TerrainTemplate.cpp

libtestsupport_la_SOURCES = \
		support/FirstPersonCamera_test.cpp \
		support/FSM_test.cpp \
		support/Picker_test.cpp \
		support/SupportSuiteFixture.h \
		$(top_srcdir)/src/Support/FirstPersonCamera.cpp \
		$(top_srcdir)/src/Support/FSM.cpp \
		$(top_srcdir)/src/Support/Picker.cpp \
		$(top_srcdir)/src/Support/Distributed/ClientRepositoryBase.cpp \
		$(top_srcdir)/src/Support/Distributed/DistributedObjectBase.cpp
