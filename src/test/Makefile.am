# Ely Boost Unit Tests
AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)

noinst_LTLIBRARIES = \
		testgame.la \
		testaicomponents.la \
		testaudiocomponents.la \
		testbehaviorcomponents.la \
		testcontrolcomponents.la \
		testphysicscomponents.la \
		testscenecomponents.la \
		testobjectmodel.la \
		testutilities.la \
		testsupport.la

noinst_PROGRAMS = ely_test
nodist_ely_test_SOURCES =\
		Runner.cpp \
		$(sources_to_test) \
		$(testing_sources)
		 
ely_test_CPPFLAGS = -DBOOST_TEST_DYN_LINK -DTESTING $(AM_CPPFLAGS)
ely_test_LDFLAGS = $(ELY_LDFLAGS)
ely_test_LDADD = \
		$(ELY_LIBS) \
		-lboost_unit_test_framework

testaicomponents_la_SOURCES = \
		aicomponents/AISuiteFixture.h \
		aicomponents/Steering_test.cpp

testaudiocomponents_la_SOURCES = \
		AudioComponents/Listener.cpp \
		AudioComponents/ListenerTemplate.cpp \
		AudioComponents/Sound3d.cpp \
		AudioComponents/Sound3dTemplate.cpp

testbehaviorcomponents_la_SOURCES = \
		BehaviorComponents/Activity.cpp \
		BehaviorComponents/ActivityTemplate.cpp

testcontrolcomponents_la_SOURCES = \
		ControlComponents/Chaser.cpp \
		ControlComponents/ChaserTemplate.cpp \
		ControlComponents/Driver.cpp \
		ControlComponents/DriverTemplate.cpp

testgame_la_SOURCES = \
		Game/GameAIManager.cpp \
		Game/GameAudioManager.cpp \
		Game/GameControlManager.cpp \
		Game/GameManager.cpp \
		Game/GamePhysicsManager.cpp \
		Game/GameSceneManager.cpp

testobjectmodel_la_SOURCES = \
		ObjectModel/Component.cpp \
		ObjectModel/ComponentTemplate.cpp \
		ObjectModel/ComponentTemplateManager.cpp \
		ObjectModel/Object.cpp \
		ObjectModel/ObjectTemplate.cpp \
		ObjectModel/ObjectTemplateManager.cpp

testphysicscomponents_la_SOURCES = \
		PhysicsComponents/CharacterController.cpp \
		PhysicsComponents/CharacterControllerTemplate.cpp \
		PhysicsComponents/RigidBody.cpp \
		PhysicsComponents/RigidBodyTemplate.cpp

testscenecomponents_la_SOURCES = \
		SceneComponents/InstanceOf.cpp \
		SceneComponents/InstanceOfTemplate.cpp \
		SceneComponents/Model.cpp \
		SceneComponents/ModelTemplate.cpp \
		SceneComponents/NodePathWrapper.cpp \
		SceneComponents/NodePathWrapperTemplate.cpp \
		SceneComponents/Terrain.cpp \
		SceneComponents/TerrainTemplate.cpp

testsupport_la_SOURCES = \
		Support/FirstPersonCamera.cpp \
		Support/FSM.cpp \
		Support/Picker.cpp \
		Support/Distributed/ClientRepositoryBase.cpp \
		Support/Distributed/DistributedObjectBase.cpp

testutilities_la_SOURCES = \
		Utilities/Tools.cpp
		#######################################################
		######################################################
sources_to_test = \
		$(top_srcdir)/tinyxml2/tinyxml2.cpp \
		$(top_srcdir)/src/AIComponents/Steering.cpp  \
		$(top_srcdir)/src/AIComponents/SteeringTemplate.cpp \
		$(top_srcdir)/src/AudioComponents/Listener.cpp \
		$(top_srcdir)/src/AudioComponents/ListenerTemplate.cpp \
		$(top_srcdir)/src/AudioComponents/Sound3d.cpp \
		$(top_srcdir)/src/AudioComponents/Sound3dTemplate.cpp \
		$(top_srcdir)/src/BehaviorComponents/Activity.cpp \
		$(top_srcdir)/src/BehaviorComponents/ActivityTemplate.cpp \
		$(top_srcdir)/src/ControlComponents/Chaser.cpp \
		$(top_srcdir)/src/ControlComponents/ChaserTemplate.cpp \
		$(top_srcdir)/src/ControlComponents/Driver.cpp \
		$(top_srcdir)/src/ControlComponents/DriverTemplate.cpp \
		$(top_srcdir)/src/Game/GameAIManager.cpp \
		$(top_srcdir)/src/Game/GameAudioManager.cpp \
		$(top_srcdir)/src/Game/GameControlManager.cpp \
		$(top_srcdir)/src/Game/GameManager.cpp \
		$(top_srcdir)/src/Game/GamePhysicsManager.cpp \
		$(top_srcdir)/src/Game/GameSceneManager.cpp \
		$(top_srcdir)/src/ObjectModel/Component.cpp \
		$(top_srcdir)/src/ObjectModel/ComponentTemplate.cpp \
		$(top_srcdir)/src/ObjectModel/ComponentTemplateManager.cpp \
		$(top_srcdir)/src/ObjectModel/Object.cpp \
		$(top_srcdir)/src/ObjectModel/ObjectTemplate.cpp \
		$(top_srcdir)/src/ObjectModel/ObjectTemplateManager.cpp \
		$(top_srcdir)/src/PhysicsComponents/CharacterController.cpp \
		$(top_srcdir)/src/PhysicsComponents/CharacterControllerTemplate.cpp \
		$(top_srcdir)/src/PhysicsComponents/RigidBody.cpp \
		$(top_srcdir)/src/PhysicsComponents/RigidBodyTemplate.cpp \
		$(top_srcdir)/src/SceneComponents/InstanceOf.cpp \
		$(top_srcdir)/src/SceneComponents/InstanceOfTemplate.cpp \
		$(top_srcdir)/src/SceneComponents/Model.cpp \
		$(top_srcdir)/src/SceneComponents/ModelTemplate.cpp \
		$(top_srcdir)/src/SceneComponents/NodePathWrapper.cpp \
		$(top_srcdir)/src/SceneComponents/NodePathWrapperTemplate.cpp \
		$(top_srcdir)/src/SceneComponents/Terrain.cpp \
		$(top_srcdir)/src/SceneComponents/TerrainTemplate.cpp \
		$(top_srcdir)/src/Support/FirstPersonCamera.cpp \
		$(top_srcdir)/src/Support/FSM.cpp \
		$(top_srcdir)/src/Support/Picker.cpp \
		$(top_srcdir)/src/Support/Distributed/ClientRepositoryBase.cpp \
		$(top_srcdir)/src/Support/Distributed/DistributedObjectBase.cpp \
		$(top_srcdir)/src/Utilities/Tools.cpp

testing_sources = \
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
		
		