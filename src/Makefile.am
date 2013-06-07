AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

bin_PROGRAMS = ely

noinst_LTLIBRARIES = \
		libgame.la \
		libaicomponents.la \
		libaudiocomponents.la \
		libbehaviorcomponents.la \
		libcontrolcomponents.la \
		libphysicscomponents.la \
		libscenecomponents.la \
		libobjectmodel.la \
		libutilities.la

noinst_LIBRARIES = \
		libsupport.a
		
whole_archives = \
		-Wl,libsupport.a

#ely
ely_SOURCES = \
		Ely.cpp
		
ely_LDFLAGS = $(AM_LDFLAGS) -export-dynamic \
		-Wl,--whole-archive \
		$(whole_archives) \
		-Wl,--no-whole-archive
		
ely_LDADD = \
		$(noinst_LTLIBRARIES) \
		$(noinst_LIBRARIES) \
		$(top_builddir)/tinyxml2/libtinyxml2.la \
		-dlopen $(top_builddir)/callbacks/callbacks.la \
		-dlopen $(top_builddir)/initializations/initializations.la \
		-dlopen $(top_builddir)/transitions/transitions.la \
		$(ELY_LIBS)

libaicomponents_la_SOURCES = \
		AIComponents/Steering.cpp  \
		AIComponents/SteeringTemplate.cpp \
		AIComponents/CrowdAgent.cpp \
		AIComponents/CrowdAgentTemplate.cpp

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

libutilities_la_SOURCES = \
		Utilities/Tools.cpp

libsupport_a_SOURCES = \
		Support/FirstPersonCamera.cpp \
		Support/FSM.cpp \
		Support/Picker.cpp \
		Support/Raycaster.cpp \
		Support/Distributed/ClientRepositoryBase.cpp \
		Support/Distributed/DistributedObjectBase.cpp

