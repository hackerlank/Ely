# Ely Boost Unit Tests
DEFINITIONS = -DTRANSITIONS_LA=\"dummy\" -DCALLBACKS_LA=\"dummy\" -DINITIALIZATIONS_LA=\"dummy\" -DDEFAULT_CALLBACK_NAME=\"dummy\"
AM_CPPFLAGS = $(DEFINITIONS) -DELY_GAMEXML=\"ELY_GAMEXML\" -DBOOST_TEST_DYN_LINK -DTESTING -iquote$(top_srcdir)/include -iquote$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

AUTOMAKE_OPTIONS = subdir-objects

noinst_LIBRARIES = \
		libtestaicomponents.a \
		libtestaudiocomponents.a \
		libtestbehaviorcomponents.a \
		libtestcommoncomponents.a \
		libtestcontrolcomponents.a \
		libtestely.a \
		libtestphysicscomponents.a \
		libtestphysicscontrolcomponents.a \
		libtestscenecomponents.a \
		libtestgame.a \
		libtestobjectmodel.a \
		libtestsupport.a
whole_archives = \
		-Wl,libtestaicomponents.a \
		-Wl,libtestaudiocomponents.a \
		-Wl,libtestbehaviorcomponents.a \
		-Wl,libtestcommoncomponents.a \
		-Wl,libtestcontrolcomponents.a \
		-Wl,libtestely.a \
		-Wl,libtestphysicscomponents.a \
		-Wl,libtestphysicscontrolcomponents.a \
		-Wl,libtestscenecomponents.a \
		-Wl,libtestgame.a \
		-Wl,libtestobjectmodel.a \
		-Wl,libtestsupport.a

noinst_PROGRAMS = ely_test

#ely_test
nodist_ely_test_SOURCES =\
		Runner.cpp \
		$(top_srcdir)/tinyxml2/tinyxml2.cpp \
		$(top_srcdir)/src/Utilities/Tools.cpp	 
ely_test_LDFLAGS = $(AM_LDFLAGS) \
		-Wl,--whole-archive \
		$(whole_archives) \
		-Wl,--no-whole-archive
ely_test_LDADD = \
		$(noinst_LIBRARIES) \
		-lboost_unit_test_framework \
		$(ELY_LIBS)

libtestaicomponents_a_SOURCES = \
		aicomponents/AISuiteFixture.h \
		aicomponents/CrowdAgent_test.cpp \
		aicomponents/NavMesh_test.cpp \
		aicomponents/SteerPlugIn_test.cpp \
		aicomponents/SteerVehicle_test.cpp \
		$(top_srcdir)/src/AIComponents/CrowdAgent.cpp \
		$(top_srcdir)/src/AIComponents/CrowdAgentTemplate.cpp \
		$(top_srcdir)/src/AIComponents/NavMesh.cpp \
		$(top_srcdir)/src/AIComponents/NavMeshTemplate.cpp
		
libtestaudiocomponents_a_SOURCES = \
		audiocomponents/AudioSuiteFixture.h \
		audiocomponents/Listener_test.cpp \
		audiocomponents/Sound3d_test.cpp \
		$(top_srcdir)/src/AudioComponents/Listener.cpp \
		$(top_srcdir)/src/AudioComponents/ListenerTemplate.cpp \
		$(top_srcdir)/src/AudioComponents/Sound3d.cpp \
		$(top_srcdir)/src/AudioComponents/Sound3dTemplate.cpp

libtestbehaviorcomponents_a_SOURCES = \
		behaviorcomponents/BehaviorSuiteFixture.h \
		behaviorcomponents/Activity_test.cpp \
		$(top_srcdir)/src/BehaviorComponents/Activity.cpp \
		$(top_srcdir)/src/BehaviorComponents/ActivityTemplate.cpp

libtestcommoncomponents_a_SOURCES = \
		commoncomponents/CommonSuiteFixture.h \
		commoncomponents/Default_test.cpp \
		commoncomponents/GameConfig_test.cpp \
		$(top_srcdir)/src/CommonComponents/Default.cpp \
		$(top_srcdir)/src/CommonComponents/DefaultTemplate.cpp \
		$(top_srcdir)/src/CommonComponents/GameConfig.cpp \
		$(top_srcdir)/src/CommonComponents/GameConfigTemplate.cpp

libtestcontrolcomponents_a_SOURCES = \
		controlcomponents/ControlSuiteFixture.h \
		controlcomponents/Chaser_test.cpp \
		controlcomponents/Driver_test.cpp \
		$(top_srcdir)/src/ControlComponents/Chaser.cpp \
		$(top_srcdir)/src/ControlComponents/ChaserTemplate.cpp \
		$(top_srcdir)/src/ControlComponents/Driver.cpp \
		$(top_srcdir)/src/ControlComponents/DriverTemplate.cpp

