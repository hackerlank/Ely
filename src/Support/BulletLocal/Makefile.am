AM_CPPFLAGS = -iquote$(top_srcdir)/include \
	$(ELY_CPPFLAGS)
AM_CXXFLAGS = $(ELY_CXXFLAGS)
AM_LDFLAGS = $(ELY_LDFLAGS)

LIBS = $(ELY_LIBS)

AUTOMAKE_OPTIONS = subdir-objects

#convenience library
noinst_LTLIBRARIES = \
	libBulletLocal.la

#library sources
libBulletLocal_la_SOURCES = \
	bulletAllHitsRayResult.cpp \
	bulletBaseCharacterControllerNode.cpp \
	bulletBodyNode.cpp \
	bulletBoxShape.cpp \
	bulletCapsuleShape.cpp \
	bulletCharacterControllerNode.cpp \
	bulletClosestHitRayResult.cpp \
	bulletClosestHitSweepResult.cpp \
	bulletConeShape.cpp \
	bulletConeTwistConstraint.cpp \
	bulletConstraint.cpp \
	bulletContactCallbackData.cpp \
	bulletContactResult.cpp \
	bulletConvexHullShape.cpp \
	bulletConvexPointCloudShape.cpp \
	bulletCylinderShape.cpp \
	bulletDebugNode.cpp \
	bulletFilterCallbackData.cpp \
	bulletGenericConstraint.cpp \
	bulletGhostNode.cpp \
	bulletHeightfieldShape.cpp \
	bulletHelper.cpp \
	bulletHingeConstraint.cpp \
	bulletManifoldPoint.cpp \
	bulletMinkowskiSumShape.cpp \
	bulletMultiSphereShape.cpp \
	bulletPersistentManifold.cpp \
	bulletPlaneShape.cpp \
	bulletRigidBodyNode.cpp \
	bulletRotationalLimitMotor.cpp \
	bulletShape.cpp \
	bulletSliderConstraint.cpp \
	bulletSoftBodyConfig.cpp \
	bulletSoftBodyControl.cpp \
	bulletSoftBodyMaterial.cpp \
	bulletSoftBodyNode.cpp \
	bulletSoftBodyShape.cpp \
	bulletSoftBodyWorldInfo.cpp \
	bulletSphereShape.cpp \
	bulletSphericalConstraint.cpp \
	bulletTickCallbackData.cpp \
	bulletTranslationalLimitMotor.cpp \
	bulletTriangleMesh.cpp \
	bulletTriangleMeshShape.cpp \
	bullet_utils.cpp \
	bulletVehicle.cpp \
	bulletWheel.cpp \
	bulletWorld.cpp \
	config_bullet.cpp
