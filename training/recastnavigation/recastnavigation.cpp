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

int main(int argc, char **argv)
{
	App* app = new App;

	app->allOnButZeroMask = allOnButZeroMask;
	app->allOffButZeroMask = allOffButZeroMask;
	//
	app->allOnButZeroMask = BitMask32::all_on();
	app->allOnButZeroMask.clear_bit(0);
	app->allOffButZeroMask = BitMask32::all_off();
	app->allOffButZeroMask.set_bit(0);
//	std::cout << app->allOnButZeroMask << std::endl;
//	std::cout << app->allOffButZeroMask << std::endl;
//	std::cout << (app->allOnButZeroMask & app->allOffButZeroMask) << std::endl;

///use getopt: -r(recast), -c(character), -k(kinematic with z raycast),
///		-d(debug), -s(solo), t(tile), -o(obstacles)
	app->sampleType = SOLO;
	app->agentPos = agentPos * meshScale;
#ifndef WITHCHARACTER
	app->movType = RECAST;
#else
	app->movType = CHARACTER;
#endif
	app->debugPhysics = false;
	int c;
	opterr = 0;
	while ((c = getopt(argc, argv, "rcbkdsto")) != -1)
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
		case '?':
			if (isprint(optopt))
				std::cerr << "Unknown option " << optopt << std::endl;
			else
				std::cerr << "Unknown option character " << optopt << std::endl;
			return 1;
		default:
			abort();
		}
	}
	//start
	app->mBulletWorld = start(&(app->panda), argc, argv, &(app->window), app->debugPhysics);

#ifdef DEBUG_DRAW
	//set a debug node path
	app->renderDebug = app->window->get_render().attach_new_node("renderDebug");
	app->renderDebug.set_bin("fixed", 10);
	//set transparency attrib on render
//	renderDebug.set_transparency(TransparencyAttrib::M_alpha);
#endif

	//Create world mesh
	app->worldMesh = createWorldMesh(app->mBulletWorld, app->window, meshScale);
//	worldMesh.hide();

//create a global ray caster
	new Raycaster(app->panda, app->window, app->mBulletWorld, CALLBACKSNUM);

	//Create a character
	app->cs = NULL;
	app->character = createCharacter(app->mBulletWorld, app->window, app->movType,
			app->characterRadius, app->characterHeight, &(app->cs));

	///RN common
	app->rn = new RN(app->window->get_render(), app->mBulletWorld);
	//load geometry mesh
	app->rn->loadGeomMesh(rnDir, meshNameObj, meshScale);

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
		app->tileSettings.m_tileSize = 32;
		app->tileSettings.m_maxTiles = 128;
		app->tileSettings.m_maxPolysPerTile = 32768;
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
	app->settings.m_agentMaxSlope = 60.0;
	app->settings.m_agentMaxClimb = 2.5;
	app->settings.m_cellSize = 0.3;
	app->settings.m_cellHeight = 0.2;
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

