/**
 * \file common.cpp
 *
 * \date 2016-06-20
 * \author consultit
 */

#include "common.h"

///global data
extern CollideMask mask;
AsyncTask* updateTask;
extern bool toggleDebugFlag;
static GeoMipTerrain* terrain;
static LPoint3f terrainRootNetPos;
#define DEFAULT_MAXVALUE 1.0
//models and animations
string vehicleFile[5] =
{ "eve.egg", "ralph.egg", "sparrow.egg", "ball.egg", "red_car.egg" };
string vehicleAnimFiles[5][2] =
{
{ "eve-walk.egg", "eve-run.egg" },
{ "ralph-walk.egg", "ralph-run.egg" },
{ "sparrow-flying.egg", "sparrow-flying2.egg" },
{ "", "" },
{ "red_car-anim.egg", "red_car-anim2.egg" }};
const float animRateFactor[2] =
{ 1.80, 5.80 };
//obstacle model
string obstacleFile("plants2.egg");
//bame file
string bamFileName("control.boo");
//support
random_device rd;

///functions' definitions
// start base framework
void startFramework(int argc, char *argv[], const string& msg)
{
	// Load your application's configuration
	load_prc_file_data("", "model-path " + dataDir);
	load_prc_file_data("", "win-size 1024 768");
	load_prc_file_data("", "show-frame-rate-meter #t");
	load_prc_file_data("", "sync-video #t");
	// Setup your application
	framework.open_framework(argc, argv);
	framework.set_window_title("p3opensteer: " + msg);
	window = framework.open_window();
	if (window != (WindowFramework *) nullptr)
	{
		cout << "Opened the window successfully!\n";
		window->enable_keyboard();
		window->setup_trackball();
	}

	/// typed object init; not needed if you build inside panda source tree
	P3Driver::init_type();
	ControlManager::init_type();
	P3Driver::register_with_read_factory();
	///

	//common callbacks
}

// load plane stuff
NodePath loadPlane(const string& name, float widthX, float widthY)
{
	CardMaker cm("plane");
	cm.set_frame(-widthX / 2.0, widthX / 2.0, -widthY / 2.0, widthY / 2.0);
	NodePath plane(cm.generate());
	plane.set_p(-90.0);
	plane.set_z(0.0);
	plane.set_color(0.15, 0.35, 0.35);
	plane.set_collide_mask(mask);
	plane.set_name(name);
	return plane;
}

// load terrain low poly stuff
NodePath loadTerrainLowPoly(const string& name, float widthScale,
		float heightScale, const string& texture)
{
	NodePath terrainNP = window->load_model(framework.get_models(),
			"terrain-low-poly.egg");
	terrainNP.set_name(name);
	terrainNP.set_scale(widthScale, widthScale, heightScale);
	PT(Texture)tex =
	TexturePool::load_texture(Filename(texture));
	terrainNP.set_texture(tex);
	return terrainNP;
}

// terrain update
static AsyncTask::DoneStatus terrainUpdate(GenericAsyncTask* task, void* data)
{
	//set focal point
	//see https://www.panda3d.org/forums/viewtopic.php?t=5384
	LPoint3f focalPointNetPos =
			window->get_camera_group().get_net_transform()->get_pos();
	terrain->set_focal_point(focalPointNetPos - terrainRootNetPos);
	//update every frame
	terrain->update();
	//
	return AsyncTask::DS_cont;
}

// load terrain stuff
NodePath loadTerrain(const string& name, float widthScale, float heightScale)
{
	GeoMipTerrain *terrain = new GeoMipTerrain("terrain");
	PNMImage heightField(Filename(dataDir + string("/heightfield.png")));
	terrain->set_heightfield(heightField);
	//sizing
	float environmentWidthX = (heightField.get_x_size() - 1) * widthScale;
	float environmentWidthY = (heightField.get_y_size() - 1) * widthScale;
	float environmentWidth = (environmentWidthX + environmentWidthY) / 2.0;
	terrain->get_root().set_sx(widthScale);
	terrain->get_root().set_sy(widthScale);
	terrain->get_root().set_sz(heightScale);
	//set other terrain's properties
	unsigned short blockSize = 64, minimumLevel = 0;
	float nearPercent = 0.1, farPercent = 0.7;
	float terrainLODmin = min<float>(minimumLevel, terrain->get_max_level());
	GeoMipTerrain::AutoFlattenMode flattenMode = GeoMipTerrain::AFM_off;
	terrain->set_block_size(blockSize);
	terrain->set_near(nearPercent * environmentWidth);
	terrain->set_far(farPercent * environmentWidth);
	terrain->set_min_level(terrainLODmin);
	terrain->set_auto_flatten(flattenMode);
	//terrain texturing
	PT(TextureStage)textureStage0 = new TextureStage("TextureStage0");
	PT(Texture)textureImage = TexturePool::load_texture(
			Filename(string("terrain.png")));
	terrain->get_root().set_tex_scale(textureStage0, 1.0, 1.0);
	terrain->get_root().set_texture(textureStage0, textureImage, 1);
	terrain->get_root().set_collide_mask(mask);
	terrain->get_root().set_name(name);
	//brute force generation
	bool bruteForce = true;
	terrain->set_bruteforce(bruteForce);
	//Generate the terrain
	terrain->generate();
	//check if terrain needs update or not
	if (not bruteForce)
	{
		//save the net pos of terrain root
		terrainRootNetPos = terrain->get_root().get_net_transform()->get_pos();
		// Add a task to keep updating the terrain
		framework.get_task_mgr().add(
				new GenericAsyncTask("terrainUpdate", &terrainUpdate,
						(void*) nullptr));
	}
	//
	return terrain->get_root();
}

