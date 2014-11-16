AM_CPPFLAGS = -iquote$(top_srcdir)/include -iquote$(top_srcdir)/include/Support/tinyxlm2 $(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

AUTOMAKE_OPTIONS = subdir-objects

#ltlibraries
pkglib_LTLIBRARIES = \
		libsupport.la \
		libbullet.la \
		librocket.la \
		libopensteer.la \
		librecastnavigation.la \
		libtinyxml2.la

#ltlibraries sources
libtinyxml2_la_SOURCES = \
		tinyxml2/tinyxml2.cpp

libsupport_la_SOURCES = \
		FSM.cpp \
		Picker.cpp \
		Raycaster.cpp \
		Distributed/ClientRepositoryBase.cpp \
		Distributed/DistributedObjectBase.cpp

libbullet_la_SOURCES = \
		BulletLocal/bulletAllHitsRayResult.cpp \
		BulletLocal/bulletBaseCharacterControllerNode.cpp \
		BulletLocal/bulletBodyNode.cpp \
		BulletLocal/bulletBoxShape.cpp \
		BulletLocal/bulletCapsuleShape.cpp \
		BulletLocal/bulletCharacterControllerNode.cpp \
		BulletLocal/bulletClosestHitRayResult.cpp \
		BulletLocal/bulletClosestHitSweepResult.cpp \
		BulletLocal/bulletConeShape.cpp \
		BulletLocal/bulletConeTwistConstraint.cpp \
		BulletLocal/bulletConstraint.cpp \
		BulletLocal/bulletContactCallbackData.cpp \
		BulletLocal/bulletContactResult.cpp \
		BulletLocal/bulletConvexHullShape.cpp \
		BulletLocal/bulletConvexPointCloudShape.cpp \
		BulletLocal/bulletCylinderShape.cpp \
		BulletLocal/bulletDebugNode.cpp \
		BulletLocal/bulletFilterCallbackData.cpp \
		BulletLocal/bulletGenericConstraint.cpp \
		BulletLocal/bulletGhostNode.cpp \
		BulletLocal/bulletHeightfieldShape.cpp \
		BulletLocal/bulletHelper.cpp \
		BulletLocal/bulletHingeConstraint.cpp \
		BulletLocal/bulletManifoldPoint.cpp \
		BulletLocal/bulletMinkowskiSumShape.cpp \
		BulletLocal/bulletMultiSphereShape.cpp \
		BulletLocal/bulletPersistentManifold.cpp \
		BulletLocal/bulletPlaneShape.cpp \
		BulletLocal/bulletRigidBodyNode.cpp \
		BulletLocal/bulletRotationalLimitMotor.cpp \
		BulletLocal/bulletShape.cpp \
		BulletLocal/bulletSliderConstraint.cpp \
		BulletLocal/bulletSoftBodyConfig.cpp \
		BulletLocal/bulletSoftBodyControl.cpp \
		BulletLocal/bulletSoftBodyMaterial.cpp \
		BulletLocal/bulletSoftBodyNode.cpp \
		BulletLocal/bulletSoftBodyShape.cpp \
		BulletLocal/bulletSoftBodyWorldInfo.cpp \
		BulletLocal/bulletSphereShape.cpp \
		BulletLocal/bulletSphericalConstraint.cpp \
		BulletLocal/bulletTickCallbackData.cpp \
		BulletLocal/bulletTranslationalLimitMotor.cpp \
		BulletLocal/bulletTriangleMesh.cpp \
		BulletLocal/bulletTriangleMeshShape.cpp \
		BulletLocal/bullet_utils.cpp \
		BulletLocal/bulletVehicle.cpp \
		BulletLocal/bulletWheel.cpp \
		BulletLocal/bulletWorld.cpp \
		BulletLocal/config_bullet.cpp

librocket_la_SOURCES = \
		LibRocketLocal/config_rocket.cxx \
		LibRocketLocal/rocketSystemInterface.cxx \
		LibRocketLocal/rocketInputHandler.cxx \
		LibRocketLocal/rocketRenderInterface.cxx \
		LibRocketLocal/rocketRegion.cxx \
		LibRocketLocal/rocketFileInterface.cxx

libopensteer_la_SOURCES = \
		OpenSteerLocal/Draw.cpp \
		OpenSteerLocal/DrawMeshDrawer.cpp \
		OpenSteerLocal/SimpleVehicle.cpp

librecastnavigation_la_SOURCES = \
		RecastNavigationLocal/InputGeom.cpp \
		RecastNavigationLocal/ChunkyTriMesh.cpp \
		RecastNavigationLocal/MeshLoaderObj.cpp \
		RecastNavigationLocal/DebugInterfaces.cpp \
		RecastNavigationLocal/PerfTimer.cpp \
		RecastNavigationLocal/NavMeshType.cpp \
		RecastNavigationLocal/NavMeshType_Tile.cpp \
		RecastNavigationLocal/NavMeshType_Solo.cpp \
		RecastNavigationLocal/NavMeshType_Obstacle.cpp \
		RecastNavigationLocal/fastlz.c \
		RecastNavigationLocal/ConvexVolumeTool.cpp \
		RecastNavigationLocal/CrowdTool.cpp \
		RecastNavigationLocal/OffMeshConnectionTool.cpp
