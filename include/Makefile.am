recastnavigation_headers = \
		AIComponents/RecastNavigation/common.h \
		AIComponents/RecastNavigation/InputGeom.h \
		AIComponents/RecastNavigation/ChunkyTriMesh.h \
		AIComponents/RecastNavigation/MeshLoaderObj.h \
		AIComponents/RecastNavigation/DebugInterfaces.h \
		AIComponents/RecastNavigation/PerfTimer.h \
		AIComponents/RecastNavigation/NavMeshType.h \
		AIComponents/RecastNavigation/NavMeshType_Tile.h \
		AIComponents/RecastNavigation/NavMeshType_Solo.h \
		AIComponents/RecastNavigation/NavMeshType_Obstacle.h \
		AIComponents/RecastNavigation/fastlz.h \
		AIComponents/RecastNavigation/ConvexVolumeTool.h \
		AIComponents/RecastNavigation/CrowdTool.h

noinst_HEADERS = \
		Ely.h \
		AIComponents/Steering.h \
		AIComponents/SteeringTemplate.h \
		AIComponents/CrowdAgent.h \
		AIComponents/CrowdAgentTemplate.h \
		AIComponents/NavMesh.h \
		AIComponents/NavMeshTemplate.h \
		$(recastnavigation_headers) \
		AudioComponents/Listener.h \
		AudioComponents/ListenerTemplate.h \
		AudioComponents/Sound3d.h \
		AudioComponents/Sound3dTemplate.h \
		BehaviorComponents/Activity.h \
		BehaviorComponents/ActivityTemplate.h \
		ControlComponents/Chaser.h \
		ControlComponents/ChaserTemplate.h \
		ControlComponents/Driver.h \
		ControlComponents/DriverTemplate.h \
		Game/GameAIManager.h \
		Game/GameAudioManager.h \
		Game/GameControlManager.h \
		Game/GameManager.h \
		Game/GamePhysicsManager.h \
		Game/GameSceneManager.h \
		ObjectModel/Component.h \
		ObjectModel/ComponentTemplate.h \
		ObjectModel/ComponentTemplateManager.h \
		ObjectModel/Object.h \
		ObjectModel/ObjectTemplate.h \
		ObjectModel/ObjectTemplateManager.h \
		PhysicsComponents/CharacterController.h \
		PhysicsComponents/CharacterControllerTemplate.h \
		PhysicsComponents/RigidBody.h \
		PhysicsComponents/RigidBodyTemplate.h \
		SceneComponents/InstanceOf.h \
		SceneComponents/InstanceOfTemplate.h \
		SceneComponents/Model.h \
		SceneComponents/ModelTemplate.h \
		SceneComponents/NodePathWrapper.h \
		SceneComponents/NodePathWrapperTemplate.h \
		SceneComponents/Terrain.h \
		SceneComponents/TerrainTemplate.h \
		Support/FirstPersonCamera.h \
		Support/FSM.h \
		Support/Picker.h \
		Support/Raycaster.h \
		Support/Distributed/ClientRepositoryBase.h \
		Support/Distributed/DistributedObjectBase.h \
		Utilities/ComponentSuite.h \
		Utilities/Tools.h

nodist_noinst_HEADERS =\
		config.h \
		Tools_ini.h
		
EXTRA_DIST = Utilities/Tools_ini.h.in

if !ELY_DEVEL
substDataDir = sed -e 's|@elydatadir[@]|$(pkgdatadir)|g'
substLibDir = sed -e 's|@elylibdir[@]|$(pkglibdir)|g'
substData = sed -e 's/@data[@]\///g'
substCallbacks = sed -e 's/@callbacks[@]\///g'
substTransitions = sed -e 's/@transitions[@]\///g'
substInitializations = sed -e 's/@initializations[@]\///g'
else
substDataDir = sed -e 's|@elydatadir[@]|$(abs_top_builddir)|g'
substLibDir = sed -e 's|@elylibdir[@]|$(abs_top_builddir)|g'
substData = sed -e 's/@data[@]/data/g'
substCallbacks = sed -e 's/@callbacks[@]/callbacks/g'
substTransitions = sed -e 's/@transitions[@]/transitions/g'
substInitializations = sed -e 's/@initializations[@]/initializations/g'
endif 

Tools_ini.h : $(top_srcdir)/include/Utilities/Tools_ini.h.in Makefile
	$(substDataDir) $(top_srcdir)/include/Utilities/$@.in | \
	$(substLibDir) | \
	$(substData) | \
	$(substCallbacks) | \
	$(substTransitions) | \
	$(substInitializations) > $@

CLEANFILES = Tools_ini.h
	