AM_CPPFLAGS = -iquote$(top_srcdir)/include -iquote$(top_srcdir)/tinyxml2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

#ltlibraries
noinst_LTLIBRARIES = \
		libgame.la \
		libaicomponents.la \
		libaudiocomponents.la \
		libbehaviorcomponents.la \
		libcommoncomponents.la \
		libcontrolcomponents.la \
		libphysicscomponents.la \
		libphysicscontrolcomponents.la \
		libscenecomponents.la \
		libobjectmodel.la \
		libutilities.la

#libraries
noinst_LIBRARIES = \
		libsupport.a \
		librecastnavigation.a \
		libopensteer.a \
		libbullet.a

#ltlibraries sources
libaicomponents_la_SOURCES = \
		AIComponents/CrowdAgent.cpp \
		AIComponents/CrowdAgentTemplate.cpp \
		AIComponents/NavMesh.cpp \
		AIComponents/NavMeshTemplate.cpp \
		AIComponents/SteerPlugIn.cpp \
		AIComponents/SteerPlugInTemplate.cpp \
		AIComponents/SteerVehicle.cpp \
		AIComponents/SteerVehicleTemplate.cpp

libaudiocomponents_la_SOURCES = \
		AudioComponents/Listener.cpp \
		AudioComponents/ListenerTemplate.cpp \
		AudioComponents/Sound3d.cpp \
		AudioComponents/Sound3dTemplate.cpp

libbehaviorcomponents_la_SOURCES = \
		BehaviorComponents/Activity.cpp \
		BehaviorComponents/ActivityTemplate.cpp

libcommoncomponents_la_SOURCES = \
		CommonComponents/Default.cpp \
		CommonComponents/DefaultTemplate.cpp \
		CommonComponents/GameConfig.cpp \
		CommonComponents/GameConfigTemplate.cpp

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
		PhysicsComponents/Ghost.cpp \
		PhysicsComponents/GhostTemplate.cpp \
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

#libraries sources
libsupport_a_SOURCES = \
		Support/FSM.cpp \
		Support/Picker.cpp \
		Support/Raycaster.cpp \
		Support/Distributed/ClientRepositoryBase.cpp \
		Support/Distributed/DistributedObjectBase.cpp

librecastnavigation_a_SOURCES = \
		Support/RecastNavigationLocal/InputGeom.cpp \
		Support/RecastNavigationLocal/ChunkyTriMesh.cpp \
		Support/RecastNavigationLocal/MeshLoaderObj.cpp \
		Support/RecastNavigationLocal/DebugInterfaces.cpp \
		Support/RecastNavigationLocal/PerfTimer.cpp \
		Support/RecastNavigationLocal/NavMeshType.cpp \
		Support/RecastNavigationLocal/NavMeshType_Tile.cpp \
		Support/RecastNavigationLocal/NavMeshType_Solo.cpp \
		Support/RecastNavigationLocal/NavMeshType_Obstacle.cpp \
		Support/RecastNavigationLocal/fastlz.c \
		Support/RecastNavigationLocal/ConvexVolumeTool.cpp \
		Support/RecastNavigationLocal/CrowdTool.cpp \
		Support/RecastNavigationLocal/OffMeshConnectionTool.cpp

libopensteer_a_SOURCES = \
		Support/OpenSteerLocal/Draw.cpp \
		Support/OpenSteerLocal/DrawMeshDrawer.cpp \
		Support/OpenSteerLocal/SimpleVehicle.cpp

libbullet_a_SOURCES = \
		Support/BulletLocal/bulletAllHitsRayResult.cpp \
		Support/BulletLocal/bulletBaseCharacterControllerNode.cpp \
		Support/BulletLocal/bulletBodyNode.cpp \
		Support/BulletLocal/bulletBoxShape.cpp \
		Support/BulletLocal/bulletCapsuleShape.cpp \
		Support/BulletLocal/bulletCharacterControllerNode.cpp \
		Support/BulletLocal/bulletClosestHitRayResult.cpp \
		Support/BulletLocal/bulletClosestHitSweepResult.cpp \
		Support/BulletLocal/bulletConeShape.cpp \
		Support/BulletLocal/bulletConeTwistConstraint.cpp \
		Support/BulletLocal/bulletConstraint.cpp \
		Support/BulletLocal/bulletContactCallbackData.cpp \
		Support/BulletLocal/bulletContactResult.cpp \
		Support/BulletLocal/bulletConvexHullShape.cpp \
		Support/BulletLocal/bulletConvexPointCloudShape.cpp \
		Support/BulletLocal/bulletCylinderShape.cpp \
		Support/BulletLocal/bulletDebugNode.cpp \
		Support/BulletLocal/bulletFilterCallbackData.cpp \
		Support/BulletLocal/bulletGenericConstraint.cpp \
		Support/BulletLocal/bulletGhostNode.cpp \
		Support/BulletLocal/bulletHeightfieldShape.cpp \
		Support/BulletLocal/bulletHelper.cpp \
		Support/BulletLocal/bulletHingeConstraint.cpp \
		Support/BulletLocal/bulletManifoldPoint.cpp \
		Support/BulletLocal/bulletMinkowskiSumShape.cpp \
		Support/BulletLocal/bulletMultiSphereShape.cpp \
		Support/BulletLocal/bulletPersistentManifold.cpp \
		Support/BulletLocal/bulletPlaneShape.cpp \
		Support/BulletLocal/bulletRigidBodyNode.cpp \
		Support/BulletLocal/bulletRotationalLimitMotor.cpp \
		Support/BulletLocal/bulletShape.cpp \
		Support/BulletLocal/bulletSliderConstraint.cpp \
		Support/BulletLocal/bulletSoftBodyConfig.cpp \
		Support/BulletLocal/bulletSoftBodyControl.cpp \
		Support/BulletLocal/bulletSoftBodyMaterial.cpp \
		Support/BulletLocal/bulletSoftBodyNode.cpp \
		Support/BulletLocal/bulletSoftBodyShape.cpp \
		Support/BulletLocal/bulletSoftBodyWorldInfo.cpp \
		Support/BulletLocal/bulletSphereShape.cpp \
		Support/BulletLocal/bulletSphericalConstraint.cpp \
		Support/BulletLocal/bulletTickCallbackData.cpp \
		Support/BulletLocal/bulletTranslationalLimitMotor.cpp \
		Support/BulletLocal/bulletTriangleMesh.cpp \
		Support/BulletLocal/bulletTriangleMeshShape.cpp \
		Support/BulletLocal/bullet_utils.cpp \
		Support/BulletLocal/bulletVehicle.cpp \
		Support/BulletLocal/bulletWheel.cpp \
		Support/BulletLocal/bulletWorld.cpp \
		Support/BulletLocal/config_bullet.cpp