# recastnavigation tests
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = \
	-I$(srcdir)/../../source \
	-I$(srcdir)/../../source/library_os \
	-I$(srcdir)/../../source/support_os \
	-I$(srcdir)/../../source/library_rn \
	-I$(srcdir)/../../source/support_rn \
	-I/usr/include/panda3d -I/usr/include/eigen3 \
	-DELY_DEBUG=

AM_CXXFLAGS = -O0 -g3 -Wall -Wno-reorder -fmessage-length=0 -std=c++11
AM_LDFLAGS = -L/usr/lib/x86_64-linux-gnu/panda3d

LDADD = -lp3framework -lpanda -lpandaexpress -lp3dtool -lp3dtoolconfig \
		-lp3pystub -lXxf86dga -lpython2.7 -lpandaphysics -lpthread

noinst_HEADERS = common.h

noinst_PROGRAMS = os_basic os_callback_test rn_basic rn_callback_test test1 test2 \
	low_speed_turn pedestrian boid multiple_pursuit capture_the_flag map_drive \
	pedestrian_external soccer #soccer_gprof soccer_gcov

BUILT_SOURCES = data.cpp

substDataDir = sed -e 's|@sampledatadir[@]|$(srcdir)/../data|g'

data.cpp: $(srcdir)/data.cpp.in Makefile
	$(substDataDir) $(srcdir)/$@.in > $@
	
EXTRA_DIST = data.cpp.in

common_sources = \
	data.cpp \
	$(srcdir)/../../source/aiManager.cxx \
	$(srcdir)/../../source/aiTools.cxx \
	$(srcdir)/../../source/rnCrowdAgent.cxx \
	$(srcdir)/../../source/rnNavMesh.cxx \
	$(srcdir)/../../source/osSteerPlugIn.cxx \
	$(srcdir)/../../source/osSteerVehicle.cxx \
	$(srcdir)/../../source/library_rn/DebugDraw.cpp \
	$(srcdir)/../../source/library_rn/DetourAlloc.cpp \
	$(srcdir)/../../source/library_rn/DetourCommon.cpp \
	$(srcdir)/../../source/library_rn/DetourCrowd.cpp \
	$(srcdir)/../../source/library_rn/DetourDebugDraw.cpp \
	$(srcdir)/../../source/library_rn/DetourLocalBoundary.cpp \
	$(srcdir)/../../source/library_rn/DetourNavMeshBuilder.cpp \
	$(srcdir)/../../source/library_rn/DetourNavMesh.cpp \
	$(srcdir)/../../source/library_rn/DetourNavMeshQuery.cpp \
	$(srcdir)/../../source/library_rn/DetourNode.cpp \
	$(srcdir)/../../source/library_rn/DetourObstacleAvoidance.cpp \
	$(srcdir)/../../source/library_rn/DetourPathCorridor.cpp \
	$(srcdir)/../../source/library_rn/DetourPathQueue.cpp \
	$(srcdir)/../../source/library_rn/DetourProximityGrid.cpp \
	$(srcdir)/../../source/library_rn/DetourTileCacheBuilder.cpp \
	$(srcdir)/../../source/library_rn/DetourTileCache.cpp \
	$(srcdir)/../../source/library_rn/RecastAlloc.cpp \
	$(srcdir)/../../source/library_rn/RecastArea.cpp \
	$(srcdir)/../../source/library_rn/RecastContour.cpp \
	$(srcdir)/../../source/library_rn/Recast.cpp \
	$(srcdir)/../../source/library_rn/RecastDebugDraw.cpp \
	$(srcdir)/../../source/library_rn/RecastDump.cpp \
	$(srcdir)/../../source/library_rn/RecastFilter.cpp \
	$(srcdir)/../../source/library_rn/RecastLayers.cpp \
	$(srcdir)/../../source/library_rn/RecastMesh.cpp \
	$(srcdir)/../../source/library_rn/RecastMeshDetail.cpp \
	$(srcdir)/../../source/library_rn/RecastRasterization.cpp \
	$(srcdir)/../../source/library_rn/RecastRegion.cpp \
	$(srcdir)/../../source/support_rn/ChunkyTriMesh.cpp \
	$(srcdir)/../../source/support_rn/ConvexVolumeTool.cpp \
	$(srcdir)/../../source/support_rn/CrowdTool.cpp \
	$(srcdir)/../../source/support_rn/DebugInterfaces.cpp \
	$(srcdir)/../../source/support_rn/fastlz.c \
	$(srcdir)/../../source/support_rn/InputGeom.cpp \
	$(srcdir)/../../source/support_rn/MeshLoaderObj.cpp \
	$(srcdir)/../../source/support_rn/NavMeshTesterTool.cpp \
	$(srcdir)/../../source/support_rn/NavMeshType.cpp \
	$(srcdir)/../../source/support_rn/NavMeshType_Obstacle.cpp \
	$(srcdir)/../../source/support_rn/NavMeshType_Solo.cpp \
	$(srcdir)/../../source/support_rn/NavMeshType_Tile.cpp \
	$(srcdir)/../../source/support_rn/OffMeshConnectionTool.cpp \
	$(srcdir)/../../source/support_rn/PerfTimer.cpp \
	common.cpp \
	$(srcdir)/../../source/library_os/OpenSteer/Camera.cpp \
	$(srcdir)/../../source/library_os/OpenSteer/Clock.cpp \
	$(srcdir)/../../source/library_os/OpenSteer/Color.cpp \
	$(srcdir)/../../source/library_os/OpenSteer/lq.c \
	$(srcdir)/../../source/library_os/OpenSteer/Obstacle.cpp \
	$(srcdir)/../../source/library_os/OpenSteer/OldPathway.cpp \
	$(srcdir)/../../source/library_os/OpenSteer/Path.cpp \
	$(srcdir)/../../source/library_os/OpenSteer/Pathway.cpp \
	$(srcdir)/../../source/library_os/OpenSteer/PlugIn.cpp \
	$(srcdir)/../../source/library_os/OpenSteer/PolylineSegmentedPath.cpp \
	$(srcdir)/../../source/library_os/OpenSteer/PolylineSegmentedPathwaySegmentRadii.cpp \
	$(srcdir)/../../source/library_os/OpenSteer/PolylineSegmentedPathwaySingleRadius.cpp \
	$(srcdir)/../../source/library_os/OpenSteer/SegmentedPath.cpp \
	$(srcdir)/../../source/library_os/OpenSteer/SegmentedPathway.cpp \
	$(srcdir)/../../source/library_os/OpenSteer/TerrainRayTest.cpp \
	$(srcdir)/../../source/library_os/OpenSteer/Vec3.cpp \
	$(srcdir)/../../source/library_os/OpenSteer/Vec3Utilities.cpp \
	$(srcdir)/../../source/support_os/Draw.cpp \
	$(srcdir)/../../source/support_os/DrawMeshDrawer.cpp \
	$(srcdir)/../../source/support_os/SimpleVehicle.cpp