// throws a ray and returns the first collision entry or nullptr
PT(CollisionEntry)getCollisionEntryFromCamera()
{
	// get ai manager
	ControlManager* aiMgr = ControlManager::get_global_ptr();
	// get the mouse watcher
	PT(MouseWatcher)mwatcher = DCAST(MouseWatcher, window->get_mouse().node());
	if (mwatcher->has_mouse())
	{
		// Get to and from pos in camera coordinates
		LPoint2f pMouse = mwatcher->get_mouse();
		//
		LPoint3f pFrom, pTo;
		NodePath mCamera = window->get_camera_group();
		PT(Lens)mCamLens = DCAST(Camera, mCamera.get_child(0).node())->get_lens();
		if (mCamLens->extrude(pMouse, pFrom, pTo))
		{
			// Transform to global coordinates
			pFrom = window->get_render().get_relative_point(mCamera,
					pFrom);
			pTo = window->get_render().get_relative_point(mCamera, pTo);
			LVector3f direction = (pTo - pFrom).normalized();
			aiMgr->get_collision_ray()->set_origin(pFrom);
			aiMgr->get_collision_ray()->set_direction(direction);
			aiMgr->get_collision_traverser()->traverse(window->get_render());
			// check collisions
			if (aiMgr->get_collision_handler()->get_num_entries() > 0)
			{
				// Get the closest entry
				aiMgr->get_collision_handler()->sort_entries();
				return aiMgr->get_collision_handler()->get_entry(0);
			}
		}
	}
	return nullptr;
}

// print creation parameters
void printCreationParameters()
{
	ControlManager* steerMgr = ControlManager::get_global_ptr();
	//
	ValueList<string> valueList = steerMgr->get_parameter_name_list(
			ControlManager::DRIVER);
	cout << endl << "P3Driver creation parameters:" << endl;
	for (int i = 0; i < valueList.get_num_values(); ++i)
	{
		cout << "\t" << valueList[i] << " = "
				<< steerMgr->get_parameter_value(ControlManager::DRIVER,
						valueList[i]) << endl;
	}
	//
	valueList = steerMgr->get_parameter_name_list(ControlManager::CHASER);
	cout << endl << "P3Chaser creation parameters:" << endl;
	for (int i = 0; i < valueList.get_num_values(); ++i)
	{
		cout << "\t" << valueList[i] << " = "
				<< steerMgr->get_parameter_value(ControlManager::CHASER,
						valueList[i]) << endl;
	}
}

// return a random point on the facing upwards surface of the model
LPoint3f getRandomPos(NodePath modelNP)
{
	// collisions are made wrt render
	ControlManager* aiMgr = ControlManager::get_global_ptr();
	// get the bounding box of scene
	LVecBase3f modelDims;
	LVector3f modelDeltaCenter;
	// modelRadius not used
	aiMgr->get_bounding_dimensions(modelNP, modelDims, modelDeltaCenter);
	// throw a ray downward from a point with z = double scene's height
	// and x,y randomly within the scene's (x,y) plane
	float x, y = 0.0;
	Pair<bool, float> gotCollisionZ;
	// set the ray origin at double of maximum height of the model
	float zOrig = ((-modelDeltaCenter.get_z() + modelDims.get_z() / 2.0)
			+ modelNP.get_z()) * 2.0;
	do
	{
		x = modelDims.get_x() * ((float) rd() / (float) rd.max() - 0.5)
				- modelDeltaCenter.get_x() + modelNP.get_x();
		y = modelDims.get_y() * ((float) rd() / (float) rd.max() - 0.5)
				- modelDeltaCenter.get_y() + modelNP.get_y();
		gotCollisionZ = aiMgr->get_collision_height(LPoint3f(x, y, zOrig));

	} while (not gotCollisionZ.get_first());
	return LPoint3f(x, y, gotCollisionZ.get_second());
}

// read scene from a file
bool readFromBamFile(string fileName)
{
	return ControlManager::get_global_ptr()->read_from_bam_file(fileName);
}

// write scene to a file (and exit)
void writeToBamFileAndExit(const Event* e, void* data)
{
	string fileName = *reinterpret_cast<string*>(data);
	ControlManager::get_global_ptr()->write_to_bam_file(fileName);
	/// second option: remove custom update updateTask
	framework.get_task_mgr().remove(updateTask);

	/// this is for testing explicit removal and destruction of all elements
	WPT(ControlManager)steerMgr = ControlManager::get_global_ptr();
	// destroy drivers
	while (steerMgr->get_num_drivers() > 0)
	{
		// destroy the first one on every cycle
		steerMgr->destroy_driver(
				NodePath::any_path(steerMgr->get_driver(0)));
///		delete DCAST(OSSteerVehicle, steerMgr->get_driver(0).node()); //ERROR
	}
//	// destroy chasers
//	while (steerMgr->get_num_chasers() > 0)
//	{
//		// destroy the first one on every cycle
//		steerMgr->destroy_chaser(
//				NodePath::any_path(steerMgr->get_chaser(0)));
/////		delete DCAST(OSSteerPlugIn, steerMgr->get_chaser(0).node()); //ERROR
//	}
	///
	// delete control manager
	delete ControlManager::get_global_ptr();
	// close the window framework
	framework.close_framework();
	//
	exit(0);
}
