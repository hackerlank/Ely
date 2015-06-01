memorypool_headers = \
	Support/MemoryPool/MemoryMacros.h \
	Support/MemoryPool/MemoryPool.h
		
opensteer_headers = \
	Support/OpenSteerLocal/common.h \
	Support/OpenSteerLocal/DrawMeshDrawer.h \
	Support/OpenSteerLocal/PlugIn_Boids.h \
	Support/OpenSteerLocal/PlugIn_MultiplePursuit.h \
	Support/OpenSteerLocal/PlugIn_OneTurning.h \
	Support/OpenSteerLocal/PlugIn_Pedestrian.h \
	Support/OpenSteerLocal/PlugIn_Soccer.h \
	Support/OpenSteerLocal/PlugIn_CaptureTheFlag.h \
	Support/OpenSteerLocal/PlugIn_LowSpeedTurn.h \
	Support/OpenSteerLocal/PlugIn_MapDrive.h \
	Support/OpenSteerLocal/SimpleVehicle.h

recastnavigation_headers = \
	Support/RecastNavigationLocal/common.h \
	Support/RecastNavigationLocal/InputGeom.h \
	Support/RecastNavigationLocal/ChunkyTriMesh.h \
	Support/RecastNavigationLocal/MeshLoaderObj.h \
	Support/RecastNavigationLocal/DebugInterfaces.h \
	Support/RecastNavigationLocal/PerfTimer.h \
	Support/RecastNavigationLocal/NavMeshType.h \
	Support/RecastNavigationLocal/NavMeshType_Tile.h \
	Support/RecastNavigationLocal/NavMeshType_Solo.h \
	Support/RecastNavigationLocal/NavMeshType_Obstacle.h \
	Support/RecastNavigationLocal/fastlz.h \
	Support/RecastNavigationLocal/ConvexVolumeTool.h \
	Support/RecastNavigationLocal/CrowdTool.h \
	Support/RecastNavigationLocal/OffMeshConnectionTool.h
	
tinyxml2_headers = \
	Support/tinyxlm2/tinyxml2.h

#noinst headers
noinst_HEADERS = \
	$(memorypool_headers) \
	$(opensteer_headers) \
	$(recastnavigation_headers) \
	$(tinyxml2_headers)

#headers
nobase_pkginclude_HEADERS = \
	AIComponents/CrowdAgent.h \
	AIComponents/NavMesh.h \
	AIComponents/SteerPlugIn.h \
	AIComponents/SteerVehicle.h \
	AudioComponents/Listener.h \
	AudioComponents/Sound3d.h \
	BehaviorComponents/Activity.h \
	CommonComponents/Default.h \
	CommonComponents/GameConfig.h \
	ControlComponents/Chaser.h \
	ControlComponents/Driver.h \
	Game/GameAIManager.h \
	Game/GameAudioManager.h \
	Game/GameBehaviorManager.h \
	Game/GameControlManager.h \
	Game/GameGUIManager.h \
	Game/GameManager.h \
	Game/GamePhysicsManager.h \
	Game/GameSceneManager.h \
	ObjectModel/Component.h \
	ObjectModel/ComponentTemplateManager.h \
	ObjectModel/Object.h \
	ObjectModel/ObjectTemplateManager.h \
	PhysicsComponents/Ghost.h \
	PhysicsComponents/RigidBody.h \
	PhysicsComponents/SoftBody.h \
	PhysicsControlComponents/CharacterController.h \
	PhysicsControlComponents/Vehicle.h \
	SceneComponents/InstanceOf.h \
	SceneComponents/Model.h \
	SceneComponents/NodePathWrapper.h \
	SceneComponents/Terrain.h \
	Support/FSM.h \
	Support/Picker.h \
	Support/Raycaster.h \
	Utilities/ComponentSuite.h \
	Utilities/Tools.h

nodist_noinst_HEADERS = \
	config.h
