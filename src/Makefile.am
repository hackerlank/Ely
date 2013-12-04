AM_CPPFLAGS = -iquote$(top_srcdir)/include -iquote$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

noinst_LTLIBRARIES = \
		libgame.la \
		libaicomponents.la \
		libaudiocomponents.la \
		libbehaviorcomponents.la \
		libcontrolcomponents.la \
		libphysicscomponents.la \
		libphysicscontrolcomponents.la \
		libscenecomponents.la \
		libobjectmodel.la \
		libutilities.la

noinst_LIBRARIES = \
		libsupport.a

recastnavigation_sources = \
		AIComponents/RecastNavigation/InputGeom.cpp \
		AIComponents/RecastNavigation/ChunkyTriMesh.cpp \
		AIComponents/RecastNavigation/MeshLoaderObj.cpp \
		AIComponents/RecastNavigation/DebugInterfaces.cpp \
		AIComponents/RecastNavigation/PerfTimer.cpp \
		AIComponents/RecastNavigation/NavMeshType.cpp \
		AIComponents/RecastNavigation/NavMeshType_Tile.cpp \
		AIComponents/RecastNavigation/NavMeshType_Solo.cpp \
		AIComponents/RecastNavigation/NavMeshType_Obstacle.cpp \
		AIComponents/RecastNavigation/fastlz.c \
		AIComponents/RecastNavigation/ConvexVolumeTool.cpp \
		AIComponents/RecastNavigation/CrowdTool.cpp \
		AIComponents/RecastNavigation/OffMeshConnectionTool.cpp

opensteer_sources = \
		AIComponents/OpenSteerLocal/Draw.cpp \
		AIComponents/OpenSteerLocal/DrawMeshDrawer.cpp
			
libaicomponents_la_SOURCES = \
		AIComponents/Steering.cpp  \
		AIComponents/SteeringTemplate.cpp \
		AIComponents/CrowdAgent.cpp \
		AIComponents/CrowdAgentTemplate.cpp \
		AIComponents/NavMesh.cpp \
		AIComponents/NavMeshTemplate.cpp \
		AIComponents/OpenSteerPlugIn.cpp \
		AIComponents/OpenSteerPlugInTemplate.cpp \
		AIComponents/OpenSteerVehicle.cpp \
		AIComponents/OpenSteerVehicleTemplate.cpp \
		$(recastnavigation_sources)

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
		Game/GameBehaviorManager.cpp \
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
		PhysicsComponents/RigidBody.cpp \
		PhysicsComponents/RigidBodyTemplate.cpp \
		PhysicsComponents/SoftBody.cpp \
		PhysicsComponents/SoftBodyTemplate.cpp

libphysicscontrolcomponents_la_SOURCES = \
		PhysicsControlComponents/CharacterController.cpp \
		PhysicsControlComponents/CharacterControllerTemplate.cpp \
		PhysicsControlComponents/Vehicle.cpp \
		PhysicsControlComponents/VehicleTemplate.cpp

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

