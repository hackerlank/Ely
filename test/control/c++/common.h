/**
 * \file common.h
 *
 * \date 2016-06-20
 * \author consultit
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <random>
#include <algorithm>
#include <pandaFramework.h>
#include <auto_bind.h>
#include <load_prc_file.h>
#include <geoMipTerrain.h>
#include <texturePool.h>
#include <mouseWatcher.h>
#include <cardMaker.h>
#include <controlManager.h>
#include <p3Driver.h>

extern string dataDir;

///global data declaration
extern PandaFramework framework;
extern WindowFramework *window;
extern CollideMask mask;
extern AsyncTask* updateTask;
extern bool toggleDebugFlag;
//models and animations
extern string vehicleFile[5];
extern string vehicleAnimFiles[5][2];
extern const float animRateFactor[2];
//obstacle model
extern string obstacleFile;
//bam file
extern string bamFileName;
//support
extern random_device rd;

///functions' declarations
void startFramework(int argc, char *argv[], const string&);
NodePath loadPlane(const string&, float widthX = 30.0, float widthY = 30.0);
NodePath loadTerrainLowPoly(const string&, float widthScale = 128,
		float heightScale = 64.0, const string& texture = "dry-grass.png");
NodePath loadTerrain(const string&, float widthScale = 0.5,
		float heightScale = 10.0);
PT(CollisionEntry)getCollisionEntryFromCamera();
void printCreationParameters();
void handleVehicleEvent(const Event*, void*);
LPoint3f getRandomPos(NodePath);
bool readFromBamFile(string);
void writeToBamFileAndExit(const Event*, void*);

#endif /* COMMON_H_ */
