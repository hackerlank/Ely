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
 * \file /Ely/training/recastnavigation.cpp
 *
 * \date 14/mar/2013 10:05:13
 * \author consultit
 */

#include "RN.h"

//hack
inline unsigned int nextPow2(unsigned int v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}
inline unsigned int ilog2(unsigned int v)
{
	unsigned int r;
	unsigned int shift;
	r = (v > 0xffff) << 4; v >>= r;
	shift = (v > 0xff) << 3; v >>= shift; r |= shift;
	shift = (v > 0xf) << 2; v >>= shift; r |= shift;
	shift = (v > 0x3) << 1; v >>= shift; r |= shift;
	r |= (v >> 1);
	return r;
}

int main(int argc, char **argv)
{
	App* app = new App;

	app->allOnButZeroMask = BitMask32::all_on();
	app->allOnButZeroMask.clear_bit(0);
	app->allOffButZeroMask = BitMask32::all_off();
	app->allOffButZeroMask.set_bit(0);
//	std::cout << app->allOnButZeroMask << std::endl;
//	std::cout << app->allOffButZeroMask << std::endl;
//	std::cout << (app->allOnButZeroMask & app->allOffButZeroMask) << std::endl;
	//
	allOnButZeroMask = app->allOnButZeroMask;
	allOffButZeroMask = app->allOffButZeroMask;

///use getopt: -r(recast), -c(character), -k(kinematic with z raycast),
///		-d(debug), -s(solo), t(tile), -o(obstacles) -l scale
	app->sampleType = SOLO;
	app->meshScale = 1.0;
	app->meshPosition = LVecBase3f::zero();
#ifndef WITHCHARACTER
	app->movType = RECAST;
#else
	app->movType = CHARACTER;
#endif
	app->debugPhysics = false;
	int c;
	opterr = 0;
	while ((c = getopt(argc, argv, "rcbkdstol:x:y:z:")) != -1)
	{
		switch (c)
		{
#ifndef WITHCHARACTER
		case 'r':
			app->movType = RECAST;
			break;
		case 'k':
			app->movType = KINEMATIC;
			break;
		case 'b':
			app->movType = RIGID;
			break;
#else
			case 'c':
			app->movType = CHARACTER;
			break;
#endif
		case 'd':
			app->debugPhysics = true;
			break;
		case 's':
			app->sampleType = SOLO;
			break;
		case 't':
			app->sampleType = TILE;
			break;
		case 'o':
			app->sampleType = OBSTACLE;
			break;
		case 'l':
			app->meshScale = atof(optarg);
			break;
		case 'x':
			app->meshPosition.set_x(atof(optarg));
			break;
		case 'y':
			app->meshPosition.set_y(atof(optarg));
			break;
		case 'z':
			app->meshPosition.set_z(atof(optarg));
			break;
		case '?':
			if ((optopt == 'l') or (optopt == 'x') or (optopt == 'y')
					or (optopt == 'z'))
				std::cerr << "Option " << optopt
						<< " requires an argument.\n" << std::endl;
			else if (isprint(optopt))
				std::cerr << "Unknown option " << optopt << std::endl;
			else
				std::cerr << "Unknown option character " << optopt << std::endl;
			return 1;
		default:
			abort();
		}
	}
	if (app->meshScale <= 0.0)
	{
		app->meshScale = 1.0;
	}
	//start
	app->mBulletWorld = start(&(app->panda), argc, argv, &(app->window), app->debugPhysics);

	//Create world mesh
	app->worldMesh = createWorldMesh(app->mBulletWorld, app->window, app->meshScale);
	app->worldMesh.set_pos(app->meshPosition);
//	worldMesh.hide();
	//setup camera trackball (local coordinate)
	NodePath tballnp = app->window->get_mouse().find("**/+Trackball");
	PT(Trackball) trackball = DCAST(Trackball, tballnp.node());
	trackball->set_pos(0, 200, 0);
	trackball->set_hpr(0, 15, 0);
	//update agent pos
	app->agentPos = agentPos * app->meshScale + app->worldMesh.get_pos();

#ifdef DEBUG_DRAW
	//set a debug node path
	app->renderDebug = app->window->get_render().attach_new_node("renderDebug");
	app->renderDebug.set_bin("fixed", 10);
#endif

	//create a global ray caster
	new Raycaster(app->panda, app->window, app->mBulletWorld, CALLBACKSNUM);

	//Create a character
	app->cs = NULL;
	app->character = createCharacter(app->mBulletWorld, app->window, app->movType,
			app->characterRadius, app->characterHeight, &(app->cs));
	app->character.hide();

	///RN common
	app->rn = new RN(app->window->get_render(), app->mBulletWorld);
	//load geometry mesh
	app->rn->loadGeomMesh(rnDir, meshNameEgg, app->meshScale, app->worldMesh.get_pos());

	//create geom mesh
	switch (app->sampleType)
	{
	case SOLO:
#ifdef DEBUG_DRAW
		app->rn->createGeomMesh(new Sample_SoloMesh(app->renderDebug), SOLO);
#else
		app->rn->createGeomMesh(new Sample_SoloMesh(), SOLO);
#endif
		break;
	case TILE:
	{
#ifdef DEBUG_DRAW
		app->rn->createGeomMesh(new Sample_TileMesh(app->renderDebug), TILE);
#else
		app->rn->createGeomMesh(new Sample_TileMesh(), TILE);
#endif
		//set tile settings
		app->tileSettings =
				dynamic_cast<Sample_TileMesh*>(app->rn->getSample())->getTileSettings();
		app->tileSettings.m_buildAll = false;
		app->tileSettings.m_tileSize = 32;
		app->tileSettings.m_maxTiles = 128;
		app->tileSettings.m_maxPolysPerTile = 32768;
		dynamic_cast<Sample_TileMesh*>(app->rn->getSample())->setTileSettings(
				app->tileSettings);
	}
		break;
	case OBSTACLE:
	{
#ifdef DEBUG_DRAW
		app->rn->createGeomMesh(new Sample_TempObstacles(app->renderDebug), OBSTACLE);
#else
		app->rn->createGeomMesh(new Sample_TempObstacles(), OBSTACLE);
#endif
		//set tile settings
		app->tileSettings =
				dynamic_cast<Sample_TempObstacles*>(app->rn->getSample())->getTileSettings();
		app->tileSettings.m_tileSize = 64;
		//evaluate m_maxTiles & m_maxPolysPerTile
		const float* bmin = app->rn->getSample()->getInputGeom()->getMeshBoundsMin();
		const float* bmax = app->rn->getSample()->getInputGeom()->getMeshBoundsMax();
		//		char text[64];
		int gw = 0, gh = 0;
		rcCalcGridSize(bmin, bmax, m_cellSize, &gw, &gh);
		const int ts = (int)app->tileSettings.m_tileSize;
		const int tw = (gw + ts-1) / ts;
		const int th = (gh + ts-1) / ts;
		// Max tiles and max polys affect how the tile IDs are caculated.
		// There are 22 bits available for identifying a tile and a polygon.
		int tileBits = rcMin((int)ilog2(nextPow2(tw*th)), 14);
		if (tileBits > 14) tileBits = 14;
		int polyBits = 22 - tileBits;
		app->tileSettings.m_maxTiles = 1 << tileBits;
		app->tileSettings.m_maxPolysPerTile = 1 << polyBits;
		dynamic_cast<Sample_TempObstacles*>(app->rn->getSample())->setTileSettings(
				app->tileSettings);
	}
		break;
	default:
		break;
	}

	//set sample settings
	app->settings = app->rn->getSettings();
	app->settings.m_agentRadius = app->characterRadius;
	app->settings.m_agentHeight = app->characterHeight;
	app->settings.m_agentMaxSlope = m_agentMaxSlope;
	app->settings.m_agentMaxClimb = m_agentMaxClimb;
	app->settings.m_cellSize = m_cellSize;
	app->settings.m_cellHeight = m_cellHeight;
	app->rn->setSettings(app->settings);

	//set convex volume tool
	app->rn->setConvexVolumeTool(app->renderDebug);
	//set convex volume add/remove callbacks
	Raycaster::GetSingletonPtr()->setHitCallback(ADD_CONVEX_VOLUME_Idx,
			addConvexVolume, reinterpret_cast<void*>(app->rn), ADD_CONVEX_VOLUME_Key,
			BitMask32::all_on());
	Raycaster::GetSingletonPtr()->setHitCallback(REMOVE_CONVEX_VOLUME_Idx,
			removeConvexVolume, reinterpret_cast<void*>(app->rn), REMOVE_CONVEX_VOLUME_Key,
			BitMask32::all_on());
	//set convex volume set area type callbacks
	app->setAreaTypeCallback("a");

	//set "continue" callback (after the optionally convex volume construction)
	app->setContinueCallback("c");

	// Do the main loop
	app->panda->main_loop();
	//end
	delete app->rn;
	end(app->panda);
	delete app;
	return 0;
}

