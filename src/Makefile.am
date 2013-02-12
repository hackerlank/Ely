AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)

noinst_LIBRARIES = \
		libgame.a \
		libaicomponents.a \
		libaudiocomponents.a \
		libbehaviorcomponents.a \
		libcontrolcomponents.a \
		libphysicscomponents.a \
		libscenecomponents.a \
		libobjectmodel.a \
		libutilities.a \
		libsupport.a

bin_PROGRAMS = ely
ely_SOURCES = \
		Ely.cpp
ely_LDFLAGS = -L$(top_srcdir)/tinyxml2 $(ELY_LDFLAGS)
ely_LDADD = \
		$(noinst_LIBRARIES) \
		-ltinyxml2

libaicomponents_a_SOURCES = \
		AIComponents/Steering.cpp  \
		AIComponents/SteeringTemplate.cpp

libaudiocomponents_a_SOURCES = \
		AudioComponents/Listener.cpp \
		AudioComponents/ListenerTemplate.cpp \
		AudioComponents/Sound3d.cpp \
		AudioComponents/Sound3dTemplate.cpp

libbehaviorcomponents_a_SOURCES = \
		BehaviorComponents/Activity.cpp \
		BehaviorComponents/ActivityTemplate.cpp

libcontrolcomponents_a_SOURCES = \
		ControlComponents/Chaser.cpp \
		ControlComponents/ChaserTemplate.cpp \
		ControlComponents/Driver.cpp \
		ControlComponents/DriverTemplate.cpp

libgame_a_SOURCES = \
		Game/GameAIManager.cpp \
		Game/GameAudioManager.cpp \
		Game/GameControlManager.cpp \
		Game/GameManager.cpp \
		Game/GamePhysicsManager.cpp \
		Game/GameSceneManager.cpp

libobjectmodel_a_SOURCES = \
		ObjectModel/Component.cpp \
		ObjectModel/ComponentTemplate.cpp \
		ObjectModel/ComponentTemplateManager.cpp \
		ObjectModel/Object.cpp \
		ObjectModel/ObjectTemplate.cpp \
		ObjectModel/ObjectTemplateManager.cpp

libphysicscomponents_a_SOURCES = \
		PhysicsComponents/CharacterController.cpp \
		PhysicsComponents/CharacterControllerTemplate.cpp \
		PhysicsComponents/RigidBody.cpp \
		PhysicsComponents/RigidBodyTemplate.cpp

libscenecomponents_a_SOURCES = \
		SceneComponents/InstanceOf.cpp \
		SceneComponents/InstanceOfTemplate.cpp \
		SceneComponents/Model.cpp \
		SceneComponents/ModelTemplate.cpp \
		SceneComponents/NodePathWrapper.cpp \
		SceneComponents/NodePathWrapperTemplate.cpp \
		SceneComponents/Terrain.cpp \
		SceneComponents/TerrainTemplate.cpp

libsupport_a_SOURCES = \
		Support/FirstPersonCamera.cpp \
		Support/FSM.cpp \
		Support/Picker.cpp \
		Support/Distributed/ClientRepositoryBase.cpp \
		Support/Distributed/DistributedObjectBase.cpp

libutilities_a_SOURCES = \
		Utilities/Tools.cpp