#	$(srcdir)/../../source/library_os/OpenSteer/main.cpp
#	$(srcdir)/../../source/library_os/OpenSteer/OpenSteerDemo.cpp
#	$(srcdir)/../../source/library_os/OpenSteer/Draw.cpp
#	$(srcdir)/../../source/library_os/OpenSteer/SimpleVehicle.cpp

#os_basic
os_basic_SOURCES = \
	os_basic.cpp

nodist_os_basic_SOURCES = $(common_sources)

#os_callback_test
os_callback_test_SOURCES = \
	os_callback_test.cpp

nodist_os_callback_test_SOURCES = $(common_sources)

#low_speed_turn
low_speed_turn_SOURCES = \
	low_speed_turn.cpp

nodist_low_speed_turn_SOURCES = $(common_sources)

#pedestrian
pedestrian_SOURCES = \
	pedestrian.cpp

nodist_pedestrian_SOURCES = $(common_sources)

#boid
boid_SOURCES = \
	boid.cpp

nodist_boid_SOURCES = $(common_sources)

#multiple_pursuit
multiple_pursuit_SOURCES = \
	multiple_pursuit.cpp

nodist_multiple_pursuit_SOURCES = $(common_sources)

#soccer
soccer_SOURCES = \
	soccer.cpp

nodist_soccer_SOURCES = $(common_sources)

#capture_the_flag
capture_the_flag_SOURCES = \
	capture_the_flag.cpp

nodist_capture_the_flag_SOURCES = $(common_sources)

#map_drive
map_drive_SOURCES = \
	map_drive.cpp

nodist_map_drive_SOURCES = $(common_sources)

#pedestrian_external
pedestrian_external_SOURCES = \
	pedestrian_external.cpp

nodist_pedestrian_external_SOURCES = $(common_sources)

#rn_basic
rn_basic_SOURCES = \
	rn_basic.cpp

nodist_rn_basic_SOURCES = $(common_sources)

#rn_callback_test
rn_callback_test_SOURCES = \
	rn_callback_test.cpp

nodist_rn_callback_test_SOURCES = $(common_sources)

#test1
test1_SOURCES = \
	test1.cpp
	
nodist_test1_SOURCES = $(common_sources)

#test2
test2_SOURCES = \
	test2.cpp

nodist_test2_SOURCES = $(common_sources)

###PROFILE BUILDS###
PROFILE_CPPFLAGS = $(AM_CPPFLAGS) -UOS_DEBUG
GPROF_CXXFLAGS = $(AM_CXXFLAGS) -pg -g -Wextra
GCOV_CXXFLAGS = $(AM_CXXFLAGS) -fprofile-arcs -ftest-coverage
GCOV_LDADD = $(LDADD) -lgcov

#soccer_gprof soccer_gcov
soccer_gprof_CPPFLAGS = $(PROFILE_CPPFLAGS)
soccer_gcov_CPPFLAGS = $(PROFILE_CPPFLAGS)
soccer_gprof_CXXFLAGS = $(GPROF_CXXFLAGS)
soccer_gcov_CXXFLAGS = $(GCOV_CXXFLAGS)
soccer_gcov_LDADD = $(GCOV_LDADD)

soccer_gprof_SOURCES = \
	soccer.cpp
soccer_gcov_SOURCES = \
	soccer.cpp

nodist_soccer_gprof_SOURCES = $(common_sources)
nodist_soccer_gcov_SOURCES = $(common_sources)

CLEANFILES = data.cpp
