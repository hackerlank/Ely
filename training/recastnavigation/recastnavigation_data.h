/*
 *   This file is part of Ely.
 *
 *   Ely is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Ely is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Ely.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * \file /Ely/training/recastnavigation/recastnavigation_data.h
 *
 * \date 24/mag/2013 (13:25:56)
 * \author consultit
 */


#ifndef RECASTNAVIGATION_DATA_H_
#define RECASTNAVIGATION_DATA_H_

#include "Utilities/Tools.h"
#include "Support/Raycaster.h"
#include <unistd.h>
#include <cctype>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <load_prc_file.h>
#include <auto_bind.h>
#include <partBundleHandle.h>
#include <character.h>
#include <animControlCollection.h>
#include <pandaFramework.h>
#include "Support/BulletLocal/bulletWorld.h"
#include "Support/BulletLocal/bulletClosestHitRayResult.h"
#include "Support/BulletLocal/bulletSphericalConstraint.h"
#include "Support/BulletLocal/bulletRigidBodyNode.h"
#include "Support/BulletLocal/bulletCharacterControllerNode.h"
#include "Support/BulletLocal/bulletCylinderShape.h"
#include "Support/BulletLocal/bulletBoxShape.h"
#include "Support/BulletLocal/bulletTriangleMeshShape.h"
#include "Support/BulletLocal/bulletTriangleMesh.h"
#include <mouseWatcher.h>
#include <lvector3.h>
#include <nodePath.h>
#include <genericAsyncTask.h>

//#define TESTANOMALIES
#define DEBUG_DRAW

//https://groups.google.com/forum/?fromgroups=#!searchin/recastnavigation/z$20axis/recastnavigation/fMqEAqSBOBk/zwOzHmjRsj0J
inline void LVecBase3fToRecast(const LVecBase3f& v, float* p)
{
	p[0] = v.get_x();
	p[1] = v.get_z();
	p[2] = -v.get_y();
}
inline LVecBase3f RecastToLVecBase3f(const float* p)
{
	return LVecBase3f(p[0], -p[2], p[1]);
}
inline LVecBase3f Recast3fToLVecBase3f(const float x, const float y, const float z)
{
	return LVecBase3f(x, -z, y);
}

//Movement type
enum MOVTYPE
{
#ifndef WITHCHARACTER
	RECAST, KINEMATIC, RIGID
#else
	CHARACTER
#endif
};

#ifdef DEBUG_DRAW
#	define PRINTDRAW(msg) std::cout << msg << std::endl
#else
#	define PRINTDRAW
#endif

///Data constants
extern std::string baseDir;
extern std::string rnDir;
///
//convert obj to egg: obj2egg -TR 90,0,0 nav_test.obj -o nav_test_panda.egg
//triangulate nav_test_panda.egg and...
//...(re)convert egg to obj:
//egg2obj -cs y-up -o nav_test_panda.obj nav_test_panda.egg

///dungeon || nav_test || ...
extern std::string meshNameEggDefault;
extern LPoint3f agentPosDefault;

///eve actor
extern std::string actorFile;
extern std::string anim0File;
extern std::string anim1File;
extern const float characterMaxSpeed;
extern const float rateFactor;
extern const float characterScale;
///guy actor
//extern std::string actorFile;
//extern std::string anim0File;
//extern const float agentMaxSpeed;
//extern const float rateFactor;
//extern const float actorScale;

extern const int AI_TASK_SORT;
extern const int PHYSICS_TASK_SORT;

extern BitMask32 allOnButZeroMask, allOffButZeroMask;

//Declarations
// don't use PT or CPT with AnimControlCollection
extern AnimControlCollection rn_anim_collection;
AsyncTask::DoneStatus update_physics(GenericAsyncTask* task, void* data);
SMARTPTR(BulletWorld)start(PandaFramework** panda, int argc, char **argv, WindowFramework** window, bool debugPhysics);
void end(PandaFramework* panda);
NodePath createWorldMesh(const std::string& meshNameEgg, SMARTPTR(BulletWorld)mBulletWorld, WindowFramework* window, float scale);
NodePath createCharacter(SMARTPTR(BulletWorld)mBulletWorld, WindowFramework* window,
MOVTYPE movType, float& agentRadius, float& agentHeight, BulletConstraint** pcs);

#endif /* RECASTNAVIGATION_DATA_H_ */
