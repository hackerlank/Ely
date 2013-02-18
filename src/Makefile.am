SUBDIRS = . test

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)

noinst_LTLIBRARIES = \
		libgame.la \
		libaicomponents.la \
		libaudiocomponents.la \
		libbehaviorcomponents.la \
		libcontrolcomponents.la \
		libphysicscomponents.la \
		libscenecomponents.la \
		libobjectmodel.la \
		libutilities.la \
		libsupport.la

bin_PROGRAMS = ely
ely_SOURCES = \
		Ely.cpp
ely_LDFLAGS = $(ELY_LDFLAGS) -export-dynamic
ely_LDADD = \
		$(noinst_LTLIBRARIES) \
		$(top_srcdir)/tinyxml2/libtinyxml2.la \
		-dlopen $(top_srcdir)/callbacks/callbacks.la \
		-dlopen $(top_srcdir)/initializations/initializations.la \
		-dlopen $(top_srcdir)/transitions/transitions.la \
		$(ELY_LIBS)

libaicomponents_la_SOURCES = \
		AIComponents/Steering.cpp  \
		AIComponents/SteeringTemplate.cpp

libaudiocomponents_la_SOURCES = \
		AudioComponents/Listener.cpp \
		AudioComponents/ListenerTemplate.cpp \
		AudioComponents/Sound3d.cpp \
		AudioComponents/Sound3dTemplate.cpp

libbehaviorcomponents_la_SOURCES = \
		BehaviorComponents/Activity.cpp \
		BehaviorComponents/ActivityTemplate.cpp

libcontrolcomponents_la_SOURCES = \
		ControlComponents/Chaser.cpp \
		ControlComponents/ChaserTemplate.cpp \
		ControlComponents/Driver.cpp \
		ControlComponents/DriverTemplate.cpp

libgame_la_SOURCES = \
		Game/GameAIManager.cpp \
		Game/GameAudioManager.cpp \
		Game/GameControlManager.cpp \
		Game/GameManager.cpp \
		Game/GamePhysicsManager.cpp \
		Game/GameSceneManager.cpp

libobjectmodel_la_SOURCES = \
		ObjectModel/Component.cpp \
		ObjectModel/ComponentTemplate.cpp \
		ObjectModel/ComponentTemplateManager.cpp \
		ObjectModel/Object.cpp \
		ObjectModel/ObjectTemplate.cpp \
		ObjectModel/ObjectTemplateManager.cpp

libphysicscomponents_la_SOURCES = \
		PhysicsComponents/CharacterController.cpp \
		PhysicsComponents/CharacterControllerTemplate.cpp \
		PhysicsComponents/RigidBody.cpp \
		PhysicsComponents/RigidBodyTemplate.cpp

libscenecomponents_la_SOURCES = \
		SceneComponents/InstanceOf.cpp \
		SceneComponents/InstanceOfTemplate.cpp \
		SceneComponents/Model.cpp \
		SceneComponents/ModelTemplate.cpp \
		SceneComponents/NodePathWrapper.cpp \
		SceneComponents/NodePathWrapperTemplate.cpp \
		SceneComponents/Terrain.cpp \
		SceneComponents/TerrainTemplate.cpp

libsupport_la_SOURCES = \
		Support/FirstPersonCamera.cpp \
		Support/FSM.cpp \
		Support/Picker.cpp \
		Support/Distributed/ClientRepositoryBase.cpp \
		Support/Distributed/DistributedObjectBase.cpp

libutilities_la_SOURCES = \
		Utilities/Tools.cpp