libtestely_a_SOURCES = \
		ely/ElySuiteFixture.h \
		ely/Ely_test.cpp

libtestgame_a_SOURCES = \
		game/GameSuiteFixture.h \
		game/GameManagers_test.cpp \
		$(top_srcdir)/src/Game/GameAIManager.cpp \
		$(top_srcdir)/src/Game/GameAudioManager.cpp \
		$(top_srcdir)/src/Game/GameBehaviorManager.cpp \
		$(top_srcdir)/src/Game/GameControlManager.cpp \
		$(top_srcdir)/src/Game/GameManager.cpp \
		$(top_srcdir)/src/Game/GamePhysicsManager.cpp \
		$(top_srcdir)/src/Game/GameSceneManager.cpp

libtestobjectmodel_a_SOURCES = \
		objectmodel/ObjectModelSuiteFixture.h \
		objectmodel/ComponentTemplateManager_test.cpp \
		objectmodel/ObjectTemplateManager_test.cpp \
		objectmodel/Object_test.cpp \
		$(top_srcdir)/src/ObjectModel/Component.cpp \
		$(top_srcdir)/src/ObjectModel/ComponentTemplate.cpp \
		$(top_srcdir)/src/ObjectModel/ComponentTemplateManager.cpp \
		$(top_srcdir)/src/ObjectModel/Object.cpp \
		$(top_srcdir)/src/ObjectModel/ObjectTemplate.cpp \
		$(top_srcdir)/src/ObjectModel/ObjectTemplateManager.cpp

libtestphysicscomponents_a_SOURCES = \
		physicscomponents/PhysicsSuiteFixture.h \
		physicscomponents/Ghost_test.cpp \
		physicscomponents/RigidBody_test.cpp \
		physicscomponents/SoftBody_test.cpp \
		$(top_srcdir)/src/PhysicsComponents/Ghost.cpp \
		$(top_srcdir)/src/PhysicsComponents/GhostTemplate.cpp \
		$(top_srcdir)/src/PhysicsComponents/RigidBody.cpp \
		$(top_srcdir)/src/PhysicsComponents/RigidBodyTemplate.cpp \
		$(top_srcdir)/src/PhysicsComponents/SoftBody.cpp \
		$(top_srcdir)/src/PhysicsComponents/SoftBodyTemplate.cpp

libtestphysicscontrolcomponents_a_SOURCES = \
		physicscomponents/PhysicsControlSuiteFixture.h \
		physicscomponents/CharacterController_test.cpp \
		physicscomponents/Vehicle_test.cpp \
		$(top_srcdir)/src/PhysicsControlComponents/CharacterController.cpp \
		$(top_srcdir)/src/PhysicsControlComponents/CharacterControllerTemplate.cpp \
		$(top_srcdir)/src/PhysicsControlComponents/Vehicle.cpp \
		$(top_srcdir)/src/PhysicsControlComponents/VehicleTemplate.cpp

libtestscenecomponents_a_SOURCES = \
		scenecomponents/SceneSuiteFixture.h \
		scenecomponents/InstanceOf_test.cpp \
		scenecomponents/Model_test.cpp \
		scenecomponents/Terrain_test.cpp \
		$(top_srcdir)/src/SceneComponents/InstanceOf.cpp \
		$(top_srcdir)/src/SceneComponents/InstanceOfTemplate.cpp \
		$(top_srcdir)/src/SceneComponents/Model.cpp \
		$(top_srcdir)/src/SceneComponents/ModelTemplate.cpp \
		$(top_srcdir)/src/SceneComponents/NodePathWrapper.cpp \
		$(top_srcdir)/src/SceneComponents/NodePathWrapperTemplate.cpp \
		$(top_srcdir)/src/SceneComponents/Terrain.cpp \
		$(top_srcdir)/src/SceneComponents/TerrainTemplate.cpp

libtestsupport_a_SOURCES = \
		support/SupportSuiteFixture.h \
		support/FirstPersonCamera_test.cpp \
		support/FSM_test.cpp \
		support/Picker_test.cpp \
		support/RayCaster_test.cpp \
		support/Distributed_test.cpp \
		$(top_srcdir)/src/Support/FirstPersonCamera.cpp \
		$(top_srcdir)/src/Support/FSM.cpp \
		$(top_srcdir)/src/Support/Picker.cpp \
		$(top_srcdir)/src/Support/RayCaster.cpp \
		$(top_srcdir)/src/Support/Distributed/ClientRepositoryBase.cpp \
		$(top_srcdir)/src/Support/Distributed/DistributedObjectBase.cpp
